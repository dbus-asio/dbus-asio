#include "dbus_messageistream.h"
#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include "dbus_type.h"
#include "dbus_type_int32.h"
#include "dbus_type_string.h"
#include <catch2/catch.hpp>
#include <iostream>

namespace DBus {
namespace test {

    void TestUnmarshallFromMessageIStream(const std::string& stream,
        unsigned byteOrder,
        const std::string str)
    {
        // An array of structures
        Type::String dbusString;
        MessageIStream istream((uint8_t*)stream.data(), stream.size(),
            byteOrder != __LITTLE_ENDIAN);
        dbusString.unmarshall(istream);

        REQUIRE(dbusString.asString() == str);
    }

    void TestUnmarshallFromMessageIStream(
        unsigned byteOrder, const std::string& str,
        std::string::size_type truncate_at = std::string::npos)
    {
        std::string stream;
        uint32_t length = str.size();

        if (byteOrder != __BYTE_ORDER) {
            length = __bswap_32(length);
        }

        stream.append((char*)&length, 4); // Length of array
        stream.append(str);
        stream.push_back('\0');

        if (stream.size() > truncate_at)
            stream.erase(truncate_at);

        fprintf(stderr, "%u\n", stream.size());

        TestUnmarshallFromMessageIStream(stream, byteOrder, str);
    }

    void TestUnmarshallFromMessageIStream(const std::string& str)
    {
        SECTION("Little endian")
        {
            TestUnmarshallFromMessageIStream(__LITTLE_ENDIAN, "This is a string");
        }

        SECTION("Big endian")
        {
            TestUnmarshallFromMessageIStream(__BIG_ENDIAN, "This is a string");
        }
    }

    TEST_CASE("Unmarshall string")
    {
        TestUnmarshallFromMessageIStream("This is a string");
    }

    TEST_CASE("Unmarshall string with control characters - from MessageIStream")
    {
        TestUnmarshallFromMessageIStream(
            "Control characters \r\n\n\r\b \xc2\xa3 \xf0\x9f\x92\xa9");
    }

    TEST_CASE("Unmarshall string with bad UTF-8")
    {
        // 0xc2 is a lead octet for a two-octet UTF-8 character.
        // 0xf0 is a lead octet for a four-octet UTF-8 character.
        TestUnmarshallFromMessageIStream("Not UTF-8 \xc2. 0\xf0");
    }

    TEST_CASE("Truncated stream")
    {
        for (const auto endian : { __LITTLE_ENDIAN, __BIG_ENDIAN }) {
            INFO(((endian == __LITTLE_ENDIAN) ? "Little endian" : "Big endian"));

            const std::string test_string { "To be truncated" };
            SECTION("Remove terminator")
            {
                REQUIRE_THROWS_WITH(TestUnmarshallFromMessageIStream(
                                        __LITTLE_ENDIAN, "Not UTF-8 \xc2. 0\xf0", 19),
                    "OctetBuffer index out of range error");
            }
            SECTION("Remove some of string")
            {
                REQUIRE_THROWS_WITH(TestUnmarshallFromMessageIStream(
                                        __LITTLE_ENDIAN, "Not UTF-8 \xc2. 0\xf0", 16),
                    "Read string error: Not enough data in stream");
            }
            SECTION("Remove string body")
            {
                REQUIRE_THROWS_WITH(TestUnmarshallFromMessageIStream(
                                        __LITTLE_ENDIAN, "Not UTF-8 \xc2. 0\xf0", 4),
                    "Read string error: Not enough data in stream");
            }
            SECTION("Remove some of length 3")
            {
                REQUIRE_THROWS_WITH(TestUnmarshallFromMessageIStream(
                                        __LITTLE_ENDIAN, "Not UTF-8 \xc2. 0\xf0", 3),
                    "OctetBuffer copy error: Not enough data in buffer");
            }
            SECTION("Remove some of length 2")
            {
                REQUIRE_THROWS_WITH(TestUnmarshallFromMessageIStream(
                                        __LITTLE_ENDIAN, "Not UTF-8 \xc2. 0\xf0", 2),
                    "OctetBuffer copy error: Not enough data in buffer");
            }
            SECTION("Remove some of length 1")
            {
                REQUIRE_THROWS_WITH(TestUnmarshallFromMessageIStream(
                                        __LITTLE_ENDIAN, "Not UTF-8 \xc2. 0\xf0", 1),
                    "OctetBuffer copy error: Not enough data in buffer");
            }
            SECTION("Remove everything")
            {
                REQUIRE_THROWS_WITH(TestUnmarshallFromMessageIStream(
                                        __LITTLE_ENDIAN, "Not UTF-8 \xc2. 0\xf0", 0),
                    "OctetBuffer copy error: Not enough data in buffer");
            }
        }
    }

    TEST_CASE("Marshall and unmarshall string")
    {
        const std::string str("An another string");
        Type::String dbusString(str);

        MessageOStream stream;
        dbusString.marshall(stream);

        TestUnmarshallFromMessageIStream(stream.data, __LITTLE_ENDIAN, str);
    }

} // namespace test
} // namespace DBus

using namespace DBus::test;
