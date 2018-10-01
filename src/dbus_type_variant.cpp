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
#include "dbus_utils.h"

#include "dbus_type.h"
#include "dbus_type_base.h"
#include "dbus_type_helpers.h"
#include "dbus_type_objectpath.h"
#include "dbus_type_signature.h"
#include "dbus_type_string.h"
#include "dbus_type_struct.h"
#include "dbus_type_uint32.h"
#include "dbus_type_variant.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"

const std::string DBus::Type::Variant::s_StaticTypeCode("v");

/*
Variants are marshalled as the SIGNATURE of the contents (which must be a single complete type),
followed by a marshalled m_Value with the type given by that signature.

The variant has the same 1-byte alignment as the signature, which means that alignment padding
before a variant is never needed. Use of variants must not cause a total message depth to be
larger than 64, including other container types such as structures. (See Valid Signatures.)
*/

DBus::Type::Variant::Variant() { setSignature(s_StaticTypeCode); }

DBus::Type::Variant::Variant(const DBus::Type::ObjectPath& v)
    : m_Value(v)
{
    // Technically speaking, the signature is part of the data field
    // (We're reusing this method for convenience while we have the type)
    setSignature(v.getSignature());
}

DBus::Type::Variant::Variant(const DBus::Type::String& v)
    : m_Value(v)
{
    setSignature(v.getSignature());
}

DBus::Type::Variant::Variant(const DBus::Type::Uint32& v)
    : m_Value(v)
{
    setSignature(v.getSignature());
}

const DBus::Type::Generic& DBus::Type::Variant::getValue() const { return m_Value; }

void DBus::Type::Variant::marshall(MessageStream& stream) const
{

    // The marshalled SIGNATURE of a single complete type...
    stream.writeSignature(getSignature());

    // ...followed by a marshaled m_Value with the type given in the signature.
    DBus::Type::marshallData(m_Value, stream);
}

bool DBus::Type::Variant::unmarshall(const UnmarshallingData& data)
{

    // A variant consists of
    // 1. A signature indicating the type of data type it contains
    // 2. The data type itself

    if (m_Unmarshalling.isReadingSignature) {
        if (m_Unmarshalling.signature.unmarshall(data)) {
            m_Value = DBus::Type::create(m_Unmarshalling.signature.getValue());
            setSignature(m_Unmarshalling.signature.getValue());
            //
            m_Unmarshalling.isReadingSignature = false;
        }
        return false;
    }
    // else we're reading the data

    return DBus::Type::unmarshallData(m_Value, data);
}

std::string DBus::Type::Variant::toString(const std::string& prefix) const
{
    std::stringstream ss;

    ss << prefix << "Variant (" << getSignature() << ")\n";
    std::string contents_prefix(prefix);
    contents_prefix += "   ";
    ss << DBus::Type::toString(m_Value, contents_prefix);

    return ss.str();
}

std::string DBus::Type::Variant::asString() const { return DBus::Type::asString(m_Value); }
