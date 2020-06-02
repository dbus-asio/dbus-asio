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

#include <sstream>
#include "dbus_type.h"
#include "dbus_type_string.h"
#include "dbus_messageostream.h"
#include "dbus_messageistream.h"

const std::string DBus::Type::String::s_StaticTypeCode("s");

DBus::Type::String::String() { setSignature(s_StaticTypeCode); }

DBus::Type::String::String(const std::string& v)
    : m_Value(v)
{
    setSignature(s_StaticTypeCode);
}

void DBus::Type::String::marshall(MessageOStream& stream) const
{
    // A UINT32 indicating the string's length in bytes excluding its terminating nul,
    // followed by non-nul string data of the given length,
    // followed by a terminating nul byte.
    stream.writeString(m_Value);
}

void DBus::Type::String::unmarshall(MessageIStream& stream)
{
    uint32_t size;
    stream.read<uint32_t>(&size);
    stream.read(m_Value, size);
    stream.read(); // null byte
}

std::string DBus::Type::String::toString(const std::string& prefix) const
{
    std::stringstream ss;

    ss << prefix << "String (" << m_Value.size() << ") \"" << m_Value << "\"\n";

    return ss.str();
}

std::string DBus::Type::String::asString() const { return m_Value; }
