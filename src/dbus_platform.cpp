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
#include <unistd.h>

namespace {

const static std::string UNIX_PATH_PREFIX("unix:path=");
const static std::string UNIX_ABSTRACT_PATH_PREFIX("unix:abstract=");

std::string getAbstractPath(const std::string& path)
{
    if (path.find(UNIX_ABSTRACT_PATH_PREFIX) == 0) {
        std::string abstract_path;
        abstract_path.push_back('\0');
        abstract_path += path.substr(UNIX_ABSTRACT_PATH_PREFIX.size());

        const size_t guid_part_pos(abstract_path.find(",guid="));
        if (guid_part_pos != std::string::npos) {
            abstract_path = abstract_path.substr(0, guid_part_pos);
        }
        return abstract_path;
    }
    return path;
}

std::string getPath(const char* bus)
{
    std::string path(bus);
    if (path.find(UNIX_PATH_PREFIX) == 0) {
        return path.substr(UNIX_PATH_PREFIX.size());
    } else {
        return getAbstractPath(bus);
    }
    return path;
}

} // anonymous namespace

uint32_t DBus::Platform::getPID() { return getpid(); }

uint32_t DBus::Platform::getUID() { return getuid(); }

std::string DBus::Platform::getSystemBus(const char* dbus_system_bus_address)
{
    if (dbus_system_bus_address) {
        return getPath(dbus_system_bus_address);
    }
    return "/var/run/dbus/system_bus_socket";
}

std::string
DBus::Platform::getSessionBus(const char* dbus_session_bus_address)
{
    if (dbus_session_bus_address) {
        return getPath(dbus_session_bus_address);
    }
    return "";
}

std::string DBus::Platform::getSystemBus()
{
    return getSystemBus(getenv(
        "DBUS_SYSTEM_BUS_ADDRESS")); // AFAIK, this is not a standardised name
}

std::string DBus::Platform::getSessionBus()
{
    return getSessionBus(getenv("DBUS_SESSION_BUS_ADDRESS"));
}
