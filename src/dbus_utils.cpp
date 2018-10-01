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

#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <string>

#include "dbus_utils.h"

void DBus::Utils::ConvertHexStringToBinary(std::string& result, const std::string& input)
{
    std::istringstream ss(input);
    std::string word;
    while (ss >> std::setw(2) >> word) {
        std::stringstream converter;
        uint16_t temp;
        converter << std::hex << word;
        converter >> temp;
        result.push_back(temp);
    }
}

void DBus::Utils::ConvertBinaryToHexString(std::string& result, const std::string& input)
{
    std::stringstream ss;

    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < input.length(); ++i) {
        ss << std::setw(2) << int(uint8_t(input[i]));
    }
    result.append(ss.str());
}

bool DBus::Utils::isAlignedTo(size_t pad, size_t size) { return (size % pad) == 0 ? true : false; }

bool DBus::Utils::isAlignedTo8(size_t offset) { return isAlignedTo(8, offset); }

// How many bytes of padding are required so that 'size' is padded to 'pad'
size_t DBus::Utils::getPadding(size_t pad, size_t size)
{
    // The zero case is generally not used but be used as code-as-comment (e.g. pad(0) to indicate none is needed)
    if (pad == 0) {
        return 0;
    }

    size_t required = pad - (size % pad);

    if (required == pad) {
        return 0;
    }
    return required;
}
