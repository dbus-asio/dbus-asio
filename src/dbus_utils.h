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

#ifndef DBUS_UTILS_H
#define DBUS_UTILS_H

#include <string>

namespace DBus {
namespace Utils {
    void ConvertHexStringToBinary(std::string& result, const std::string& input);
    void ConvertBinaryToHexString(std::string& result, const std::string& input);
    bool isAlignedTo(size_t pad, size_t size);
    bool isAlignedTo8(size_t offset);
    size_t getPadding(size_t pad, size_t size);
} // namespace Utils
} // namespace DBus

#endif // DBUS_UTILS_H
