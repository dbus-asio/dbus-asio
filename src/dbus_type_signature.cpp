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
#include "dbus_type_signature.h"
#include "dbus_type_struct.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"

const std::string DBus::Type::Signature::s_StaticTypeCode("g");

DBus::Type::Signature::Signature() { setSignature(s_StaticTypeCode); }

DBus::Type::Signature::Signature(const std::string& v)
    : m_Value(v)
{
    setSignature(s_StaticTypeCode);
}

const std::string& DBus::Type::Signature::getValue() const { return m_Value; }

void DBus::Type::Signature::marshall(MessageOStream& stream) const
{
    // NOTE: We marshall out the 'g' to indicate a signature, but don't parse it on unmarshall
    stream.writeByte(1);
    stream.writeByte(s_StaticTypeCode[0]);
    stream.writeByte(0);

    // The same as STRING except the length is a single byte (thus signatures
    // have a maximum length of 255) and the content must be a valid signature.
    stream.writeSignature(m_Value);
}

bool DBus::Type::Signature::unmarshall(const UnmarshallingData& data)
{

    if (m_Unmarshalling.count == 0) {
        m_Unmarshalling.signature_size = data.c;

    } else if (m_Unmarshalling.count - 1 < m_Unmarshalling.signature_size) { // octets for the signature
        m_Value += data.c;

    } else if (m_Unmarshalling.count - 1 == m_Unmarshalling.signature_size) { // the signature NUL terminator
        return true;
    }

    ++m_Unmarshalling.count;

    return false;
}

std::string DBus::Type::Signature::toString(const std::string& prefix) const
{
    std::stringstream ss;

    ss << prefix << "Signature (" << m_Value << ")\n";

    return ss.str();
}

std::string DBus::Type::Signature::asString() const { return m_Value; }
