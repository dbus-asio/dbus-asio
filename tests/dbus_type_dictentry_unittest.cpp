#include <catch2/catch.hpp>
#include "dbus_type_dictentry.h"
#include "dbus_type_uint32.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"
#include <byteswap.h>
#include <iomanip>

namespace DBus { namespace test {

void TestUnmarshall(unsigned byteOrder, const std::string& stream, uint32_t key, uint32_t value)
{
    Type::DictEntry dictEntry;
    dictEntry.setSignature("{uu}");
    dictEntry.setLittleEndian(byteOrder == __LITTLE_ENDIAN);
    UnmarshallingData data;
    for (auto i = 0; i < stream.size(); ++i) {
        data.c = stream[i];
        if (i != stream.size() - 1 ) {
            REQUIRE(!dictEntry.unmarshall(data));
        } else {
            REQUIRE(dictEntry.unmarshall(data));
        }
        ++data.offset;
    }
    std::stringstream ss;
    ss << "DictEntry ({uu}) : {" << std::endl;
    ss << "   key:      Uint32 " << key << " (0x";
    ss << std::hex << std::setw(4) << std::setfill('0') << key;
    ss << std::dec << std::setw(0) << ")" << std::endl;
    ss << "   value:    Uint32 " << value << " (0x";
    ss << std::hex << std::setw(4) << std::setfill('0') << value;
    ss << std::dec << std::setw(0) << ")" << std::endl;
    ss << "}" << std::endl;

    REQUIRE(dictEntry.toString() == ss.str());
}

void TestUnmarshall(unsigned byteOrder, uint32_t key, uint32_t value)
{
    std::string data;

    uint32_t writeKey = byteOrder == __BYTE_ORDER ? key : bswap_32(key);
    uint32_t writeValue = byteOrder == __BYTE_ORDER ? value : bswap_32(value);
    data.append((char*)&writeKey, sizeof(uint32_t));
    data.append((char*)&writeValue, sizeof(uint32_t));

    TestUnmarshall(byteOrder, data, key, value);
}

TEST_CASE("Unmarshall dictionary entry little endian")
{
    TestUnmarshall(__LITTLE_ENDIAN, 12345, 864);
}

TEST_CASE("Unmarshall dictionary entry big endian")
{
    TestUnmarshall(__BIG_ENDIAN, 1, 2);
}

TEST_CASE("Marshall and unmarshall dictionary entry")
{
    Type::Uint32 dbusIntKey(42);
    Type::Uint32 dbusIntValue(24);
    Type::DictEntry dictEntry(dbusIntKey, dbusIntValue);

    MessageOStream stream;
    dictEntry.marshall(stream);
    TestUnmarshall(__LITTLE_ENDIAN, stream.data, 42, 24);
}

}} // namespace DBus::test

using namespace DBus::test;
