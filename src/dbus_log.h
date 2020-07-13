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

    static bool isActive(size_t type)
    {
        if (type < m_Level) {
            return false;
        }
        return true;
    }

    static void write(size_t type, const char* msg, ...)
    {
        if (!isActive(type)) {
            return;
        }

        va_list ap;
        va_start(ap, msg);
        vfprintf(stderr, msg, ap);
        flush();

        va_end(ap);
    }

    static void writeHex(size_t type, const std::string& prefix,
        const std::string& hex)
    {
        if (!isActive(type)) {
            return;
        }

        write(type, prefix.c_str());

        size_t column = 0;
        for (auto it : hex) {
            write(type, "%.2x ", (uint8_t)it);
            if (++column == 32) {
                write(type, "\n");
                column = 0;
                if (hex.size() % 32) { // pad the next line if there's likely to be one.
                    // i.e. not 32, 64, 96 length etc
                    write(type, std::string(prefix.length(), ' ').c_str()); // pad 2nd line to match prefix
                }
            }
        }
        // Tidy up the last line
        if (column) {
            write(type, "\n");
        }
    }

    static void flush() { fflush(stderr); }

    static void setLevel(size_t lowest_visible_level)
    {
        m_Level = lowest_visible_level;
    }

    static size_t m_Level;
};
} // namespace DBus

#endif // DBUS_LOG_H
