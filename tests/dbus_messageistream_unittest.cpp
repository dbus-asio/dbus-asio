#include <catch2/catch.hpp>
#include "dbus_messageistream.h"
#include <string.h>

namespace DBus { namespace test {

template<class T>
T swap(T value)
{
    switch(sizeof(T)) {
        case 2:
            return bswap_16(value); break;
        case 4:
            return bswap_32(value); break;
        case 8:
            return bswap_64(value); break;
        default:
            break;
    }
    return value;
}

template<class T>
void testAlignedRead(T value)
{
    std::vector<uint8_t> data(2 * sizeof(T), 0);

    for (auto i = 0; i < sizeof(T); i++) {
        memcpy(&data[i > 0 ? sizeof(T) : 0], &value, sizeof(T));

        MessageIStream leStream(data.data(), data.size(), false);
        MessageIStream beStream(data.data(), data.size(), true);

        std::string str;
        leStream.read(str, i);
        beStream.read(str, i);

        T readValue = 0;
        leStream.read<T>(&readValue);
        REQUIRE(readValue == value);

        readValue = 0;
        beStream.read<T>(&readValue);
        REQUIRE(readValue == swap(value));
    }
}

TEST_CASE("Read byte")
{
    const uint8_t data = 34;
    MessageIStream stream(&data, 1, false);

    REQUIRE(stream.read() == 34);
    CHECK_THROWS(stream.read());
}

TEST_CASE("Read data")
{
    const uint8_t data[4] = {34, 35, 36, 37};
    uint8_t readData[3];
    MessageIStream stream(data, 4, false);

    stream.read(readData, 3);
    REQUIRE(memcmp(data, readData, 3) == 0);
    CHECK_THROWS(stream.read(readData, 3));
}

TEST_CASE("Read double")
{
    double value = 0.75483;
    MessageIStream leStream((const uint8_t*)&value, sizeof(double), false);

    uint64_t swapped_value = __bswap_64(*reinterpret_cast<uint64_t *>(&value));
    MessageIStream beStream((const uint8_t*)&swapped_value, sizeof(double), true);

    double readValue = 0.0;
    leStream.read(&readValue);
    REQUIRE(readValue == value);

    readValue = 0.0;
    beStream.read(&readValue);
    REQUIRE(readValue == value);

    CHECK_THROWS(leStream.read(&readValue));
}

TEST_CASE("Read string")
{
    std::string value("Hello world");
    MessageIStream stream((const uint8_t*)value.data(), value.size(), false);

    std::string readValue = "Test";
    stream.read(readValue, value.size());
    REQUIRE(readValue == value);

    CHECK_THROWS(stream.read(readValue, 1));
}

TEST_CASE("Is empty")
{
    std::string value("Hello world");
    MessageIStream stream((const uint8_t*)value.data(), value.size(), false);

    REQUIRE(stream.empty() == false);

    std::string readValue;
    stream.read(readValue, value.size());

    REQUIRE(stream.empty() == true);
}

TEST_CASE("Read 2 Byte aligned values")
{
    int16_t iValue = 0x1BCD;
    uint16_t uValue = 0xFBCD;

    testAlignedRead<int16_t>(iValue);
    testAlignedRead<uint16_t>(uValue);
}


TEST_CASE("Read 4 Byte aligned values")
{
    int32_t iValue = 0x1BCD6734;
    uint32_t uValue = 0xFBCD3497;

    testAlignedRead<int32_t>(iValue);
    testAlignedRead<uint32_t>(uValue);
}

TEST_CASE("Read 8 Byte aligned values")
{
    int64_t iValue = 0x1BCD787612349876;
    uint64_t uValue = 0xFBCD787612349876;

    testAlignedRead<int64_t>(iValue);
    testAlignedRead<uint64_t>(uValue);
}




}} // namespace DBus::test

using namespace DBus::test;
