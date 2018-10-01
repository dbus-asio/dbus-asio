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

#ifndef DBUS_AUTH_H
#define DBUS_AUTH_H

#include <boost/thread/recursive_mutex.hpp>

namespace DBus {
class Transport;

// See Authentication state diagrams
class AuthenticationProtocol {
public:
    typedef enum AuthRequired {
        AUTH_BASIC,
        AUTH_NEGOTIATE_UNIX_FD,
    } AuthRequired;

    AuthenticationProtocol(std::shared_ptr<Transport>& transport);

    void reset();
    void sendAuthListMethods();
    void sendAuth(AuthenticationProtocol::AuthRequired type = AUTH_BASIC);
    bool onReceiveOctet(uint8_t c);

protected:
    bool onOK(const std::string& guid /* unused in this case */);
    bool onError(const std::string& error_message);
    bool onRejected(const std::string& error_message);
    bool onAgreeUnixFD();

    bool onAuth(const std::string&);
    bool onData(const std::string&);
    bool onCancel();
    bool onNegotiateUnixFD(const std::string&);

    void sendNegotiateUnixFD();
    void sendBegin();
    void sendData(std::string& data);
    void sendWire(const std::string& data);
    // Return true once we have completed the auth state
    bool onCommand(const std::string& command);

private:
    std::shared_ptr<Transport> m_Transport;
    uint8_t m_LastOctetSeen;
    std::string m_CurrentCommand;
    AuthenticationProtocol::AuthRequired m_AuthType;
    mutable boost::recursive_mutex m_AuthTypeMutex;
};
}

#endif // DBUS_AUTH_H
