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

#include "dbus_log.h"

#include "dbus_type.h"
#include "dbus_type_array.h"
#include "dbus_type_base.h"
#include "dbus_type_boolean.h"
#include "dbus_type_byte.h"
#include "dbus_type_dictentry.h"
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
#include "dbus_messageostream.h"
#include "dbus_messageistream.h"

//
// Helper methods to extract navtive types from the opaque 'Generic' type
//
uint8_t DBus::Type::asByte(const Generic& v) { return std::stoi(Type::asString(v)); }

uint32_t DBus::Type::asUint32(const Generic& v) { return std::stoi(Type::asString(v)); }

// Generate a compact, machine-friendly, version of the data.
// (compare to toString, which is intended for humans debugging)
std::string DBus::Type::asString(const DBus::Type::Generic& value)
{
    // It is possible to receive an empty value when dealing with void parameter lists, or
    // with uninitialised types from header fields.
    if (value.empty()) {
        return "";
    }
#define TYPE_ASSTRING(typename)                                        \
    {                                                                  \
        static Type::Generic is_type = typename();                     \
        if (ti == is_type.type()) {                                    \
            return boost::any_cast<const typename&>(value).asString(); \
        }                                                              \
    }
    const std::type_info& ti = value.type();

    TYPE_ASSTRING(Type::Byte);
    TYPE_ASSTRING(Type::Boolean);
    TYPE_ASSTRING(Type::Int16);
    TYPE_ASSTRING(Type::Uint16);
    TYPE_ASSTRING(Type::Int32);
    TYPE_ASSTRING(Type::Uint32);
    TYPE_ASSTRING(Type::Int64);
    TYPE_ASSTRING(Type::Uint64);
    TYPE_ASSTRING(Type::Double);
    TYPE_ASSTRING(Type::String);
    TYPE_ASSTRING(Type::Array);
    TYPE_ASSTRING(Type::Variant);
    TYPE_ASSTRING(Type::Struct);
    TYPE_ASSTRING(Type::ObjectPath);
    TYPE_ASSTRING(Type::Signature);
    TYPE_ASSTRING(Type::DictEntry);

#undef TYPE_ASSTRING

    Log::write(DBus::Log::ERROR, "DBus :: asString type not implemented (%s)\n", ti.name());

    return "";
}

const DBus::Type::Array& DBus::Type::refArray(const Generic& value) { return boost::any_cast<const Type::Array&>(value); }

const DBus::Type::Struct& DBus::Type::refStruct(const Generic& value) { return boost::any_cast<const Type::Struct&>(value); }

const DBus::Type::Signature& DBus::Type::refSignature(const Generic& value) { return boost::any_cast<const Type::Signature&>(value); }

const DBus::Type::Variant& DBus::Type::refVariant(const Generic& value) { return boost::any_cast<const Type::Variant&>(value); }

//
// Mapping methods to convert between abstract Generic types, to specific classes
//
DBus::Type::Generic DBus::Type::create(const std::string& type, bool littleEndian)
{
#define TYPE_CREATE(typename)                        \
    if (type[0] == typename ::s_StaticTypeCode[0]) { \
        typename vv;                                 \
        vv.setSignature(type);                       \
        vv.setLittleEndian(littleEndian);            \
        Type::Generic v(vv);                         \
        return v;                                    \
    }

    TYPE_CREATE(Type::Byte);
    TYPE_CREATE(Type::Boolean);
    TYPE_CREATE(Type::Int16);
    TYPE_CREATE(Type::Uint16);
    TYPE_CREATE(Type::Int32);
    TYPE_CREATE(Type::Uint32);
    TYPE_CREATE(Type::Int64);
    TYPE_CREATE(Type::Uint64);
    TYPE_CREATE(Type::Double);
    TYPE_CREATE(Type::String);
    TYPE_CREATE(Type::Array);
    TYPE_CREATE(Type::Variant);
    TYPE_CREATE(Type::Struct);
    TYPE_CREATE(Type::ObjectPath);
    TYPE_CREATE(Type::Signature);
    TYPE_CREATE(Type::DictEntry);

#undef TYPE_CREATE

    Log::write(DBus::Log::ERROR, "DBus :: Can not create a signature type of (%s)\n", type.c_str());

    return Type::Generic();
}

