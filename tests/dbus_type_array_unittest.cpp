#include <catch2/catch.hpp>
#include "dbus_type_array.h"
#include "dbus_type_int32.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"
#include "dbus_messageistream.h"
#include "dbus_type.h"
#include <byteswap.h>

namespace DBus { namespace test {

void TestUnmarshallFromMessageIStream(const std::string &stream,
    unsigned byteOrder, int32_t v1, int32_t v2)
{
    // An array of structures
    Type::Array array;
    array.setLittleEndian(byteOrder == __LITTLE_ENDIAN);
    array.setSignature("a(i)");

    MessageIStream istream((uint8_t*)stream.data(), stream.size(), byteOrder != __LITTLE_ENDIAN);
    array.unmarshall(istream);

    REQUIRE(array.size() == 2);

    const Type::Struct& struct1 = boost::any_cast<const Type::Struct&>(array.getContents()[0]);
    const Type::Struct& struct2 = boost::any_cast<const Type::Struct&>(array.getContents()[1]);

    REQUIRE(Type::asString(struct1[0]) == std::to_string(v1));
    REQUIRE(Type::asString(struct2[0]) == std::to_string(v2));
}

void TestUnmarshallFromMessageIStream(unsigned byteOrder, int32_t v1, int32_t v2)
{
    std::string stream;
    uint32_t arrayLength = 12;

    arrayLength = byteOrder == __BYTE_ORDER ? arrayLength : bswap_32(arrayLength);
    uint32_t writeV1 = byteOrder == __BYTE_ORDER ? v1 : bswap_32(v1);
    uint32_t writeV2 = byteOrder == __BYTE_ORDER ? v2 : bswap_32(v2);

    stream.append((char*)&arrayLength, 4); // Length of array
    stream.append(4, '\0'); // offset to first struct
    stream.append((char*)&writeV1, 4);
    stream.append(4, '\0'); // offset to next struct
    stream.append((char*)&writeV2, 4);

    TestUnmarshallFromMessageIStream(stream, byteOrder, v1, v2);
}

TEST_CASE("Unmarshall an array of structs from MessageIStream - little endian")
{
    TestUnmarshallFromMessageIStream(__LITTLE_ENDIAN, 1234, 5678);
}

TEST_CASE("Unmarshall array of structss from MessageIStream - big endian")
{
    TestUnmarshallFromMessageIStream(__BIG_ENDIAN, 1234, 5678);
}

TEST_CASE("Marshall and unmarshall array of structs")
{
    Type::Array array;
    Type::Struct struct1;
    DBus::Type::Int32 value1(-1);
    Type::Struct struct2;
    DBus::Type::Int32 value2(24567);

    struct1.add(value1);
    struct2.add(value2);
    array.setSignature("a(i)");
    array.add(struct1);
    array.add(struct2);
    MessageOStream stream;
    array.marshall(stream);

    TestUnmarshallFromMessageIStream(stream.data, __LITTLE_ENDIAN, -1, 24567);
}


}} // namespace DBus::test

using namespace DBus::test;
