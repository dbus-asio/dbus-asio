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

#include "dbus_log.h"

static size_t g_Level = DBus::Log::WARNING;

bool DBus::Log::isActive(size_t type)
{
    if (type < g_Level) {
        return false;
    }
    return true;
}

void DBus::Log::write(size_t type, const char* msg, ...)
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

void DBus::Log::writeHex(size_t type, const std::string& prefix,
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

void DBus::Log::flush() { fflush(stderr); }

void DBus::Log::setLevel(size_t lowest_visible_level)
{
    g_Level = lowest_visible_level;
}
