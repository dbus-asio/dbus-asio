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

#ifndef DBUS_OCTECTSTREAM_H
#define DBUS_OCTECTSTREAM_H

#include <cstdint>
#include <cstddef>

namespace DBus {

class OctetBuffer
{
    const uint8_t * m_data;
    size_t m_size;

public:
    OctetBuffer(const uint8_t* data, size_t size);

    size_t size() const;
    const uint8_t* data() const;
    void remove_prefix(size_t count);
    bool empty() const;
    uint8_t operator[](unsigned long index) const;
    void copy(uint8_t* data, size_t size) const;
    size_t find(uint8_t byte) const;
};

} // namespace DBus

#endif
