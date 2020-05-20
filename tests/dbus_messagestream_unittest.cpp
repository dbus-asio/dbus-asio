#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include "dbus_messagestream.h"

namespace DBus { namespace test {

TEST_CASE("Write byte")
{
    MessageStream stream;

    stream.writeByte((uint8_t)'A');
    REQUIRE(stream.data.size() == 1);
    REQUIRE(stream.data[0] == 'A');

    stream.writeByte((uint8_t)-12);
    REQUIRE(stream.data.size() == 2);
    REQUIRE(stream.data[1] == -12);
}

template <class T>
void testType(T value, MessageStream& stream, std::function<void(T)> write)
{
    for (auto i = 0; i < sizeof(T) + 1; ++i) {
        stream.data.clear();
        for (auto p = 0; p < i; ++p) {
            stream.writeByte('p');
        }

        auto expectedSize = i + sizeof(T);
        expectedSize+= (i % sizeof(T) == 0) ? 0 : sizeof(T) - (i % sizeof(T));
        write(value);
        REQUIRE(stream.data.size() == expectedSize);

        // Check padding
        for (auto p = i; p < expectedSize - sizeof(T); ++p) {
            REQUIRE(stream.data[p] == 0);
        }

        // Check value
        char *valuePtr = (char *)&value;
        for (auto p = expectedSize - sizeof(T); p < expectedSize; ++p, ++valuePtr) {
           REQUIRE(stream.data[p] == *valuePtr);
        }
    }
}

TEST_CASE("Write Int16")
{
    MessageStream stream;
    std::function<void(int16_t)> func = std::bind(&MessageStream::writeInt16, &stream, std::placeholders::_1);
    testType<int16_t>(0x9876, stream, func);
}
TEST_CASE("Write Uint16")
{
    MessageStream stream;
    std::function<void(uint16_t)> func = std::bind(&MessageStream::writeUint16, &stream, std::placeholders::_1);
    testType<uint16_t>(0x1234, stream, func);
}

TEST_CASE("Write Int32")
{
    MessageStream stream;
    std::function<void(int32_t)> func = std::bind(&MessageStream::writeInt32, &stream, std::placeholders::_1);
    testType<int32_t>(0x98765432, stream, func);
}
TEST_CASE("Write Uint32")
{
    MessageStream stream;
    std::function<void(uint32_t)> func = std::bind(&MessageStream::writeUint32, &stream, std::placeholders::_1);
    testType<uint32_t>(0x12345678, stream, func);
}

TEST_CASE("Write Int64")
{
    MessageStream stream;
    std::function<void(int64_t)> func = std::bind(&MessageStream::writeInt64, &stream, std::placeholders::_1);
    testType<int64_t>(0x987654322345678UL, stream, func);
}


TEST_CASE("Write Uint64")
{
    MessageStream stream;
    std::function<void(uint64_t)> func = std::bind(&MessageStream::writeUint64, &stream, std::placeholders::_1);
    testType<uint64_t>(0x1234567887654321UL, stream, func);
}

TEST_CASE("Write double")
{
    MessageStream stream;
    std::function<void(double)> func = std::bind(&MessageStream::writeDouble, &stream, std::placeholders::_1);
    testType<double>(13.1416346335, stream, func);
}

TEST_CASE("Write boolean")
{
    MessageStream stream;
    std::function<void(uint32_t)> func = std::bind(&MessageStream::writeBoolean, &stream, std::placeholders::_1);
    testType<uint32_t>(0, stream, func);
    testType<uint32_t>(1, stream, func);
}

TEST_CASE("Write string")
{
    MessageStream stream;
    std::string str("Text");
    stream.write(str);
    REQUIRE(stream.data == str);

    stream.write(str);
    REQUIRE(stream.data == str + str);
}

TEST_CASE("Write a message stream")
{
    MessageStream stream;
    std::string str("Text");
    stream.write(str);

    MessageStream stream2;
    stream2.write(stream);
    REQUIRE(stream2.data == str);

    stream.write(stream);
    REQUIRE(stream.data == str + str);
}

TEST_CASE("Write Dbus String")
{
    MessageStream stream;
    stream.writeByte(21);

    std::string str("Text");
    stream.writeString(str);

    REQUIRE(stream.data.size() == 13);
    REQUIRE(stream.data[0] == 21);
    REQUIRE(stream.data[1] == 0);
    REQUIRE(stream.data[2] == 0);
    REQUIRE(stream.data[3] == 0);
    REQUIRE(stream.data[4] == 4);
    REQUIRE(stream.data[5] == 0);
    REQUIRE(stream.data[6] == 0);
    REQUIRE(stream.data[7] == 0);
    REQUIRE(stream.data[8] == 'T');
    REQUIRE(stream.data[9] == 'e');
    REQUIRE(stream.data[10] == 'x');
    REQUIRE(stream.data[11] == 't');
    REQUIRE(stream.data[12] == 0);
}

TEST_CASE("Write Signature")
{
    MessageStream stream;
    stream.writeByte(21);

    std::string str("Text");
    stream.writeSignature(str);

    REQUIRE(stream.data.size() == 7);
    REQUIRE(stream.data[0] == 21);
    REQUIRE(stream.data[1] == 4);
    REQUIRE(stream.data[2] == 'T');
    REQUIRE(stream.data[3] == 'e');
    REQUIRE(stream.data[4] == 'x');
    REQUIRE(stream.data[5] == 't');
    REQUIRE(stream.data[6] == 0);
}

TEST_CASE("Padding")
{
    MessageStream stream;
    stream.writeByte(34);
    stream.pad2();
    REQUIRE(stream.size() == 2);
    REQUIRE(stream.data[0] ==34);
    REQUIRE(stream.data[1] == 0);

    stream.pad4();
    REQUIRE(stream.size() == 4);
    REQUIRE(stream.data[2] == 0);
    REQUIRE(stream.data[3] == 0);

    stream.pad8();
    REQUIRE(stream.size() == 8);
    REQUIRE(stream.data[4] == 0);
    REQUIRE(stream.data[5] == 0);
    REQUIRE(stream.data[6] == 0);
    REQUIRE(stream.data[7] == 0);

}

}} // namespace DBus::test

using namespace DBus::test;
