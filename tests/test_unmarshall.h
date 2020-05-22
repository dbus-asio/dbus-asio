#include <catch2/catch.hpp>
#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include <byteswap.h>
#include <string>
#include <iostream>

namespace DBus { namespace test {

int16_t swap(int16_t value)
{
    return bswap_16(value);
}

uint16_t swap(uint16_t value)
{
    return bswap_16(value);
}

int32_t swap(int32_t value)
{
    return bswap_32(value);
}

uint32_t swap(uint32_t value)
{
    return bswap_32(value);
}

int64_t swap(int64_t value)
{
    return bswap_64(value);
}

uint64_t swap(uint64_t value)
{
    return bswap_64(value);
}

template<class T, class D>
void TestUnmarshall(unsigned byteOrder, T value, D &dbusType)
{
    UnmarshallingData data;
    const T readValue = byteOrder == __BYTE_ORDER ? value : swap(value);

    char * bytePtr = (char*)&readValue;
    for (auto i = 0; i < sizeof(T); ++i, ++bytePtr) {
        data.c = *bytePtr;
        if (i != sizeof(T) - 1) {
            REQUIRE(!dbusType.unmarshall(data));
        } else {
            REQUIRE(dbusType.unmarshall(data));
        }
        ++data.offset;
    }

    REQUIRE(dbusType.asString() == std::to_string(value));
}

template<class T, class D>
void TestUnmarshallFromStream(T value, D& dbusType, MessageOStream& stream)
{
    UnmarshallingData data;
    std::for_each(stream.data.begin(), stream.data.end(),
        [&data, &dbusType](char c) {
            data.c = c;
            dbusType.unmarshall(data);
        });

    REQUIRE(dbusType.asString() ==  std::to_string(value));
}

}} // namespace DBus::test
