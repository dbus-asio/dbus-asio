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

#ifndef DBUS_MESSAGESTREAM_H
#define DBUS_MESSAGESTREAM_H

#include "dbus_utils.h"
#include <byteswap.h>

namespace DBus {

class MessageStream {
public:
    std::string data;

    size_t size() const
    {
        return data.length();
    }

    void writeByte(char byte)
    {
        data.push_back(byte);
    }

    void writeBoolean(bool value)
    {
        writeUint32(value ? 1 : 0);
    }

    void writeInt16(int16_t value)
    {
        pad4();

        if (__BYTE_ORDER == __LITTLE_ENDIAN) {
            value = __bswap_16(value);
        }

        data.push_back((value & 0xff00) >> 8);
        data.push_back((value & 0x00ff) >> 0);
    }

    void writeUint16(uint16_t value)
    {
        pad2();

        if (__BYTE_ORDER == __LITTLE_ENDIAN) {
            value = __bswap_16(value);
        }

        data.push_back((value & 0xff00) >> 8);
        data.push_back((value & 0x00ff) >> 0);
    }

    void writeInt32(int32_t value)
    {
        pad4();

        if (__BYTE_ORDER == __LITTLE_ENDIAN) {
            value = bswap_32(value);
        }

        data.push_back((uint8_t)((value & 0xff000000) >> 24));
        data.push_back((uint8_t)((value & 0x00ff0000) >> 16));
        data.push_back((uint8_t)((value & 0x0000ff00) >> 8));
        data.push_back((uint8_t)((value & 0x000000ff) >> 0));
    }

    void writeUint32(uint32_t value)
    {
        pad4();

        if (__BYTE_ORDER == __LITTLE_ENDIAN) {
            value = bswap_32(value);
        }

        data.push_back((uint8_t)((value & 0xff000000) >> 24));
        data.push_back((uint8_t)((value & 0x00ff0000) >> 16));
        data.push_back((uint8_t)((value & 0x0000ff00) >> 8));
        data.push_back((uint8_t)((value & 0x000000ff) >> 0));
    }

    void writeInt64(int64_t value)
    {
        pad8();

        if (__BYTE_ORDER == __LITTLE_ENDIAN) {
            value = bswap_64(value);
        }

        data.push_back((uint8_t)((value & 0xff00000000000000) >> 56));
        data.push_back((uint8_t)((value & 0x00ff000000000000) >> 48));
        data.push_back((uint8_t)((value & 0x0000ff0000000000) >> 40));
        data.push_back((uint8_t)((value & 0x000000ff00000000) >> 32));
        data.push_back((uint8_t)((value & 0x00000000ff000000) >> 24));
        data.push_back((uint8_t)((value & 0x0000000000ff0000) >> 16));
        data.push_back((uint8_t)((value & 0x000000000000ff00) >> 8));
        data.push_back((uint8_t)((value & 0x00000000000000ff) >> 0));
    }

    void writeUint64(uint64_t value)
    {
        pad8();

        if (__BYTE_ORDER == __LITTLE_ENDIAN) {
            value = bswap_64(value);
        }

        data.push_back((uint8_t)((value & 0xff00000000000000) >> 56));
        data.push_back((uint8_t)((value & 0x00ff000000000000) >> 48));
        data.push_back((uint8_t)((value & 0x0000ff0000000000) >> 40));
        data.push_back((uint8_t)((value & 0x000000ff00000000) >> 32));
        data.push_back((uint8_t)((value & 0x00000000ff000000) >> 24));
        data.push_back((uint8_t)((value & 0x0000000000ff0000) >> 16));
        data.push_back((uint8_t)((value & 0x000000000000ff00) >> 8));
        data.push_back((uint8_t)((value & 0x00000000000000ff) >> 0));
    }

    void writeDouble(double value)
    {
        uint64_t v = *(int64_t*)&value;
        writeUint64(v);
    }

    void write(const std::string& str)
    {
        data.append(str.data(), str.length());
    }

    void write(const MessageStream& stream)
    {
        data.append(stream.data.data(), stream.data.length());
    }

    void writeString(const std::string& str)
    {
        pad4();

        // A UINT32 indicating the string's length in bytes excluding its terminating nul,
        writeUint32(str.length());

        // followed by non-nul string data of the given length,
        data.append(str);

        // followed by a terminating nul byte.
        writeByte(0);
    }

    void writeSignature(const std::string& str)
    {
        // The same as STRING except the length is a single byte (thus signatures have a maximum length of 255)
        writeByte(str.length());
        // and the content must be a valid signature (see above).
        data.append(str);
        writeByte(0);
    }

    void pad(size_t padding)
    {
        size_t required = DBus::Utils::getPadding(padding, data.length());

        for (size_t i = 0; i < required; ++i) {
            writeByte(0);
        }
    }

    void pad2()
    {
        pad(2);
    }

    void pad4()
    {
        pad(4);
    }

    void pad8()
    {
        pad(8);
    }
};
}

#endif
