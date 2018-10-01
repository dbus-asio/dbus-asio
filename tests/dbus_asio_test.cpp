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

#include <cstdio>
#include "dbus.h"

int main()
{
    DBus::Log::setLevel(DBus::Log::WARNING);

    DBus::Log::write(DBus::Log::INFO, "System bus: %s\n", DBus::Platform::getSystemBus().c_str());
    DBus::Log::write(DBus::Log::INFO, "Session bus: %s\n", DBus::Platform::getSessionBus().c_str());
    DBus::Native native(DBus::Platform::getSessionBus());
    sleep(1);

    native.BeginAuth(DBus::AuthenticationProtocol::AUTH_BASIC); // AUTH_BASIC or AUTH_NEGOTIATE_UNIX_FD
    return 0;
}
