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

#include "dbus_type_boolean.h"
#include "dbus_messageistream.h"
#include "dbus_messageostream.h"
#include <sstream>

const std::string DBus::Type::Boolean::s_StaticTypeCode("b");

DBus::Type::Boolean::Boolean()
    : m_Value(0)
{
    setSignature(s_StaticTypeCode);
}

DBus::Type::Boolean::Boolean(uint32_t v)
    : m_Value(v)
{
    setSignature(s_StaticTypeCode);
}

void DBus::Type::Boolean::marshall(MessageOStream& stream) const
{
    stream.writeBoolean(m_Value ? true : false);
}

void DBus::Type::Boolean::unmarshall(MessageIStream& stream)
{
    stream.read<uint32_t>(&m_Value);
}

std::string DBus::Type::Boolean::toString(const std::string& prefix) const
{
    std::stringstream ss;

    ss << prefix << "Boolean ";
    ss << (m_Value ? "True" : "Talse") << "\n";

    return ss.str();
}

std::string DBus::Type::Boolean::asString() const
{
    std::stringstream ss;
    ss << (size_t)m_Value;
    return ss.str();
}
