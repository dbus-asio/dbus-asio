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

#ifndef DBUS_MESSAGEISTREAM_H
#define DBUS_MESSAGEISTREAM_H

#include "dbus_octetbuffer.h"
#include <byteswap.h>
#include <string>

namespace DBus {

class MessageIStream {
    OctetBuffer m_data;
    size_t m_offset;
    bool m_swapByteOrder;

    template <class T>
    T swap(T value) const
    {
        switch (sizeof(T)) {
        case 2:
            return bswap_16(value);
            break;
        case 4:
            return bswap_32(value);
            break;
        case 8:
            return bswap_64(value);
            break;
        default:
            break;
        }
        return value;
    }

public:
    MessageIStream(const uint8_t* data, size_t size, bool swapByteOrder);
    MessageIStream(MessageIStream& stream, size_t size);
    bool empty();

    void align(size_t alignment);
    uint8_t read();
    void read(uint8_t* value, size_t size);
    void read(double* value);
    void read(std::string& string, size_t size);

    template <class T>
    void read(T* value)
    {
        align(sizeof(T));
        read((uint8_t*)value, sizeof(T));
        if (m_swapByteOrder) {
            *value = swap(*value);
        }
    }
};

} // namespace DBus

#endif
