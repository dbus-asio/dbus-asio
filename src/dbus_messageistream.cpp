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

#include "dbus_messageistream.h"
#include <string>
#include <stdexcept>

namespace DBus {

MessageIStream::MessageIStream(const uint8_t* data, size_t size, bool swapByteOrder)
  : m_data(data, size),
    m_offset(0),
    m_swapByteOrder(swapByteOrder)
{

}

MessageIStream::MessageIStream(MessageIStream& stream, size_t size)
  : m_data(stream.m_data.data(), size),
    m_offset(stream.m_offset),
    m_swapByteOrder(stream.m_swapByteOrder)
{
    stream.m_data.remove_prefix(size);
    stream.m_offset += size;
}

void MessageIStream::align(size_t alignment)
{
    if (m_offset % alignment) {
        const size_t skip = alignment - (m_offset % alignment);
        m_data.remove_prefix(skip);
        m_offset += skip;
    }
}

uint8_t MessageIStream::read()
{
    uint8_t byte = m_data[0];
    ++m_offset;
    m_data.remove_prefix(1);
    return byte;
}

void MessageIStream::read(uint8_t* value, size_t size)
{
    m_data.copy((uint8_t*)value, size);
    m_data.remove_prefix(size);
    m_offset+= size;
}

void MessageIStream::read(double* value)
{
    uint64_t u;
    read(&u);
    *value = *reinterpret_cast<double *>(&u);
}

void MessageIStream::read(std::string& string, size_t size)
{
    if (size > m_data.size()) {
        throw std::runtime_error("Read string error: Not enough data in stream");
    }

    string.clear();
    string.append((const char *)m_data.data(), size);
    m_data.remove_prefix(size);
    m_offset+= size;
}

bool MessageIStream::empty()
{
    return m_data.empty();
}

} // namespace DBus
