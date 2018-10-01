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

#include "dbus_platform.h"
#include "dbus_type.h"

uint32_t DBus::Platform::getPID() { return getpid(); }

uint32_t DBus::Platform::getUID() { return getuid(); }

std::string DBus::Platform::getSystemBus()
{
    const char* bus = getenv("DBUS_SYSTEM_BUS_ADDRESS"); // AFAIK, this is not a standardised name

    if (bus) {
        std::string result(bus);
        if (result.substr(0, 10) == "unix:path=") {
            return result.substr(10);
        }
        return result;
    }

    return "/var/run/dbus/system_bus_socket";
}

std::string DBus::Platform::getSessionBus()
{
    const char* bus = getenv("DBUS_SESSION_BUS_ADDRESS");

    if (bus) {
        std::string result(bus);
        if (result.substr(0, 10) == "unix:path=") {
            return result.substr(10);
        }
        return result;
    }

    return "";
}
