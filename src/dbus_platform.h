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

#ifndef DBUS_PLATFORM_H
#define DBUS_PLATFORM_H

#include <cstdint>
#include <string>

namespace DBus {
namespace Platform {
    // TODO: Is this abstraction truly needed?
    // typedef _pid_t _pid_t;
    // typedef dbus_uid_t _pid_t;

    uint32_t getPID();
    uint32_t getUID();

    std::string getSystemBus(const char* dbus_system_bus_address);
    std::string getSessionBus(const char* dbus_session_bus_address);

    std::string getSystemBus();
    std::string getSessionBus();
} // namespace Platform
} // namespace DBus

#endif
