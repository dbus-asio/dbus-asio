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
#include "dbus_type_base.h"
#include "dbus_type_string.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"
#include "dbus_type_struct.h"

const std::string DBus::Type::String::s_StaticTypeCode("s");

DBus::Type::String::String() { setSignature(s_StaticTypeCode); }

DBus::Type::String::String(const std::string& v)
    : m_Value(v)
{
    setSignature(s_StaticTypeCode);
}

void DBus::Type::String::marshall(MessageStream& stream) const
{
    // A UINT32 indicating the string's length in bytes excluding its terminating nul,
    // followed by non-nul string data of the given length,
    // followed by a terminating nul byte.
    stream.writeString(m_Value);
}

bool DBus::Type::String::unmarshall(const UnmarshallingData& data)
{

    if (m_Unmarshalling.areWeSkippingPadding && !Utils::isAlignedTo(getAlignment(), data.offset)) {
        return false;
    }
    m_Unmarshalling.areWeSkippingPadding = false;

    // Read in 4 bytes describing the strings length
    if (m_Unmarshalling.count < 4) {
        *((uint8_t*)&m_Unmarshalling.size + m_Unmarshalling.count) = data.c;
        if (++m_Unmarshalling.count == 4) {
            m_Unmarshalling.size = doSwap32(m_Unmarshalling.size);
        }
        // Even if the string is 0 bytes in length, we still need to process the data stream to
        // accept (and throw away) the nul terminator.
        // (This case was unclear to me in the spec.)
        return false;
    }

    // This adds each cahracter to the string, excluding the NUL terminator
    if (data.c) {
        m_Value += data.c;
    }

    return ++m_Unmarshalling.count == m_Unmarshalling.size + 4 + 1;
}

std::string DBus::Type::String::toString(const std::string& prefix) const
{
    std::stringstream ss;

    ss << prefix << "String (" << m_Value.size() << ") \"" << m_Value << "\"\n";

    return ss.str();
}

std::string DBus::Type::String::asString() const { return m_Value; }
