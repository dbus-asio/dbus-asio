#include "dbus_messageistream.h"
#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include "dbus_type_byte.h"
#include <catch2/catch.hpp>

namespace DBus {
namespace test {

    TEST_CASE("Unmarshall from MessageIStream")
    {
        std::basic_string<uint8_t> data;
        data.push_back(65);
        MessageIStream istream(data.data(), data.size(), false);

        Type::Byte byte;
        byte.unmarshall(istream);
        REQUIRE(byte.asString() == std::to_string((size_t)65));
    }

    TEST_CASE("Marshall and unmarshall byte")
    {
        MessageOStream stream;
        stream.writeByte(213);

        MessageIStream istream((uint8_t*)stream.data.data(), stream.data.size(),
            false);
        Type::Byte byte;
        byte.unmarshall(istream);
        REQUIRE(byte.asString() == std::to_string(213));
    }

} // namespace test
} // namespace DBus

using namespace DBus::test;
