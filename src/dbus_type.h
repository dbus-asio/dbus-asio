#ifndef DBUS_TYPE_H
#define DBUS_TYPE_H

#include <boost/any.hpp>
#include <string>
#include <vector>

// Our base type, used for implementation convenience
#include "dbus_type_base.h"

namespace DBus {
class MessageStream;
class UnmarshallingData;

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

    void marshallData(const DBus::Type::Generic& any, MessageStream& stream);

    bool unmarshallData(DBus::Type::Generic& result, const UnmarshallingData& data);
    Type::Generic unmarshallString(const std::string& data);

    void unmarshallToStruct(DBus::Type::Struct& dest, const std::string& data);

    std::string toString(const DBus::Type::Generic& data, const std::string& prefix = "");

    size_t getAlignment(const std::string& declaration);

    std::string extractSignature(const std::string& declaration, size_t idx);

    std::string getMarshallingSignature(const DBus::Type::Generic& value);
    std::string getMarshallingSignature(const std::vector<DBus::Type::Generic>& value);
}
}

#endif