std::string DBus::Type::getMarshallingSignature(const DBus::Type::Generic& value)
{
    // It is possible to receive an empty value when dealing with void parameter lists.
    if (value.empty()) {
        return "";
    }
#define TYPE_MARSHALL_SIGNATURE(typename)          \
    {                                              \
        static Type::Generic is_type = typename(); \
        if (ti == is_type.type()) {                \
            return typename ::s_StaticTypeCode;    \
        }                                          \
    }
//		if (ti == is_type.type()) {	return boost::any_cast<typename>(value).getSignature(); } }
#define TYPE_MARSHALL_SIGNATURE2(typename)                          \
    {                                                               \
        static Type::Generic is_type = typename();                  \
        if (ti == is_type.type()) {                                 \
            return boost::any_cast<typename>(value).getSignature(); \
        }                                                           \
    }
    const std::type_info& ti = value.type();

    TYPE_MARSHALL_SIGNATURE(Type::Byte);
    TYPE_MARSHALL_SIGNATURE(Type::Boolean);
    TYPE_MARSHALL_SIGNATURE(Type::Int16);
    TYPE_MARSHALL_SIGNATURE(Type::Uint16);
    TYPE_MARSHALL_SIGNATURE(Type::Int32);
    TYPE_MARSHALL_SIGNATURE(Type::Uint32);
    TYPE_MARSHALL_SIGNATURE(Type::Int64);
    TYPE_MARSHALL_SIGNATURE(Type::Uint64);
    TYPE_MARSHALL_SIGNATURE(Type::Double);
    TYPE_MARSHALL_SIGNATURE(Type::String);
    TYPE_MARSHALL_SIGNATURE2(Type::Array);
    TYPE_MARSHALL_SIGNATURE(Type::Variant);
    TYPE_MARSHALL_SIGNATURE2(Type::Struct);
    TYPE_MARSHALL_SIGNATURE(Type::ObjectPath);
    TYPE_MARSHALL_SIGNATURE(Type::Signature);
    TYPE_MARSHALL_SIGNATURE2(Type::DictEntry);

    Log::write(DBus::Log::ERROR, "DBus :: Marshalling signature type not implemented (%s)\n", ti.name());

#undef TYPE_MARSHALL_SIGNATURE

    return "?";
}

// TODO: macro-in-a-macro to try and reduce duplicated list of types

void DBus::Type::marshallData(const DBus::Type::Generic& value, MessageOStream& stream)
{
    // It is possible to receive an empty value when dealing with void parameter lists.
    if (value.empty()) {
        return;
    }
    std::string type(getMarshallingSignature(value));

#define TYPE_MARSHALL_DATA(typename)                       \
    if (type[0] == typename ::s_StaticTypeCode[0]) {       \
        boost::any_cast<typename>(value).marshall(stream); \
        return;                                            \
    }

    TYPE_MARSHALL_DATA(Type::Byte);
    TYPE_MARSHALL_DATA(Type::Boolean);
    TYPE_MARSHALL_DATA(Type::Int16);
    TYPE_MARSHALL_DATA(Type::Uint16);
    TYPE_MARSHALL_DATA(Type::Int32);
    TYPE_MARSHALL_DATA(Type::Uint32);
    TYPE_MARSHALL_DATA(Type::Int64);
    TYPE_MARSHALL_DATA(Type::Uint64);
    TYPE_MARSHALL_DATA(Type::Double);
    TYPE_MARSHALL_DATA(Type::String);
    TYPE_MARSHALL_DATA(Type::Array);
    TYPE_MARSHALL_DATA(Type::Variant);
    TYPE_MARSHALL_DATA(Type::Struct);
    TYPE_MARSHALL_DATA(Type::ObjectPath);
    TYPE_MARSHALL_DATA(Type::Signature);
    TYPE_MARSHALL_DATA(Type::DictEntry);

    Log::write(DBus::Log::ERROR, "DBus :: Marshalling data of type %s is not implemented.\n", type.c_str());

#undef TYPE_MARSHALL_DATA
}

// Process one octet of data, into the given type. Returns true if that type has completed.
void DBus::Type::unmarshallData(DBus::Type::Generic& result, MessageIStream& stream)
{
// TODO: Consider whether a template could handle this
#define TYPE_UNMARSHALL_DATA(typename)                    \
    if (type[0] == typename ::s_StaticTypeCode[0]) {      \
        typename& v = boost::any_cast<typename&>(result); \
        v.unmarshall(stream);                             \
        return;                                           \
    }

    std::string type = getMarshallingSignature(result);

    TYPE_UNMARSHALL_DATA(Type::Byte);
    TYPE_UNMARSHALL_DATA(Type::Boolean);
    TYPE_UNMARSHALL_DATA(Type::Int16);
    TYPE_UNMARSHALL_DATA(Type::Uint16);
    TYPE_UNMARSHALL_DATA(Type::Int32);
    TYPE_UNMARSHALL_DATA(Type::Uint32);
    TYPE_UNMARSHALL_DATA(Type::Int64);
    TYPE_UNMARSHALL_DATA(Type::Uint64);
    TYPE_UNMARSHALL_DATA(Type::Double);
    TYPE_UNMARSHALL_DATA(Type::String);
    TYPE_UNMARSHALL_DATA(Type::Array);
    TYPE_UNMARSHALL_DATA(Type::Variant);
    TYPE_UNMARSHALL_DATA(Type::Struct);
    TYPE_UNMARSHALL_DATA(Type::ObjectPath);
    TYPE_UNMARSHALL_DATA(Type::Signature);
    TYPE_UNMARSHALL_DATA(Type::DictEntry);

    Log::write(DBus::Log::ERROR, "DBus :: Unmarshalling type not implemented (%s)\n", type.c_str());

#undef TYPE_UNMARSHALL_DATA

}

