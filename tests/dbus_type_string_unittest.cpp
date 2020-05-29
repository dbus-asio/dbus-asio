#include <catch2/catch.hpp>
#include "dbus_type_string.h"
#include "dbus_type_int32.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"
#include "dbus_messageistream.h"
#include "dbus_type.h"
#include <iostream>

namespace DBus { namespace test {

void TestUnmarshallFromMessageIStream(const std::string &stream,
    unsigned byteOrder, const std::string str)
{
    // An array of structures
    Type::String dbusString;
    MessageIStream istream((uint8_t*)stream.data(), stream.size(), byteOrder != __LITTLE_ENDIAN);
    dbusString.unmarshall(istream);

    REQUIRE(dbusString.asString() == str);
}

void TestUnmarshallFromMessageIStream(unsigned byteOrder, const std::string& str)
{
    std::string stream;
    uint32_t length = str.size();

    if (byteOrder != __BYTE_ORDER) {
        length = __bswap_32(length);
    }

    stream.append((char*)&length, 4); // Length of array
    stream.append(str);
    stream.push_back('\0');

    TestUnmarshallFromMessageIStream(stream, byteOrder, str);
}

TEST_CASE("Unmarshall string - little endian from MessageIStream")
{
    TestUnmarshallFromMessageIStream(__LITTLE_ENDIAN, "This is a string");
}

TEST_CASE("Unmarshall string - big endian from MessageIStream")
{
    TestUnmarshallFromMessageIStream(__BIG_ENDIAN, "This is another string");
}

TEST_CASE("Marshall and unmarshall string")
{
    const std::string str("An another string");
    Type::String dbusString(str);

    MessageOStream stream;
    dbusString.marshall(stream);

   TestUnmarshallFromMessageIStream(stream.data, __LITTLE_ENDIAN, str);
}


}} // namespace DBus::test

using namespace DBus::test;
