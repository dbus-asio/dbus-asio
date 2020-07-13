#include "dbus_messageistream.h"
#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include "dbus_type_byte.h"
#include "dbus_type_dictentry.h"
#include "dbus_type_double.h"
#include "dbus_type_int32.h"
#include "dbus_type_string.h"
#include "dbus_type_struct.h"
#include "dbus_type_uint32.h"
#include "dbus_type_variant.h"
#include <byteswap.h>
#include <catch2/catch.hpp>
#include <iomanip>
#include <iostream>

namespace DBus {
namespace test {

    void TestUnmarshallFromMessageIStream(unsigned byteOrder,
        const std::string& stream,
        const std::string& key, uint32_t value)
    {
        Type::DictEntry dictEntry;
        dictEntry.setSignature("{su}");
        MessageIStream istream((uint8_t*)stream.data(), stream.size(),
            byteOrder != __LITTLE_ENDIAN);
        dictEntry.unmarshall(istream);

        std::stringstream ss;
        ss << "DictEntry ({su}) : {" << std::endl;
        ss << "   key:      String (" << key.size() << ") \"" << key << "\""
           << std::endl;
        ss << "   value:    Uint32 " << value << " (0x";
        ss << std::hex << std::setw(4) << std::setfill('0') << value;
        ss << std::dec << std::setw(0) << ")" << std::endl;
        ss << "}" << std::endl;

        REQUIRE(dictEntry.toString() == ss.str());
    }

    void TestUnmarshallFromMessageIStream(unsigned byteOrder,
        const std::string& key, uint32_t value)
    {
        std::string data;

        uint32_t length = key.size();
        if (byteOrder != __BYTE_ORDER) {
            length = __bswap_32(length);
        }
        data.append((char*)&length, 4); // Length of array
        data.append(key);
        data.push_back('\0');

        while (data.size() % 4 != 0)
            data.push_back('\0');

        uint32_t writeValue = byteOrder == __BYTE_ORDER ? value : bswap_32(value);
        data.append((char*)&writeValue, sizeof(uint32_t));

        TestUnmarshallFromMessageIStream(byteOrder, data, key, value);
    }

    void TestUnmarshallFromMessageIStream(unsigned byteOrder,
        const std::string& stream,
        const int32_t key,
        const std::pair<double, uint8_t>& value)
    {
        Type::DictEntry dictEntry;
        dictEntry.setSignature("{i(dy)}");
        MessageIStream istream((uint8_t*)stream.data(), stream.size(),
            byteOrder != __LITTLE_ENDIAN);
        dictEntry.unmarshall(istream);

        std::stringstream ss;
        ss << "DictEntry ({i(dy)}) : {" << std::endl;
        ss << "   key:      Int32 " << key << " (0x";
        ss << std::hex << std::setw(4) << std::setfill('0') << key;
        ss << std::dec << std::setw(0) << ")" << std::endl;
        ss << "   value:    Struct (dy) <" << std::endl;
        ss << "      Double " << value.first << std::endl;
        ss << "      Byte " << static_cast<unsigned int>(value.second) << " (0x";
        ss << std::hex << std::setw(2) << std::setfill('0')
           << static_cast<unsigned int>(value.second);
        ss << std::dec << std::setw(0) << ")" << std::endl;
        ss << "   >" << std::endl;
        ss << "}" << std::endl;

        REQUIRE(dictEntry.toString() == ss.str());
    }

    void TestUnmarshallFromMessageIStream(unsigned byteOrder, const int32_t key,
        const std::pair<double, uint8_t>& value)
    {
        std::string data;

        uint32_t key_to_write = key;
        if (byteOrder != __BYTE_ORDER) {
            key_to_write = __bswap_32(key_to_write);
        }
        data.append((char*)&key_to_write, 4);

        while (data.size() % 8 != 0)
            data.push_back('\0');

        uint64_t value1;
        memcpy(&value1, &value.first, sizeof(value1));
        if (byteOrder != __BYTE_ORDER) {
            value1 = __bswap_64(value1);
        }

        data.append((char*)&value1, 8);
        data.append((char*)&value.second, 1);

        TestUnmarshallFromMessageIStream(byteOrder, data, key, value);
    }

    TEST_CASE("Unmarshall dictionary entry little endian from MessageIStream")
    {
        TestUnmarshallFromMessageIStream(__LITTLE_ENDIAN, "Hello", 864);
    }

    TEST_CASE("Unmarshall dictionary entry big endian from MessageIStream")
    {
        TestUnmarshallFromMessageIStream(__BIG_ENDIAN, "Goodbye", 2);
    }

    TEST_CASE("Unmarshall dictionary entry little endian with struct value")
    {
        TestUnmarshallFromMessageIStream(
            __LITTLE_ENDIAN, -42, std::make_pair(3.25, static_cast<uint8_t>(85)));
    }

    TEST_CASE("Unmarshall dictionary entry big endian with struct value")
    {
        TestUnmarshallFromMessageIStream(
            __BIG_ENDIAN, -77, std::make_pair(404.25, static_cast<uint8_t>(42)));
    }

    TEST_CASE("Marshall and unmarshall dictionary entry")
    {
        Type::String dbusStringKey("Dictionary");
        Type::Uint32 dbusIntValue(24);
        Type::DictEntry dictEntry(dbusStringKey, dbusIntValue);

        MessageOStream stream;
        dictEntry.marshall(stream);
        TestUnmarshallFromMessageIStream(__LITTLE_ENDIAN, stream.data, "Dictionary",
            24);
    }

    TEST_CASE("Marshall and unmarshall dictionary entry with struct value")
    {
        Type::Int32 dbusIntKey(85);
        Type::Double dbusDoubleValue(4.5);
        Type::Byte dbusByteValue(32);
        Type::Struct dbusStructValue;
        dbusStructValue.add(dbusDoubleValue);
        dbusStructValue.add(dbusByteValue);
        Type::DictEntry dictEntry(dbusIntKey, dbusStructValue);

        REQUIRE(dictEntry.getSignature() == "{i(dy)}");

        MessageOStream stream;
        dictEntry.marshall(stream);
        TestUnmarshallFromMessageIStream(
            __LITTLE_ENDIAN, stream.data, 85,
            std::make_pair(4.5, static_cast<uint8_t>(32)));
    }

    TEST_CASE("Invalid basic type for key input")
    {
        std::array<uint8_t, 22> encoded = {
            0x01,
            0x73,
            0x00,
            0x00,
            0x03,
            0x00,
            0x00,
            0x00,
            0x4b,
            0x65,
            0x79,
            0x00,
            0x05,
            0x00,
            0x00,
            0x00,
            0x56,
            0x61,
            0x6c,
            0x75,
            0x65,
            0x00,
        };

        Type::DictEntry dictEntry;
        dictEntry.setSignature("{vu}");
        MessageIStream istream(encoded.data(), encoded.size(), false);
        REQUIRE_THROWS_WITH(dictEntry.unmarshall(istream), "Invalid basic type: v");
    }

} // namespace test
} // namespace DBus

using namespace DBus::test;
