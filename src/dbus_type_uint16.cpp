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
#include "dbus_type_uint16.h"
#include "dbus_messageostream.h"
#include "dbus_messageistream.h"

const std::string DBus::Type::Uint16::s_StaticTypeCode("q");

DBus::Type::Uint16::Uint16()
    : m_Value(0)
{
    setSignature(s_StaticTypeCode);
}

DBus::Type::Uint16::Uint16(uint16_t v)
    : m_Value(v)
{
    setSignature(s_StaticTypeCode);
}

void DBus::Type::Uint16::marshall(MessageOStream& stream) const { stream.writeUint16(m_Value); }

void DBus::Type::Uint16::unmarshall(MessageIStream& stream)
{
    stream.read<uint16_t>(&m_Value);
}

std::string DBus::Type::Uint16::toString(const std::string& prefix) const
{
    std::stringstream ss;

    ss << prefix << "Uint16 ";
    ss << m_Value << " (0x" << std::hex << std::setfill('0') << std::setw(2) << m_Value << ")\n";

    return ss.str();
}

std::string DBus::Type::Uint16::asString() const
{
    std::stringstream ss;
    ss << m_Value;
    return ss.str();
}
