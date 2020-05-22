#include <catch2/catch.hpp>
#include "dbus_type_string.h"
#include "dbus_type_int32.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"
#include "dbus_type.h"
#include <iostream>

namespace DBus { namespace test {

void TestUnmarshallFromStream(const std::string &stream,
    unsigned byteOrder, const std::string str)
{
    // An array of structures
    Type::String dbusString;
    dbusString.setLittleEndian(byteOrder == __LITTLE_ENDIAN);

    UnmarshallingData data;
    for (auto i = 0; i < stream.size(); ++i) {
        data.c = stream[i];
        if (i != stream.size() - 1) {
            REQUIRE(!dbusString.unmarshall(data));
        } else {
            REQUIRE(dbusString.unmarshall(data));
        }
        ++data.offset;
    }

    REQUIRE(dbusString.asString() == str);
}

void TestUnmarshall(unsigned byteOrder, const std::string& str)
{
    std::string stream;
    uint32_t length = str.size();

    if (byteOrder != __BYTE_ORDER) {
        length = __bswap_32(length);
    }

    stream.append((char*)&length, 4); // Length of array
    stream.append(str);
    stream.push_back('\0');

    TestUnmarshallFromStream(stream, byteOrder, str);
}

TEST_CASE("Unmarshall string - little endian")
{
    TestUnmarshall(__LITTLE_ENDIAN, "This is a string");
}

TEST_CASE("Unmarshall string - big endian")
{
    TestUnmarshall(__BIG_ENDIAN, "This is another string");
}

TEST_CASE("Marshall and unmarshall string")
{
    const std::string str("An another string");
    Type::String dbusString(str);

    MessageStream stream;
    dbusString.marshall(stream);

   TestUnmarshallFromStream(stream.data, __LITTLE_ENDIAN, str);
}


}} // namespace DBus::test

using namespace DBus::test;
