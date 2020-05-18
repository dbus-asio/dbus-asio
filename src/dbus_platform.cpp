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

namespace {

const static std::string UNIX_PATH_PREFIX("unix:path=");

void getUnixPath(const char* bus, std::string& path)
{
    if (bus) {
        path = bus;
        if (path.find(UNIX_PATH_PREFIX) == 0) {
            path = path.substr(UNIX_PATH_PREFIX.size());
        }
    }
}

} // anonymous namespace

uint32_t DBus::Platform::getPID() { return getpid(); }

uint32_t DBus::Platform::getUID() { return getuid(); }

std::string DBus::Platform::getSystemBus(const char* dbus_system_bus_address)
{
    std::string path("/var/run/dbus/system_bus_socket");
    getUnixPath(dbus_system_bus_address, path);
    return path;
}

std::string DBus::Platform::getSessionBus(const char* dbus_session_bus_address)
{
    std::string path;
    getUnixPath(dbus_session_bus_address, path);
    return path;
}

std::string DBus::Platform::getSystemBus()
{
    return getSystemBus(getenv("DBUS_SYSTEM_BUS_ADDRESS")); // AFAIK, this is not a standardised name
}

std::string DBus::Platform::getSessionBus()
{
    return getSessionBus(getenv("DBUS_SESSION_BUS_ADDRESS"));
}
