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
#include "dbus_validation.h"
#include <stdexcept>

namespace DBus
{
    namespace Validation
    {
        bool isValidBasicType(char type)
        {
            switch (type)
            {
            case 'y':
            case 'b':
            case 'n':
            case 'q':
            case 'i':
            case 'u':
            case 'x':
            case 't':
            case 'd':
            case 'h':
            case 's':
            case 'o':
            case 'g':
                return true;
            default:
                return false;
            }
        }

        void throwOnInvalidBasicType(char type)
        {
            if (!isValidBasicType(type))
                throw std::runtime_error(std::string("Invalid basic type: ") + type);
        }

        void throwOnInvalidBasicType(const std::string &type)
        {
            if (type.size() != 1 || !isValidBasicType(type[0]))
                throw std::runtime_error(std::string("Invalid basic type: ") + type);
        }
    }
}
