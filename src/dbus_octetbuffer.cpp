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

#include "dbus_octetbuffer.h"
#include <stdexcept>
#include <string.h>
#include <string>

namespace DBus {

OctetBuffer::OctetBuffer(const uint8_t* data, size_t size)
    : m_data(data),
    m_size(size)
{
}

size_t OctetBuffer::size() const
{
    return m_size;
}

const uint8_t* OctetBuffer::data() const
{
    return m_data;
}

void OctetBuffer::remove_prefix(size_t count)
{
    if (count > m_size) {
        throw std::out_of_range("OctetBuffer::remove_prefix error: Not enough data in buffer");
    }
    m_data+= count;
    m_size-= count;
}

bool OctetBuffer::empty() const
{
    return m_size == 0;
}

uint8_t OctetBuffer::operator[](unsigned long index) const
{
    if (index >= m_size) {
        throw std::out_of_range("OctetBuffer index out of range error");
    }
    return m_data[index];
}

void OctetBuffer::copy(uint8_t* data, size_t size) const
{
    if (size > m_size) {
        throw std::out_of_range("OctetBuffer copy error: Not enough data in buffer");
    }
    memcpy(data, m_data, size);
}

size_t OctetBuffer::find(uint8_t byte) const
{
    for (size_t i = 0; i < m_size; ++i) {
        if (m_data[i] == byte) {
            return i;
        }
    }
    return std::string::npos;
}

} // namespace DBus
