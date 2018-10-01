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

#include <boost/thread/recursive_mutex.hpp>

#include "dbus_log.h"

#include "dbus_type.h"
#include "dbus_type_base.h"
#include "dbus_type_boolean.h"
#include "dbus_type_byte.h"
#include "dbus_type_double.h"
#include "dbus_type_helpers.h"
#include "dbus_type_int16.h"
#include "dbus_type_int32.h"
#include "dbus_type_int64.h"
#include "dbus_type_objectpath.h"
#include "dbus_type_signature.h"
#include "dbus_type_string.h"
#include "dbus_type_struct.h"
#include "dbus_type_uint16.h"
#include "dbus_type_uint32.h"
#include "dbus_type_uint64.h"
#include "dbus_type_variant.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"

/*
Structs and dict entries are marshalled in the same way as their contents, but their
alignment is always to an 8-byte boundary, even if their contents would normally be
less strictly aligned.
*/

std::string DBus::Type::Struct::s_StaticTypeCode("(");

DBus::Type::Struct::Struct()
{
    //	setSignature(s_StaticTypeCode);
}

void DBus::Type::Struct::add(const DBus::Type::Byte& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::Boolean& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::ObjectPath& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::Int16& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::Uint16& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::Int32& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::Uint32& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::Int64& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::Uint64& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::Double& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::String& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::Variant& v) { m_Value.push_back(v); }

void DBus::Type::Struct::add(const DBus::Type::Signature& v) { m_Value.push_back(v); }

// BUGWARN: This should work in all cases, but I'm thinking of moving it into
// a utility method (like all unmarshall).
void DBus::Type::Struct::clear()
{
    m_Value.clear();
    m_Unmarshalling.reset();
}

void DBus::Type::Struct::marshall(MessageStream& stream) const
{

    // A struct must start on an 8-byte boundary regardless of the type of the struct fields.
    stream.pad8();

    for (auto it : m_Value) {
        DBus::Type::marshallData(it, stream);
    }
}

bool DBus::Type::Struct::unmarshall(const UnmarshallingData& data)
{

    if (m_Unmarshalling.areWeSkippingPadding && !Utils::isAlignedTo(getAlignment(), data.offset)) {
        return false;
    }
    m_Unmarshalling.areWeSkippingPadding = false;

    if (m_Unmarshalling.createNewType) {
        m_Unmarshalling.createNewType = false;

        m_Unmarshalling.signature = Type::extractSignature(m_Signature, m_Unmarshalling.signatureIndex);
        m_Value.push_back(DBus::Type::create(m_Unmarshalling.signature));
    }

    // Grab the data element in the process of being unmarshalled
    DBus::Type::Generic& current_type = m_Value.back();
    m_Unmarshalling.createNewType = DBus::Type::unmarshallData(current_type, data);

    // If the previous type has been fully unmarshalled, we must now skip to the next
    // type in the struct.
    if (m_Unmarshalling.createNewType) {
        m_Unmarshalling.signatureIndex += m_Unmarshalling.signature.length();

        if (m_Unmarshalling.signatureIndex >= m_Signature.length() - 1) {
            return true;
        }
    }

    return false;
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
