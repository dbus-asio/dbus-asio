#include <catch2/catch.hpp>
#include "dbus_type_variant.h"
#include "dbus_type_uint32.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"
#include <byteswap.h>

namespace DBus { namespace test {

void TestUnmarshall(unsigned byteOrder, const std::string& stream, uint32_t value)
{
    Type::Variant variant;
    variant.setLittleEndian(byteOrder == __LITTLE_ENDIAN);
    UnmarshallingData data;
    for (auto i = 0; i < stream.size(); ++i) {
        data.c = stream[i];
        if (i != stream.size() - 1 ) {
            REQUIRE(!variant.unmarshall(data));
        } else {
            REQUIRE(variant.unmarshall(data));
        }
        ++data.offset;
    }

    REQUIRE(variant.asString() == std::to_string(value));
}

void TestUnmarshall(unsigned byteOrder, uint32_t value)
{
    std::string data;
    // UINT32 signature
    data.push_back(1);
    data.push_back('u');
    data.push_back('\0');

    // Padding
    data.push_back('\0');

    // UINT32 value
    uint32_t writeValue = byteOrder == __BYTE_ORDER ? value : bswap_32(value);
    data.append((char*)&writeValue, sizeof(uint32_t));

    TestUnmarshall(byteOrder, data, value);
}

TEST_CASE("Unmarshall variant little endian")
{
    TestUnmarshall(__LITTLE_ENDIAN, 12345);
}

TEST_CASE("Unmarshall variant big endian")
{
    TestUnmarshall(__BIG_ENDIAN, 5668434);
}

TEST_CASE("Marshall and unmarshall variant")
{
    Type::Uint32 dbusInt(42);
    Type::Variant variant(dbusInt);

    MessageOStream stream;
    variant.marshall(stream);
    TestUnmarshall(__LITTLE_ENDIAN, stream.data, 42);
}


}} // namespace DBus::test

using namespace DBus::test;
