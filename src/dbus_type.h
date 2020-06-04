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

#ifndef DBUS_TYPE_H
#define DBUS_TYPE_H

#include <boost/any.hpp>
#include <string>
#include <vector>

// Our base type, used for implementation convenience
#include "dbus_type_base.h"

namespace DBus {
class MessageOStream;
class MessageIStream;

namespace Type {
    class Array;
    class Struct;
    class Signature;
    class Variant;

    // Helper methods to return native types
    uint8_t asByte(const Generic& v);
    uint32_t asUint32(const Generic& v);
    std::string asString(const Generic& v);
    std::string asObjectPath(const Generic& v);

    // Helper methods to return type-safe DBus references
    const Type::Array& refArray(const Generic& v);
    const Type::Struct& refStruct(const Generic& v);
    const Type::Signature& refSignature(const Generic& v);
    const Type::Variant& refVariant(const Generic& v);

    // Generic type handling code
    DBus::Type::Generic create(const std::string& type);

    void marshallData(const DBus::Type::Generic& any, MessageOStream& stream);
    void unmarshallData(DBus::Type::Generic& result, MessageIStream& stream);

    std::string toString(const DBus::Type::Generic& data, const std::string& prefix = "");

    size_t getAlignment(const std::string& declaration);

    std::string extractSignature(const std::string& declaration, size_t idx);

    std::string getMarshallingSignature(const DBus::Type::Generic& value);
    std::string getMarshallingSignature(const std::vector<DBus::Type::Generic>& value);
}
}

#endif
