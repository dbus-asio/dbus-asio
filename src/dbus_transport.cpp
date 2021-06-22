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

#include "dbus_transport.h"
#include "dbus_log.h"
#include <boost/bind/bind.hpp>
#include <chrono>

using namespace std::chrono_literals;

DBus::Transport::Transport(const std::string& path)
    : m_Busname(path)
    , m_ReadyToSend(false)
    , m_socket(m_io_context)
    , m_ShuttingDown(false)
{
    setDataHandler(
        std::bind(&Transport::onReceiveData, this, std::placeholders::_1));

    m_socket.connect(m_Busname);

    // The "special credentials passing NUL byte" is required, even for protocols
    // that can send credentials without needing one. Otherwise, the server may
    // disconnect us.
    sendOctetDirect('\0');

    m_socket.async_read_some(
        boost::asio::buffer(m_DataBuffer, BufferSize),
        boost::bind(&Transport::handle_read_data, this,
                    boost::placeholders::_1, boost::placeholders::_2));

    // We use a second thread for the io context until further notice
    m_io_context_thread = boost::thread(boost::bind(&boost::asio::io_context::run, &m_io_context));
}

void DBus::Transport::handle_read_data(const boost::system::error_code& error,
    std::size_t bytes_transferred)
{
    boost::recursive_mutex::scoped_lock guard(m_CallbackMutex);
    OctetBuffer buffer(m_DataBuffer, bytes_transferred);
    m_ReceiveDataCallback(buffer);

    if (error) {
        if (error.category() == boost::asio::error::misc_category && error.value() == boost::asio::error::misc_errors::eof) {
            if (!m_ShuttingDown)
                Log::write(
                    Log::ERROR,
                    "DBus :: Transport received slightly unexpected end of stream\n");
        } else
            Log::write(Log::ERROR, "DBus :: Transport error. %s (%d)\n",
                error.message().c_str(), error.value());
    } else
        m_socket.async_read_some(
            boost::asio::buffer(m_DataBuffer, BufferSize),
            boost::bind(&Transport::handle_read_data, this, boost::placeholders::_1, boost::placeholders::_2));
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
        // Take the callback mutex so that we don't call m_socket.shutdown() at the
        // same time as handle_read_data() is calling async_read_some() on the same
        // object
        boost::recursive_mutex::scoped_lock guard(m_CallbackMutex);
        boost::system::error_code ec;
        m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        if (ec) {
            DBus::Log::write(
                DBus::Log::ERROR,
                "DBus :: Transport :: Socket shutdown failed: (%d) \"%s\"\n",
                ec.value(), ec.message().c_str());
        }
    }

    // Wait for pending async_writes to complete
    if (!m_io_context_thread.try_join_for(boost::chrono::seconds(30))) {
        DBus::Log::write(DBus::Log::ERROR,
            "DBus :: Transport :: IO service thread cannot join\n");
        abort();
    }
}

void DBus::Transport::onAuthComplete()
{
    DBus::Log::write(DBus::Log::INFO,
        "DBus :: Transport :: Authorisation has completed.\n");
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
    DBus::Log::write(DBus::Log::INFO, "DBus :: Transport :: No BEGIN has been "
                                      "received, so message is queued.\n");
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
    boost::asio::async_write(
        m_socket, boost::asio::buffer(*buf.get()),
        boost::bind(&Transport::handle_write_output, this, buf, boost::placeholders::_1, boost::placeholders::_2));
}

void DBus::Transport::sendStringDirect(const std::string& data)
{
    DBus::Log::write(DBus::Log::TRACE, "DBus :: SENDDIRECT: %s\n", data.c_str());
    DBus::Log::writeHex(DBus::Log::TRACE, "DBus :: SENDDIRECT: \n", data);
    boost::recursive_mutex::scoped_lock guard(m_SendMutex);

    // We don't know when the write will complete, so we copy the buffer
    std::shared_ptr<std::string> buf(new std::string(data));
    boost::asio::async_write(
        m_socket, boost::asio::buffer(*buf.get()),
        boost::bind(&Transport::handle_write_output, this, buf, boost::placeholders::_1, boost::placeholders::_2));
    ++m_Stats.count_messagessent;
    m_Stats.bytes_sent += data.length();
}

void DBus::Transport::handle_write_output(
    std::shared_ptr<std::string> buf_written,
    const boost::system::error_code& error, std::size_t bytes_transferred)
{
    buf_written.reset();
    if (error) {
        DBus::Log::write(DBus::Log::ERROR, "DBus :: ERROR in async_write : %s\n",
            error.message().c_str());
    }
}

void DBus::Transport::setDataHandler(
    const ReceiveDataCallbackFunction& callback)
{
    boost::recursive_mutex::scoped_lock guard(m_CallbackMutex);
    m_ReceiveDataCallback = callback;
}

void DBus::Transport::onReceiveData(OctetBuffer&)
{
    // NOP - This stub gives our initial callback somewhere to go

    // This could happen if the thread reads data before native.cpp updates the
    // callback.
    DBus::Log::write(DBus::Log::WARNING,
        "DBus :: Transport : onReceiveData is processing data, "
        "whereas it should really have been directed elsewhere via "
        "setDataHandler\n");
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