// Generate an expanded description of the data type provided.
std::string DBus::Type::toString(const DBus::Type::Generic& value, const std::string& prefix)
{
#define TYPE_TOSTRING(typename)                                              \
    {                                                                        \
        static Type::Generic is_type = typename();                           \
        if (ti == is_type.type()) {                                          \
            return boost::any_cast<const typename&>(value).toString(prefix); \
        }                                                                    \
    }
    const std::type_info& ti = value.type();

    TYPE_TOSTRING(Type::Byte);
    TYPE_TOSTRING(Type::Boolean);
    TYPE_TOSTRING(Type::Int16);
    TYPE_TOSTRING(Type::Uint16);
    TYPE_TOSTRING(Type::Int32);
    TYPE_TOSTRING(Type::Uint32);
    TYPE_TOSTRING(Type::Int64);
    TYPE_TOSTRING(Type::Uint64);
    TYPE_TOSTRING(Type::Double);
    TYPE_TOSTRING(Type::String);
    TYPE_TOSTRING(Type::Array);
    TYPE_TOSTRING(Type::Variant);
    TYPE_TOSTRING(Type::Struct);
    TYPE_TOSTRING(Type::ObjectPath);
    TYPE_TOSTRING(Type::Signature);
    TYPE_TOSTRING(Type::DictEntry);

#undef TYPE_TOSTRING

    Log::write(DBus::Log::ERROR, "DBus :: toString type not implemented (%s)\n", ti.name());

    return "?unknown?";
}

size_t DBus::Type::getAlignment(const std::string& declaration)
{
#define TYPE_UNMARSHALL_ALIGNMENT(typename)                 \
    if (declaration[0] == typename ::s_StaticTypeCode[0]) { \
        typename vv;                                        \
        return vv.getAlignment();                           \
    }

    TYPE_UNMARSHALL_ALIGNMENT(Type::Byte);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Boolean);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Int16);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Uint16);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Int32);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Uint32);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Int64);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Uint64);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Double);
    TYPE_UNMARSHALL_ALIGNMENT(Type::String);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Array);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Variant);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Struct);
    TYPE_UNMARSHALL_ALIGNMENT(Type::ObjectPath);
    TYPE_UNMARSHALL_ALIGNMENT(Type::Signature);
    TYPE_UNMARSHALL_ALIGNMENT(Type::DictEntry);

#undef TYPE_UNMARSHALL_ALIGNMENT

    return 1;
}

//
// Other methods
//
std::string DBus::Type::extractSignature(const std::string& declaration, size_t idx)
{
    std::string result;

    // Handle arrays
    if (declaration[idx] == '(') {
        // Find the matching bracket, and skip all internal signatures
        size_t number_of_brackets = 0;
        for (size_t i = idx; i < declaration.size(); ++i) {
            result += declaration[i];

            if (declaration[i] == '(') {
                ++number_of_brackets;
            } else if (declaration[i] == ')') {
                if (--number_of_brackets == 0) {
                    return result;
                }
            }
        }
        //
        Log::write(Log::ERROR, "DBus :: The declaration is invalid due to mis-matched brackets in the array signature.");
    }
    // Handle dictentry
    else if (declaration[idx] == '{') {
        if (idx == 0) {
            Log::write(Log::ERROR, "DBus :: The declaration is invalid because a dictentry must be inside an array container type.");
        }
        // Find the matching brace, and skip all internal signatures
        size_t number_of_brackets = 0;
        for (size_t i = idx; i < declaration.size(); ++i) {
            result += declaration[i];

            if (declaration[i] == '{') {
                ++number_of_brackets;
            } else if (declaration[i] == '}') {
                if (--number_of_brackets == 0) {
                    return result;
                }
            }
        }
        //
        Log::write(Log::ERROR, "DBus :: The declaration is invalid due to mis-matched braces in the dictentry signature.");
    }

    result += declaration[idx];

    if (declaration[idx] == 'a') {
        result += extractSignature(declaration, idx + 1);
    }

    return result;
}

std::string DBus::Type::getMarshallingSignature(const std::vector<DBus::Type::Generic>& value)
{
    std::string result;

    for (auto it : value) {
        result += getMarshallingSignature(it);
    }
    return result;
}
