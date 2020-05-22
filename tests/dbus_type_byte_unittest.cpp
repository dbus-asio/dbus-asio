#include <catch2/catch.hpp>
#include "dbus_type_byte.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"

namespace DBus { namespace test {

TEST_CASE("Unmarshall byte")
{
    Type::Byte byte;

    UnmarshallingData data;
    data.c = 123;
    REQUIRE(byte.unmarshall(data) == true);
    REQUIRE(byte.asString() == std::to_string((size_t)data.c));
}

TEST_CASE("Marshall and unmarshall byte")
{
    MessageOStream stream;
    stream.writeByte(213);

    Type::Byte byte;
    UnmarshallingData data;
    data.c = stream.data[0];

    REQUIRE(byte.unmarshall(data) == true);
    REQUIRE(byte.asString() == std::to_string(213));
}


}} // namespace DBus::test

using namespace DBus::test;
