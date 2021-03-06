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

#include "dbus_type_struct.h"
#include "dbus_messageistream.h"
#include "dbus_messageostream.h"
#include "dbus_type_boolean.h"
#include "dbus_type_byte.h"
#include "dbus_type_dictentry.h"
#include "dbus_type_double.h"
#include "dbus_type_int16.h"
#include "dbus_type_int32.h"
#include "dbus_type_int64.h"
#include "dbus_type_objectpath.h"
#include "dbus_type_signature.h"
#include "dbus_type_string.h"
#include "dbus_type_uint16.h"
#include "dbus_type_uint32.h"
#include "dbus_type_uint64.h"
#include "dbus_type_variant.h"
#include <sstream>

/*
Structs and dict entries are marshalled in the same way as their contents, but
their alignment is always to an 8-byte boundary, even if their contents would
normally be less strictly aligned.
*/

std::string DBus::Type::Struct::s_StaticTypeCode("(");

DBus::Type::Struct::Struct()
{
    //	setSignature(s_StaticTypeCode);
}

void DBus::Type::Struct::add(const DBus::Type::Byte& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::Boolean& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::ObjectPath& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::Int16& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::Uint16& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::Int32& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::Uint32& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::Int64& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::Uint64& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::Double& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::String& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::Variant& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::Signature& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::Struct& v)
{
    m_Value.push_back(v);
}

void DBus::Type::Struct::add(const DBus::Type::DictEntry& v)
{
    m_Value.push_back(v);
}

// BUGWARN: This should work in all cases, but I'm thinking of moving it into
// a utility method (like all unmarshall).
void DBus::Type::Struct::clear() { m_Value.clear(); }

void DBus::Type::Struct::marshall(MessageOStream& stream) const
{

    // A struct must start on an 8-byte boundary regardless of the type of the
    // struct fields.
    stream.pad8();

    for (auto it : m_Value) {
        DBus::Type::marshallData(it, stream);
    }
}

void DBus::Type::Struct::unmarshall(MessageIStream& stream)
{
    // A struct must start on an 8-byte boundary regardless of the type of the
    // struct fields.
    stream.align(8);
    size_t signatureIndex = 1; // skip first '('
    do {
        const std::string signature = Type::extractSignature(m_Signature, signatureIndex);
        m_Value.push_back(DBus::Type::create(signature));
        DBus::Type::unmarshallData(m_Value.back(), stream);
        signatureIndex += signature.size();
    } while (signatureIndex < m_Signature.size() - 1);
}

std::string DBus::Type::Struct::toString(const std::string& prefix) const
{
    std::stringstream ss;

    ss << prefix << "Struct " << getSignature() << " <\n";
    std::string contents_prefix(prefix);
    contents_prefix += "   ";
    for (auto it : m_Value) {
        ss << DBus::Type::toString(it, contents_prefix);
    }
    ss << prefix << ">\n";

    return ss.str();
}

std::string DBus::Type::Struct::asString() const { return "[struct]"; }
