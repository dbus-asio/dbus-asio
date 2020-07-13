// This file is part of dbus-asio
// Copyright 2018-2020 Brightsign LLC
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

#ifndef DBUS_VALIDATION_H
#define DBUS_VALIDATION_H

#include <string>
#include <stdexcept>

namespace DBus {
    namespace Validation {
        bool isValidBasicType(char type);
        void throwOnInvalidBasicType(char type);
        void throwOnInvalidBasicType(const std::string &type);
    }
}

#endif // DBUS_VALIDATION_H
