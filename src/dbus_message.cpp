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

#include <boost/thread/recursive_mutex.hpp>

#include "dbus_type.h"
#include "dbus_type_array.h"
#include "dbus_type_array.h"
#include "dbus_type_byte.h"
#include "dbus_type_signature.h"
#include "dbus_type_string.h"
#include "dbus_type_struct.h"
#include "dbus_type_uint32.h"
#include "dbus_type_variant.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"
#include "dbus_messageistream.h"

uint32_t DBus::Message::Base::m_SerialCounter = 1;
boost::recursive_mutex DBus::Message::Base::m_SerialCounterMutex;

DBus::Message::Base::Base(uint32_t serial)
{
    boost::recursive_mutex::scoped_lock guard(m_SerialCounterMutex);
    m_Header.serial = serial ? serial : m_SerialCounter++;
}

DBus::Message::Base::Base(const DBus::Type::Struct& header, const std::string& body)
{
    // Capture the basic parameters
    bool isLittleEndian = Type::asByte(header[0]) == 'l' ? true : false;
    m_Header.type = Type::asByte(header[1]);
    m_Header.flags = Type::asByte(header[2]);
    m_Header.serial = Type::asUint32(header[5]);

    // The fields are opaque types, each one (yv)
    for (size_t i = 0; i < Message::Header::HEADER_FIELD_COUNT; ++i) {
        m_Header.field[i] = Message::getHeaderField(header, i);
    }

    // Store a copy of the header fields in type-safe data types
    m_Header.path = DBus::Type::asString(m_Header.field[Message::Header::HEADER_PATH]);
    m_Header.interface = DBus::Type::asString(m_Header.field[Message::Header::HEADER_INTERFACE]);
    m_Header.member = DBus::Type::asString(m_Header.field[Message::Header::HEADER_MEMBER]);
    m_Header.destination = DBus::Type::asString(m_Header.field[Message::Header::HEADER_DESTINATION]);
    m_Header.sender = DBus::Type::asString(m_Header.field[Message::Header::HEADER_SENDER]);

    // Empty bodies have no parameters
    if (body.size()) {
        parseParameters(isLittleEndian, body);
    }
}

void DBus::Message::Base::parseParameters(bool isLittleEndian, const std::string& bodydata)
{
    std::string signature(DBus::Type::asString(m_Header.field[Message::Header::HEADER_SIGNATURE]));

    DBus::Type::Struct parameter_fields;

    parameter_fields.setSignature("(" + signature + ")");

    MessageIStream stream((uint8_t*)bodydata.data(), bodydata.size(),
        isLittleEndian ? __BYTE_ORDER != __LITTLE_ENDIAN :
                         __BYTE_ORDER != __BIG_ENDIAN);
    parameter_fields.unmarshall(stream);

    size_t count = parameter_fields.getEntries();
    for (size_t i = 0; i < count; ++i) {
        m_Parameters.add(parameter_fields[i]);
    }
}

DBus::Type::Generic DBus::Message::getHeaderField(const DBus::Type::Struct& header, size_t type)
{
    // Each field is a struct containing one byte and a variant. (yv)
    const DBus::Type::Array& fields = DBus::Type::refArray(header[6]);
    for (auto it : fields.getContents()) {
        const DBus::Type::Struct& headerField = DBus::Type::refStruct(it);

        if (Type::asByte(headerField[0]) == type) {
            // The variant contains within it a type. This is the one that's of interest.
            return DBus::Type::refVariant(headerField[1]).getValue();
        }
    }

    return DBus::Type::Generic();
}

std::string DBus::Message::Base::marshallMessage(const DBus::Type::Array& array) const
{
    MessageOStream header;
    MessageOStream body;

    /*
            The signature of the header is:

                    "yyyyuua(yv)"

            Written out more readably, this is:

                    BYTE, BYTE, BYTE, BYTE, UINT32, UINT32, ARRAY of STRUCT of (BYTE,VARIANT)
    */

    // 1st BYTE
    // Endianness flag; ASCII 'l' for little-endian or ASCII 'B' for big-endian. Both header and body are in this endianness.
    header.writeByte(__BYTE_ORDER == __LITTLE_ENDIAN ? 'l' : 'B');

    // 2nd BYTE
    // Message type. Unknown types must be ignored. Currently-defined types are described below.
    header.writeByte(m_Header.type);

    // 3rd BYTE
    // Bitwise OR of flags. Unknown flags must be ignored. Currently-defined flags are described below.
    header.writeByte(m_Header.flags);

    // 4th BYTE
    // Major protocol version of the sending application. If the major protocol version of the receiving application
    // does not match, the applications will not be able to communicate and the D-Bus connection must be
    // disconnected. The major protocol version for this version of the specification is 1.
    header.writeByte(1);

    // 1st UINT32
    // Length in bytes of the message body, starting from the end of the header. The header ends after its alignment
    // padding to an 8-boundary.
    m_Parameters.marshallData(body);

    header.writeUint32(body.data.length());

    // 2nd UINT32
    // The serial of this message, used as a cookie by the sender to identify the reply corresponding to this request. This must not be zero.
    header.writeUint32(m_Header.serial);

    // Header fields length
    MessageOStream header_fields;

    // It appears, from studying the packet data, that the header field array is not marshalled with
    // the length of the array data in bytes, as suggested in the spec.
    // Presumably this is because the header fields size can be determined from
    // the packet_size - body_size - standard_header_size
    array.marshallContents(header_fields);
    header.writeUint32((int32_t)header_fields.size());

    // End of header preparation

    // Both header & header_fields constitute the header, which must end of an 8 boundary.
    // (See the phrase: "The header ends after its alignment padding to an 8-boundary.")
    // Therefore we add the padding here.
    MessageOStream packet;
    packet.write(header);
    packet.write(header_fields);

    // The body is required to start on an 8-boundary, but not end on one.
    packet.pad8();
    packet.write(body);

    return packet.data;
}

//
// Parameters
//
std::string DBus::Message::MethodCallParameters::getMarshallingSignature() const
{
    std::string signature;
    for (auto it : m_Contents.m_TypeList) {
        signature += DBus::Type::getMarshallingSignature(it);
    }
    return signature;
}

void DBus::Message::MethodCallParameters::marshallData(MessageOStream& stream) const
{
    for (auto it : m_Contents.m_TypeList) {
        DBus::Type::marshallData(it, stream);
    }
}

size_t DBus::Message::MethodCallParameters::getParameterCount() const { return m_Contents.m_TypeList.size(); }

const DBus::Type::Generic& DBus::Message::MethodCallParameters::getParameter(size_t idx) const { return m_Contents.m_TypeList[idx]; }

DBus::Message::MethodCallParametersIn::MethodCallParametersIn(const Type::Generic& v) { add(v); }

DBus::Message::MethodCallParametersIn::MethodCallParametersIn(const std::string& v) { add(v); }

DBus::Message::MethodCallParametersIn::MethodCallParametersIn(const std::string& v1, uint32_t v2)
{
    add(v1);
    add(v2);
}

void DBus::Message::MethodCallParametersIn::add(const Type::Generic& value) { m_Contents.m_TypeList.push_back(value); }

void DBus::Message::MethodCallParametersIn::add(uint8_t value) { add(DBus::Type::Byte(value)); }

void DBus::Message::MethodCallParametersIn::add(uint32_t value) { add(DBus::Type::Uint32(value)); }

void DBus::Message::MethodCallParametersIn::add(const std::string& value) { add(DBus::Type::String(value)); }
