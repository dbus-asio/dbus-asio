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

#ifndef DBUS_TRANSPORT_H
#define DBUS_TRANSPORT_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "dbus_octetbuffer.h"

#define USE_ASIO_NORMAL 1
#define USE_ASIO_ACCESSOR 0
#define USE_NATIVE_SOCKETS 0

namespace DBus {

typedef std::function<void(OctetBuffer& buffer)> ReceiveDataCallbackFunction;

class Transport {
public:
    Transport(const std::string& path);
    ~Transport();

    void sendString(const std::string& data);
    void handle_write_output(std::shared_ptr<std::string> buf_written, const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_read_data(const boost::system::error_code& error, std::size_t bytes_transferred);

    void ThreadFunction();

    void setDataHandler(const ReceiveDataCallbackFunction& callback);
    void onReceiveData(OctetBuffer& buffer);
    void onAuthComplete();
    void addToMessageQueue(const std::string& data);

    std::string getStats() const;

protected:
    friend class AuthenticationProtocol; // this is to give access to sendStringDirect, to bypass the buffering
    void sendOctetDirect(uint8_t data);
    void sendStringDirect(const std::string& data);

    std::string m_Busname;
    bool m_ReadyToSend;
    boost::asio::io_service m_io_service;
    boost::asio::local::stream_protocol::socket m_socket;

    const static size_t BufferSize = 1024;
    uint8_t m_DataBuffer[BufferSize];
    ReceiveDataCallbackFunction m_ReceiveDataCallback;

    mutable boost::recursive_mutex m_SendMutex;
    mutable boost::recursive_mutex m_CallbackMutex;
    std::vector<std::string> m_BufferedMessages;
    std::atomic<bool> m_ShuttingDown;

    boost::thread m_io_service_thread;

private:
    struct Stats {
        size_t count_messagessent;
        size_t count_messagesqueued;
        size_t count_messagespumped;
        size_t bytes_sent;
        size_t bytes_read;
    } m_Stats;
};
}

#endif // DBUS_TRANSPORT_H
