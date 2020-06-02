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
#include "dbus_type_objectpath.h"
#include "dbus_type_signature.h"
#include "dbus_type_string.h"
#include "dbus_type_struct.h"
#include "dbus_type_variant.h"

#include "dbus_message.h"

DBus::Message::MethodCall::MethodCall(const DBus::Type::Struct& header, const std::string& body)
    : Message::Base(header, body)
    , m_Name(m_Header.path, m_Header.interface, m_Header.member)
{
    m_Header.flags = 0;
    m_Header.type = Message::Header::TYPE_METHOD_CALL;
}

DBus::Message::MethodCall::MethodCall(const Message::MethodCallIdentifier& name, const MethodCallParametersIn& params, uint32_t flags)
    : Message::Base()
    , m_Name(name)
{

    if (flags & Message::Header::FLAGS_ALLOW_INTERACTIVE_AUTHORIZATION) {
        DBus::Log::write(Log::ERROR, "DBus :: ALLOW_INTERACTIVE_AUTHORIZATION is not yet supported.");
        flags &= ~Message::Header::FLAGS_ALLOW_INTERACTIVE_AUTHORIZATION;
    }
    // Ignore any extraneous flags, also.
    flags &= Message::Header::FLAGS_MASK;

    m_Header.flags = flags;
    m_Header.type = Message::Header::TYPE_METHOD_CALL;
    m_Parameters = params;
}

std::string DBus::Message::MethodCall::marshall(const std::string& destination) const
{

    // ARRAY of STRUCT of (BYTE,VARIANT)
    // An array of zero or more header fields where the byte is the field code, and the variant is the
    // field m_Value. The message type determines which fields are required.
    DBus::Type::Array array;
    DBus::Type::Struct sPath;
    sPath.add(DBus::Type::Byte(DBus::Message::Header::HEADER_PATH));
    sPath.add(DBus::Type::Variant(DBus::Type::ObjectPath(m_Name.m_Object)));
    array.add(sPath);

    // This is necessary, to prevent errors from the daemon,although it is marked as optional in the spec
    DBus::Type::Struct sDestination;
    sDestination.add(DBus::Type::Byte(DBus::Message::Header::HEADER_DESTINATION));
    sDestination.add(DBus::Type::Variant(DBus::Type::String(destination)));
    array.add(sDestination);

    DBus::Type::Struct sInterface;
    sInterface.add(DBus::Type::Byte(DBus::Message::Header::HEADER_INTERFACE));
    sInterface.add(DBus::Type::Variant(DBus::Type::String(m_Name.m_Interface)));
    array.add(sInterface);

    DBus::Type::Struct sMember;
    sMember.add(DBus::Type::Byte(DBus::Message::Header::HEADER_MEMBER));
    sMember.add(DBus::Type::Variant(DBus::Type::String(m_Name.m_Method)));
    array.add(sMember);

    if (m_Parameters.getParameterCount()) {
        DBus::Type::Struct sSignature;
        sSignature.add(DBus::Type::Byte(DBus::Message::Header::HEADER_SIGNATURE));
        sSignature.add(DBus::Type::Signature(m_Parameters.getMarshallingSignature()));
        array.add(sSignature);
    }

    return marshallMessage(array);
}

std::string DBus::Message::MethodCall::getFullName() const { return getInterface() + "." + getMethod(); }

std::string DBus::Message::MethodCall::getObject() const { return m_Name.m_Object; }

std::string DBus::Message::MethodCall::getInterface() const { return m_Name.m_Interface; }

std::string DBus::Message::MethodCall::getMethod() const { return m_Name.m_Method; }
