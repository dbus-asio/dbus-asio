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

#include "dbus_type.h"
#include "dbus_type_dictentry.h"
#include "dbus_type_string.h"
#include "dbus_type_struct.h"
#include "dbus_type_uint32.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"
#include "dbus_validation.h"

/*
Structs and dict entries are marshalled in the same way as their contents, but their
alignment is always to an 8-byte boundary, even if their contents would normally be
less strictly aligned.

A DICT_ENTRY works exactly like a struct, but rather than parentheses it uses curly
braces, and it has more restrictions. The restrictions are: it occurs only as an array
element type; it has exactly two single complete types inside the curly braces; the
first single complete type (the "key") must be a basic type rather than a container type.
Implementations must not accept dict entries outside of arrays, must not accept dict
entries with zero, one, or more than two fields, and must not accept dict entries with
non-basic-typed keys. A dict entry is always a key-value pair.

The first field in the DICT_ENTRY is always the key. A message is considered corrupt
if the same key occurs twice in the same array of DICT_ENTRY. However, for performance
reasons implementations are not required to reject dicts with duplicate keys.


*/

const std::string DBus::Type::DictEntry::s_StaticTypeCode("{");

DBus::Type::DictEntry::DictEntry(const DBus::Type::Generic& key, const DBus::Type::Generic& value) { set(key, value); }

DBus::Type::DictEntry::DictEntry(const std::string& key, std::string& value) { set(DBus::Type::String(key), DBus::Type::String(value)); }

DBus::Type::DictEntry::DictEntry(const std::string& key, uint32_t value) { set(DBus::Type::String(key), DBus::Type::Uint32(value)); }

//
void DBus::Type::DictEntry::set(const DBus::Type::Generic& key, const DBus::Type::Generic& value)
{
    m_Value = std::make_pair(key, value);
    m_Signature = "{" + Type::getMarshallingSignature(m_Value.first) + Type::getMarshallingSignature(m_Value.second) + "}";
}

std::string DBus::Type::DictEntry::getSignature() const { return m_Signature; }

void DBus::Type::DictEntry::marshall(MessageOStream& stream) const
{

    stream.pad8();

    DBus::Type::marshallData(m_Value.first, stream);
    DBus::Type::marshallData(m_Value.second, stream);
}

bool DBus::Type::DictEntry::unmarshall(const UnmarshallingData& data)
{

    if (m_Unmarshalling.areWeSkippingPadding && !Utils::isAlignedTo(getAlignment(), data.offset)) {
        return false;
    }

    m_Unmarshalling.areWeSkippingPadding = false;
    if (m_Unmarshalling.count == 0) {
        const char key_type = getSignature()[1];
        DBus::Validation::throwOnInvalidBasicType(key_type);
        m_Value.first = Type::create(std::string(1, key_type), isLittleEndian());
    }

    if (m_Unmarshalling.onKeyType) {
        if (Type::unmarshallData(m_Value.first, data)) {
            m_Value.second = Type::create(std::string(1, getSignature()[2]), isLittleEndian());
            m_Unmarshalling.onKeyType = false;
        }
    } else { // on value type
        if (Type::unmarshallData(m_Value.second, data)) {
            return true;
        }
    }

    ++m_Unmarshalling.count;

    return false;
}

std::string DBus::Type::DictEntry::toString(const std::string& prefix) const
{
    std::stringstream ss;
    std::string contents_prefix(prefix);
    contents_prefix += "   ";

    ss << prefix << "DictEntry (" << getSignature() << ") : {\n";
    ss << prefix << "   key:   " << DBus::Type::toString(m_Value.first, contents_prefix);
    ss << prefix << "   value: " << DBus::Type::toString(m_Value.second, contents_prefix);
    ss << prefix << "}\n";

    return ss.str();
}

std::string DBus::Type::DictEntry::asString() const { return "[DictEntry]"; }
