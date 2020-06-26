#include "dbus_messageistream.h"
#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include <byteswap.h>
#include <catch2/catch.hpp>
#include <iostream>
#include <string>

namespace DBus {
namespace test {

    inline int16_t swap(int16_t value) { return bswap_16(value); }

    inline uint16_t swap(uint16_t value) { return bswap_16(value); }

    inline int32_t swap(int32_t value) { return bswap_32(value); }

    inline uint32_t swap(uint32_t value) { return bswap_32(value); }

    inline int64_t swap(int64_t value) { return bswap_64(value); }

    inline uint64_t swap(uint64_t value) { return bswap_64(value); }

    template <class T, class D>
    void TestUnmarshallFromMessageIStream(T value, D& dbusType,
        MessageIStream& istream)
    {
        dbusType.unmarshall(istream);
        REQUIRE(dbusType.asString() == std::to_string(value));
    }

    template <class T, class D>
    void TestUnmarshallFromMessageIStream(unsigned byteOrder, T value,
        D& dbusType)
    {
        T writeValue = byteOrder == __BYTE_ORDER ? value : swap(value);
        std::basic_string<uint8_t> data;
        data.append((uint8_t*)&writeValue, sizeof(T));

        MessageIStream istream(data.data(), data.size(), byteOrder != __BYTE_ORDER);
        TestUnmarshallFromMessageIStream<T, D>(value, dbusType, istream);
    }

} // namespace test
} // namespace DBus
