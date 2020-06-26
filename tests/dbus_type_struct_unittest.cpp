#include "dbus_messageistream.h"
#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include "dbus_type_struct.h"
#include "dbus_type_uint32.h"
#include <byteswap.h>
#include <catch2/catch.hpp>

namespace DBus {
namespace test {

    void TestUnmarshallFromMessageIStream(unsigned byteOrder,
        const std::string& stream, uint32_t v1,
        uint32_t v2)
    {
        Type::Struct dbusStruct;
        dbusStruct.setSignature("(uu)");

        MessageIStream istream((uint8_t*)stream.data(), stream.size(),
            byteOrder != __LITTLE_ENDIAN);
        dbusStruct.unmarshall(istream);

        REQUIRE(dbusStruct.getEntries() == 2);
        REQUIRE(Type::asString(dbusStruct[0]) == std::to_string(v1));
        REQUIRE(Type::asString(dbusStruct[1]) == std::to_string(v2));
    }

    void TestUnmarshallMessageIStream(unsigned byteOrder, uint32_t v1,
        uint32_t v2)
    {
        std::string data;

        uint32_t writeKey = byteOrder == __BYTE_ORDER ? v1 : bswap_32(v1);
        uint32_t writeValue = byteOrder == __BYTE_ORDER ? v2 : bswap_32(v2);
        data.append((char*)&writeKey, sizeof(uint32_t));
        data.append((char*)&writeValue, sizeof(uint32_t));

        TestUnmarshallFromMessageIStream(byteOrder, data, v1, v2);
    }

    TEST_CASE("Unmarshall struct little endian from MessageIStream")
    {
        TestUnmarshallMessageIStream(__LITTLE_ENDIAN, 12345, 864);
    }

    TEST_CASE("Unmarshall struct big endian from MessageIStream")
    {
        TestUnmarshallMessageIStream(__BIG_ENDIAN, 1, 2);
    }

    TEST_CASE("Marshall and unmarshall struct")
    {
        Type::Uint32 uint1(56);
        Type::Uint32 uint2(78);
        Type::Struct dbusStruct;
        dbusStruct.add(uint1);
        dbusStruct.add(uint2);

        MessageOStream stream;
        dbusStruct.marshall(stream);
        TestUnmarshallFromMessageIStream(__LITTLE_ENDIAN, stream.data, 56, 78);
    }

} // namespace test
} // namespace DBus

using namespace DBus::test;
