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

#ifndef DBUS_LOG_H
#define DBUS_LOG_H

#include <cstddef>
#include <stdarg.h>
#include <string>

namespace DBus {

class Log {
public:
    enum {
        TRACE, // Line numbers and methods indicating the program counter
        INFO, // General information about data flow, and major state changes
        WARNING, // Problems in the data into/out of the DBus
        ERROR // Problems in the library
    };

    static bool isActive(size_t type);

    static void write(size_t type, const char* msg, ...);

    static void writeHex(size_t type, const std::string& prefix,
                         const std::string& hex);
    static void flush();

    static void setLevel(size_t lowest_visible_level);
};
} // namespace DBus

#endif // DBUS_LOG_H
