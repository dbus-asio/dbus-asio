#include <boost/thread/recursive_mutex.hpp>

#include "dbus_auth.h"
#include "dbus_log.h"
#include "dbus_transport.h"
#include "dbus_type.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>

DBus::Transport::Transport(const std::string& path)
    : m_Busname(path)
    , m_ReadyToSend(false)
    , m_ReadyToReceive(false)
    , m_socket(m_io_service)
    , m_QuitThread(false)
    , m_Thread(std::bind(&Transport::ThreadFunction, this))
{
    setOctetHandler(std::bind(&Transport::onReceiveOctet, this, std::placeholders::_1));
}

DBus::Transport::~Transport()
{
    m_QuitThread = true;
    m_Thread.join();
}

void DBus::Transport::onAuthComplete()
{
    DBus::Log::write(DBus::Log::INFO, "DBus :: Transport :: Authorisation has completed.\n");
    m_ReadyToSend = true;
    pumpMessageQueue();
}

void DBus::Transport::pumpMessageQueue()
{
    DBus::Log::write(DBus::Log::INFO, "DBus :: Transport :: pumpMessageQueue of %d messages.\n", m_BufferedMessages.size());
    boost::recursive_mutex::scoped_lock guard(m_SendMutex);

    if (m_ReadyToSend) {
        for (auto msg : m_BufferedMessages) {
            sendString(msg);
            ++m_Stats.count_messagespumped;
        }
        m_BufferedMessages.clear();
    }
}

void DBus::Transport::addToMessageQueue(const std::string& data)
{
    DBus::Log::write(DBus::Log::INFO, "DBus :: Transport :: No BEGIN has been received, so message is queued.\n");
    boost::recursive_mutex::scoped_lock guard(m_SendMutex);
    m_BufferedMessages.push_back(data);
    ++m_Stats.count_messagesqueued;
}

void DBus::Transport::sendString(const std::string& data)
{
    DBus::Log::write(DBus::Log::TRACE, "DBus :: SEND: %s\n", data.c_str());
    DBus::Log::writeHex(DBus::Log::TRACE, "DBus :: DATA: ", data);

    if (m_ReadyToSend) {
        sendStringDirect(data);
    } else {
        addToMessageQueue(data);
    }
}

void DBus::Transport::sendOctetDirect(uint8_t data)
{
    boost::recursive_mutex::scoped_lock guard(m_SendMutex);
    boost::asio::async_write(m_socket, boost::asio::buffer(&data, 1), boost::bind(&Transport::handle_write_output, this, boost::asio::placeholders::error));
}

void DBus::Transport::sendStringDirect(const std::string& data)
{
    boost::recursive_mutex::scoped_lock guard(m_SendMutex);

    //    boost::asio::async_write(*s, boost::asio::buffer(&converted_number, sizeof(converted_number)),  boost::bind(&Client::doNothing,this));

    boost::asio::async_write(m_socket, boost::asio::buffer(data.data(), data.length()),
        boost::bind(&Transport::handle_write_output, this, boost::asio::placeholders::error));
    ++m_Stats.count_messagessent;
    m_Stats.bytes_sent += data.length();
}

void DBus::Transport::handle_write_output(const boost::system::error_code& error)
{
    if (!error.value()) {
        DBus::Log::write(DBus::Log::ERROR, "DBus :: ERROR in async_write : %s\n", error.message().c_str());
    }
    DBus::Log::write(DBus::Log::ERROR, "DBus :: From async_write : %s\n", error.message().c_str());
}

void DBus::Transport::ThreadFunction()
{

    Log::write(Log::INFO, "DBus :: Transport thread starting up\n");

    m_socket.connect(m_Busname.c_str());

    // TODO: Q. Is this still needed?
    while (!m_socket.is_open()) {
    }
    // Even for protocols that don't need NUL passed first, we must pass it. Otherwise,
    // there'll be a 'connection reset by peer' sent to us.
    sendOctetDirect('\0');

    m_ReadyToReceive = true;

    while (!m_QuitThread) {
        try {
            boost::asio::read(m_socket, boost::asio::buffer(m_DataBuffer, 1));
            ++m_Stats.bytes_read;
            {
                boost::recursive_mutex::scoped_lock guard(m_CallbackMutex);
                m_ReceiveOctetCallback(m_DataBuffer[0]);
            }
        } catch (const boost::system::system_error& e) {
            Log::write(Log::ERROR, "DBus :: End of file exception\n");
            break;
        } catch (const std::exception& e) {
            // e.g. "read: End of file"
            // Note: this occurs when the master closes down the port because we've sent bad data.
            Log::write(Log::ERROR, "DBus :: Transport Exception. %s\n", e.what());
            // Log::write(Log::ERROR, "DBus :: Transport Exception. %s\n", e.type());
            break;
        }
    }

    Log::write(Log::INFO, "DBus :: Transport thread closing down\n");

    boost::system::error_code ec;
    m_socket.close(ec);
}

void DBus::Transport::setOctetHandler(const ReceiveOctetCallbackFunction& callback)
{
    boost::recursive_mutex::scoped_lock guard(m_CallbackMutex);
    m_ReceiveOctetCallback = callback;
}

void DBus::Transport::onReceiveOctet(uint8_t c)
{
    // NOP - This stub gives our initial callback somewhere to go

    // This could happen if the thread reads data before native.cpp updates the callback.
    DBus::Log::write(DBus::Log::WARNING, "DBus :: Transport : onReceiveAuthOctet is processing data, whereas it should really have been directed elsewhere via "
                                         "setOctetHandler. (data = %c %d)\n",
        c, c);
}

std::string DBus::Transport::getStats() const
{
    std::stringstream ss;

    ss << "Transport stats:" << std::endl;
    ss << " count_messages_sent: " << m_Stats.count_messagessent << std::endl;
    ss << " count_messages_queued: " << m_Stats.count_messagesqueued << std::endl;
    ss << " count_messages_pumped: " << m_Stats.count_messagespumped << std::endl;
    ss << " bytes_sent: " << m_Stats.bytes_sent << std::endl;
    ss << " bytes_read: " << m_Stats.bytes_read << std::endl;
    return ss.str();
}
