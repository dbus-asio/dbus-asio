#include <catch2/catch.hpp>
#include "dbus_type_objectpath.h"
#include "dbus_type_string.h"
#include "dbus_type_variant.h"
#include "dbus_type_uint32.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"
#include "dbus_messageistream.h"
#include <byteswap.h>

namespace DBus { namespace test {

void TestUnmarshallFromMessageIStream(unsigned byteOrder, const std::string& stream, uint32_t value)
{
    Type::Variant variant;
    MessageIStream istream((uint8_t*)stream.data(), stream.size(), byteOrder != __LITTLE_ENDIAN);
    variant.unmarshall(istream);

    REQUIRE(variant.asString() == std::to_string(value));
}

void TestUnmarshallFromMessageIStream(unsigned byteOrder, uint32_t value)
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

    TestUnmarshallFromMessageIStream(byteOrder, data, value);
}

TEST_CASE("Unmarshall variant little endian from MessageIStream")
{
    TestUnmarshallFromMessageIStream(__LITTLE_ENDIAN, 12345);
}

TEST_CASE("Unmarshall variant big endian from MessageIStream")
{
    TestUnmarshallFromMessageIStream(__BIG_ENDIAN, 5668434);
}

TEST_CASE("Marshall and unmarshall variant")
{
    Type::Uint32 dbusInt(42);
    Type::Variant variant(dbusInt);

    MessageOStream stream;
    variant.marshall(stream);
    TestUnmarshallFromMessageIStream(__LITTLE_ENDIAN, stream.data, 42);
}

TEST_CASE("Round trip")
{
    Type::Variant input;

    SECTION("String")
    {
        input = Type::Variant(Type::String("Hello"));
    }

    SECTION("ObjectPath")
    {
        input = Type::Variant(Type::ObjectPath("/object/path"));
    }

    SECTION("Uint32")
    {
        input = Type::Variant(Type::Uint32(42U));
    }

    MessageOStream ostream;
    input.marshall(ostream);

    Type::Variant output;
    output.setSignature(input.getSignature());
    MessageIStream istream((uint8_t*)ostream.data.data(), ostream.data.size(), false);
    output.unmarshall(istream);

    REQUIRE(input.toString() == output.toString());
}

}} // namespace DBus::test

using namespace DBus::test;
