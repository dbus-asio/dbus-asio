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

#ifndef DBUS_MESSAGEOSTREAM_H
#define DBUS_MESSAGEOSTREAM_H

#include "dbus_utils.h"

namespace DBus {

class MessageOStream {
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
        pad2();
        data.append((char*)&value, sizeof(int16_t));
    }

    void writeUint16(uint16_t value)
    {
        pad2();
        data.append((char*)&value, sizeof(uint16_t));
    }

    void writeInt32(int32_t value)
    {
        pad4();
        data.append((char*)&value, sizeof(int32_t));
    }

    void writeUint32(uint32_t value)
    {
        pad4();
        data.append((char*)&value, sizeof(uint32_t));
    }

    void writeInt64(int64_t value)
    {
        pad8();
        data.append((char*)&value, sizeof(int64_t));
    }

    void writeUint64(uint64_t value)
    {
        pad8();
        data.append((char*)&value, sizeof(uint64_t));
    }

    void writeDouble(double value)
    {
        pad8();
        data.append((char*)&value, sizeof(double));
    }

    void write(const std::string& str)
    {
        data.append(str.data(), str.length());
    }

    void write(const MessageOStream& stream)
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
        data.append(DBus::Utils::getPadding(padding, data.length()), 0);
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
