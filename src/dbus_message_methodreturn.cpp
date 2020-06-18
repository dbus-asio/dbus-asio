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

#include "dbus_log.h"
#include "dbus_type_array.h"
#include "dbus_type_byte.h"
#include "dbus_type_signature.h"
#include "dbus_type_string.h"
#include "dbus_type_struct.h"
#include "dbus_type_uint32.h"
#include "dbus_type_variant.h"

#include "dbus_message.h"

DBus::Message::MethodReturn::MethodReturn(uint32_t serial)
    : Message::Base()
    , m_SerialReplyingTo(serial)
{
    m_Header.flags = 0;
    m_Header.type = Message::Header::TYPE_METHOD_RETURN;
}

DBus::Message::MethodReturn::MethodReturn(const DBus::Type::Struct& header, const std::string& body)
    : Message::Base(header, body)
{

    m_Header.flags = 0;
    m_Header.type = Message::Header::TYPE_METHOD_RETURN;
    m_SerialReplyingTo = getReplySerial();

    Log::write(Log::TRACE, "DBus :: Received a method return with serial #%d\n", m_SerialReplyingTo);
}

void DBus::Message::MethodReturn::addParameter(const Type::Generic& value) { m_Parameters.add(value); }

std::string DBus::Message::MethodReturn::marshall(const std::string& destination) const
{
    DBus::Type::Array array;
    DBus::Type::Struct sDestination;
    sDestination.add(DBus::Type::Byte(DBus::Message::Header::HEADER_DESTINATION));
    sDestination.add(DBus::Type::Variant(DBus::Type::String(destination)));
    array.add(sDestination);

    DBus::Type::Struct sReplySerial;
    sReplySerial.add(DBus::Type::Byte(DBus::Message::Header::HEADER_REPLY_SERIAL));
    sReplySerial.add(DBus::Type::Variant(DBus::Type::Uint32(m_SerialReplyingTo)));
    array.add(sReplySerial);

    if (m_Parameters.getParameterCount()) {
        DBus::Type::Struct sSignature;
        sSignature.add(DBus::Type::Byte(DBus::Message::Header::HEADER_SIGNATURE));
        sSignature.add(DBus::Type::Variant(DBus::Type::Signature(m_Parameters.getMarshallingSignature())));
        array.add(sSignature);
    }

    return marshallMessage(array);
}
