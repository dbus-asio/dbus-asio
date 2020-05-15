// This file is part of dbus-asio
// Copyright 2018 Brightsign LLC
//
// This library is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, version 3, or at your
// option any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// The GNU Lesser General Public License version 3 is included in the
// file named COPYING. If you do not have this file see
// <http://www.gnu.org/licenses/>.

#include <boost/thread/recursive_mutex.hpp>
#include <sys/prctl.h>

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
    , m_socket(m_io_service)
    , m_ShuttingDown(false)
{
    setOctetHandler(std::bind(&Transport::onReceiveOctet, this, std::placeholders::_1));

    m_socket.connect(m_Busname.c_str());

    // The "special credentials passing NUL byte" is required, even for protocols that
    // can send credentials without needing one. Otherwise, the server may disconnect us.
    sendOctetDirect('\0');

    m_socket.async_read_some(boost::asio::buffer(m_DataBuffer, BufferSize),
                             boost::bind(&Transport::handle_read_data, this, _1, _2));

    // We use a second thread for the io context until further notice
    m_io_service_thread = boost::thread (boost::bind (&boost::asio::io_service::run, &m_io_service));
}

void DBus::Transport::handle_read_data(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    boost::recursive_mutex::scoped_lock guard(m_CallbackMutex);
    for (size_t i = 0; i < bytes_transferred; i++)
    {
        ++m_Stats.bytes_read;
        m_ReceiveOctetCallback(m_DataBuffer[i]);
    }

    if (error)
    {
        if (error.category() == boost::asio::error::misc_category
            && error.value() == boost::asio::error::misc_errors::eof)
        {
            if (!m_ShuttingDown)
                Log::write(Log::ERROR, "DBus :: Transport received slightly unexpected end of stream\n");
        }
        else
            Log::write(Log::ERROR, "DBus :: Transport error. %s (%d)\n", error.message().c_str(), error.value());
    }
    else
        m_socket.async_read_some(boost::asio::buffer(m_DataBuffer, BufferSize),
                                 boost::bind(&Transport::handle_read_data, this, _1, _2));
}

DBus::Transport::~Transport()
{
    // EOF on read is now expected
    m_ShuttingDown = true;

    {
        boost::recursive_mutex::scoped_lock guard(m_SendMutex);
        m_ReadyToSend = false;
    }

    {
        // Take the callback mutex so that we don't call m_socket.shutdown() at the same time
        // as handle_read_data() is calling async_read_some() on the same object
        boost::recursive_mutex::scoped_lock guard(m_CallbackMutex);
        boost::system::error_code ec;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec) {
            DBus::Log::write(DBus::Log::ERROR, "DBus :: Transport :: Socket shutdown failed: (%d) \"%s\"\n", ec.value(), ec.message());
        }
    }

    // Wait for pending async_writes to complete
    if (!m_io_service_thread.try_join_for(boost::chrono::seconds(30))) {
        DBus::Log::write(DBus::Log::ERROR, "DBus :: Transport :: IO service thread cannot join\n");
        abort();
    }
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
