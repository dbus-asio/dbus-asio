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

#include "dbus_auth.h"
#include "dbus_log.h"
#include "dbus_platform.h"
#include "dbus_utils.h"

/*
The AUTH handshake is:

Server        Client
        <---  AUTH
OK      ---->
        <---  (1) BEGIN (or)
        <---  (2) NEGOTIATE_UNIX_FD
AGREE   ---->
        <---  BEGIN
*/

DBus::AuthenticationProtocol::AuthenticationProtocol(
    std::shared_ptr<Transport>& transport)
    : m_Transport(transport)
{
}

void DBus::AuthenticationProtocol::reset()
{
    m_data.clear();
    sendAuth(m_AuthType);
}

void DBus::AuthenticationProtocol::sendAuthListMethods()
{
    sendWire(std::string("AUTH\r\n"));
}

// RCVD: REJECTED EXTERNAL DBUS_COOKIE_SHA1 ANONYMOUS
void DBus::AuthenticationProtocol::sendAuth(
    DBus::AuthenticationProtocol::AuthRequired type /* = AUTH_BASIC*/)
{
    {
        boost::recursive_mutex::scoped_lock guard(m_AuthTypeMutex);
        m_AuthType = type;
    }

    std::string auth("AUTH EXTERNAL ");
    DBus::Utils::ConvertBinaryToHexString(auth,
        std::to_string(Platform::getUID()));
    auth += "\r\n";
    sendWire(auth);
}

bool DBus::AuthenticationProtocol::processData()
{
    const size_t pos = m_data.find("\r\n");
    if (pos != std::string::npos) {
        std::string command(m_data.data(), pos + 2);
        return onCommand(command);
        m_data = m_data.substr(pos + 2);
    };

    return false;
}

bool DBus::AuthenticationProtocol::onReceiveData(DBus::OctetBuffer& buffer)
{
    bool authenticated = false;

    while (buffer.size() && !authenticated) {
        size_t pos = buffer.find('\n');
        pos = pos == std::string::npos ? buffer.size() : pos + 1;
        m_data.append((const char*)buffer.data(), pos);
        buffer.remove_prefix(pos);
        authenticated = processData();
    };

    return authenticated;
}

//
// Protected methods
//
bool DBus::AuthenticationProtocol::onOK(
    const std::string& guid /* unused in this case */)
{
    std::string address;
    DBus::Utils::ConvertHexStringToBinary(address, guid);

    bool send_begin = false;

    {
        boost::recursive_mutex::scoped_lock guard(m_AuthTypeMutex);
        if (m_AuthType == AUTH_BASIC) {
            send_begin = true;
        }
    }

    if (send_begin) {
        sendBegin();
        return true;
    }

    sendNegotiateUnixFD();
    return false;
}

bool DBus::AuthenticationProtocol::onError(const std::string& error_message)
{
    DBus::Log::write(DBus::Log::ERROR, "DBus :: onError : %s\n",
        error_message.c_str());
    return false;
}

bool DBus::AuthenticationProtocol::onRejected(
    const std::string& error_message)
{
    // Split by space
    // Store the list of available auth methods
    // Optionally, retry. (determin by cb?)
    DBus::Log::write(DBus::Log::WARNING, "DBus :: Reject : %s\n",
        error_message.c_str());
    return false;
}

bool DBus::AuthenticationProtocol::onAgreeUnixFD()
{
    DBus::Log::write(DBus::Log::INFO, "DBus :: onAgreeUnixFD\n");
    sendBegin();
    return true;
}

bool DBus::AuthenticationProtocol::onAuth(const std::string&) { return false; }

bool DBus::AuthenticationProtocol::onData(const std::string&) { return false; }

bool DBus::AuthenticationProtocol::onCancel() { return false; }

bool DBus::AuthenticationProtocol::onNegotiateUnixFD(const std::string&)
{
    return false;
}

void DBus::AuthenticationProtocol::sendNegotiateUnixFD()
{
    sendWire(std::string("NEGOTIATE_UNIX_FD\r\n"));
}

void DBus::AuthenticationProtocol::sendBegin()
{
    sendWire(std::string("BEGIN\r\n"));
    m_Transport->onAuthComplete();
}

void DBus::AuthenticationProtocol::sendData(std::string& data)
{
    std::string packet("DATA ");
    DBus::Utils::ConvertBinaryToHexString(packet, data);
    packet += "\r\n";

    sendWire(packet);
}

void DBus::AuthenticationProtocol::sendWire(const std::string& data)
{
    m_Transport->sendStringDirect(data);
}

// Return true once we have completed the auth state
bool DBus::AuthenticationProtocol::onCommand(const std::string& command)
{
    DBus::Log::write(DBus::Log::TRACE, "DBus :: CMD: %s\n", command.c_str());
    DBus::Log::writeHex(DBus::Log::TRACE, "DBus :: CMD: ", command);

    std::pair<const char*, std::function<bool(const std::string&)>>
        callback_list[8] = {
            // Client
            std::make_pair("OK", std::bind(&DBus::AuthenticationProtocol::onOK, this, std::placeholders::_1)),
            std::make_pair("ERROR",
                std::bind(&DBus::AuthenticationProtocol::onError, this,
                    std::placeholders::_1)),
            std::make_pair("REJECTED",
                std::bind(&DBus::AuthenticationProtocol::onRejected,
                    this, std::placeholders::_1)),
            std::make_pair(
                "AGREE_UNIX_FD",
                std::bind(&DBus::AuthenticationProtocol::onAgreeUnixFD, this)),
            std::make_pair("DATA",
                std::bind(&DBus::AuthenticationProtocol::onData, this,
                    std::placeholders::_1)),
            // Server (TODO)
            std::make_pair("AUTH",
                std::bind(&DBus::AuthenticationProtocol::onAuth, this,
                    std::placeholders::_1)),
            std::make_pair(
                "NEGOTIATE_UNIX_FD",
                std::bind(&DBus::AuthenticationProtocol::onNegotiateUnixFD, this,
                    std::placeholders::_1)),
            std::make_pair(
                "CANCEL",
                std::bind(&DBus::AuthenticationProtocol::onCancel, this))
        };

    for (size_t i = 0; i < 8; ++i) {
        if (command.substr(0, strlen(callback_list[i].first)) == callback_list[i].first) {
            return callback_list[i].second(
                command.substr(strlen(callback_list[i].first) + 1));
        }
    }

    DBus::Log::write(DBus::Log::WARNING,
        "DBus :: CMD: %s did not execute anything, so please "
        "implement the method.\n",
        command.c_str());

    return false;
}
