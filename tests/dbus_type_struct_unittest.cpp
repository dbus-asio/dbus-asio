#include <catch2/catch.hpp>
#include "dbus_type_struct.h"
#include "dbus_type_uint32.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"
#include <byteswap.h>

namespace DBus { namespace test {

void TestUnmarshall(unsigned byteOrder, const std::string& stream, uint32_t v1, uint32_t v2)
{
    Type::Struct dbusStruct;
    dbusStruct.setSignature("(uu)");
    dbusStruct.setLittleEndian(byteOrder == __LITTLE_ENDIAN);
    UnmarshallingData data;
    for (auto i = 0; i < stream.size(); ++i) {
        data.c = stream[i];
        if (i != stream.size() - 1 ) {
            REQUIRE(!dbusStruct.unmarshall(data));
        } else {
            REQUIRE(dbusStruct.unmarshall(data));
        }
        ++data.offset;
    }

    REQUIRE(dbusStruct.getEntries() == 2);
    REQUIRE(Type::asString(dbusStruct[0]) == std::to_string(v1));
    REQUIRE(Type::asString(dbusStruct[1]) == std::to_string(v2));
}

void TestUnmarshall(unsigned byteOrder, uint32_t v1, uint32_t v2)
{
    std::string data;

    uint32_t writeKey = byteOrder == __BYTE_ORDER ? v1 : bswap_32(v1);
    uint32_t writeValue = byteOrder == __BYTE_ORDER ? v2 : bswap_32(v2);
    data.append((char*)&writeKey, sizeof(uint32_t));
    data.append((char*)&writeValue, sizeof(uint32_t));

    TestUnmarshall(byteOrder, data, v1, v2);
}

TEST_CASE("Unmarshall struct little endian")
{
    TestUnmarshall(__LITTLE_ENDIAN, 12345, 864);
}

TEST_CASE("Unmarshall struct big endian")
{
    TestUnmarshall(__BIG_ENDIAN, 1, 2);
}

TEST_CASE("Marshall and unmarshall struct")
{
    Type::Uint32 uint1(56);
    Type::Uint32 uint2(78);
    Type::Struct dbusStruct;
    dbusStruct.add(uint1);
    dbusStruct.add(uint2);

    MessageStream stream;
    dbusStruct.marshall(stream);
    TestUnmarshall(__LITTLE_ENDIAN, stream.data, 56, 78);
}

}} // namespace DBus::test

using namespace DBus::test;
