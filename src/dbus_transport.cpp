#include <boost/thread/recursive_mutex.hpp>

#include "dbus_auth.h"
#include "dbus_log.h"
#include "dbus_transport.h"
#include "dbus_type.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <chrono>

using namespace std::chrono_literals;

DBus::Transport::Transport(const std::string& path)
    : m_Busname(path)
    , m_ReadyToSend(false)
    , m_ReadyToReceive(false)
    , m_socket(m_io_service)
    , m_QuitThread(false)
{
    setOctetHandler(std::bind(&Transport::onReceiveOctet, this, std::placeholders::_1));
    std::unique_lock<std::mutex> lk(m_StartUpMutex);
    m_Thread = std::thread(std::bind(&Transport::ThreadFunction, this));
    if (!m_StartUpCondition.wait_for(lk, 5000ms, [this]() { return m_ReadyToReceive; })) {
        DBus::Log::write(DBus::Log::ERROR, "DBus :: Transport :: Transport thread start failed");
        abort();
    }
}

DBus::Transport::~Transport()
{
    {
        boost::recursive_mutex::scoped_lock guard(m_SendMutex);
        m_ReadyToSend = false;
    }
    boost::system::error_code ec;
    m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
    if (ec) {
        DBus::Log::write(DBus::Log::ERROR, "DBus :: Transport :: Socket shutdown failed: (%d) \"%s\"\n", ec.value(), ec.message());
    }
    {
        boost::recursive_mutex::scoped_lock guard(m_SendMutex);
        m_QuitThread = true;
    }
    m_Thread.join();
}

void DBus::Transport::onAuthComplete()
{
    DBus::Log::write(DBus::Log::INFO, "DBus :: Transport :: Authorisation has completed.\n");
    boost::recursive_mutex::scoped_lock guard(m_SendMutex);
    for (auto msg : m_BufferedMessages) {
        sendStringDirect(msg);
        ++m_Stats.count_messagespumped;
    }
    m_BufferedMessages.clear();
    m_ReadyToSend = true;
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

    boost::recursive_mutex::scoped_lock guard(m_SendMutex);
    if (m_ReadyToSend) {
        sendStringDirect(data);
    } else {
        addToMessageQueue(data);
    }
}

void DBus::Transport::sendOctetDirect(uint8_t data)
{
    boost::recursive_mutex::scoped_lock guard(m_SendMutex);
    std::shared_ptr<std::string> buf(new std::string());
    buf.get()->push_back(static_cast<char>(data));
    boost::asio::async_write(m_socket, boost::asio::buffer(*buf.get()), boost::bind(&Transport::handle_write_output, this, buf, _1, _2));
}

void DBus::Transport::sendStringDirect(const std::string& data)
{
    DBus::Log::write(DBus::Log::TRACE, "DBus :: SENDDIRECT: %s\n", data.c_str());
    DBus::Log::writeHex(DBus::Log::TRACE, "DBus :: SENDDIRECT: \n", data);
    boost::recursive_mutex::scoped_lock guard(m_SendMutex);

    // We don't know when the write will complete, so we copy the buffer
    std::shared_ptr<std::string> buf(new std::string(data));
    boost::asio::async_write(m_socket, boost::asio::buffer(*buf.get()),
        boost::bind(&Transport::handle_write_output, this, buf, _1, _2));
    ++m_Stats.count_messagessent;
    m_Stats.bytes_sent += data.length();
}

void DBus::Transport::handle_write_output(std::shared_ptr<std::string> buf_written, const boost::system::error_code& error, std::size_t bytes_transferred)
{
    buf_written.reset();
    if (error) {
        DBus::Log::write(DBus::Log::ERROR, "DBus :: ERROR in async_write : %s\n", error.message().c_str());
    }
}

void DBus::Transport::ThreadFunction()
{

    Log::write(Log::INFO, "DBus :: Transport thread starting up\n");

    // We use a second thread for the io context until further notice
    std::unique_ptr<boost::asio::io_service::work> work(new boost::asio::io_service::work(m_io_service));
    std::thread io_service_thread(boost::bind(&boost::asio::io_service::run, &m_io_service));

    m_socket.connect(m_Busname.c_str());

    // The "special credentials passing NUL byte" is required, even for protocols that
    // can send credentials without needing one. Otherwise, the server may disconnect us.
    sendOctetDirect('\0');

    {
        std::unique_lock<std::mutex> lk(m_StartUpMutex);
        m_ReadyToReceive = true;
        m_StartUpCondition.notify_one();
    }

    while (!m_QuitThread) {
        try {
            boost::asio::read(m_socket, boost::asio::buffer(m_DataBuffer, 1));
            ++m_Stats.bytes_read;
            {
                boost::recursive_mutex::scoped_lock guard(m_CallbackMutex);
                m_ReceiveOctetCallback(m_DataBuffer[0]);
            }
        } catch (const boost::system::system_error& e) {
            // If we have an unexpected close, it might be because we sent something
            // the other end did not like.  If we've sent a shutdown, this is unremarkable.
            if (e.code() == boost::asio::error::eof) {
                if (!m_QuitThread) {
                    Log::write(Log::ERROR, "DBus :: Connection closed\n");
                }
            } else {
                Log::write(Log::ERROR, "DBus :: Transport error. %s (%d)\n", e.what());
            }
            break;
        } catch (const std::exception& e) {
            Log::write(Log::ERROR, "DBus :: Transport error. %s\n", e.what());
            break;
        }
    }

    Log::write(Log::INFO, "DBus :: Transport thread closing down\n");
    work.reset();
    io_service_thread.join();

    boost::system::error_code ec;
    m_socket.close(ec);
    if (ec) {
        DBus::Log::write(DBus::Log::ERROR, "DBus :: Transport :: Socket close failed: (%d) \"%s\"\n", ec.value(), ec.message());
    }
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
