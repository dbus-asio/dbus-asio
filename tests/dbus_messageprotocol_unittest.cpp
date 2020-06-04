#include <catch2/catch.hpp>
#include "dbus_messageprotocol.h"
#include "dbus_transport.h"

namespace DBus { namespace test {

constexpr static uint8_t vector1[] = {0x6C,0x02,0x01,0x01,0x0B,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x3D,0x00,0x00,0x00,0x06,0x01,0x73,0x00,0x06,0x00,0x00,0x00,0x3A,0x31,0x2E,0x33,0x30,0x37,0x00,0x00,0x05,0x01,0x75,0x00,0x01,0x00,0x00,0x00,0x08,0x01,0x67,0x00,0x01,0x73,0x00,0x00,0x07,0x01,0x73,0x00,0x14,0x00,0x00,0x00,0x6F,0x72,0x67,0x2E,0x66,0x72,0x65,0x65,0x64,0x65,0x73,0x6B,0x74,0x6F,0x70,0x2E,0x44,0x42,0x75,0x73,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x3A,0x31,0x2E,0x33,0x30,0x37,0x00};
constexpr static uint8_t vector2[] = {0x6C,0x02,0x01,0x01,0x0B,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x3D,0x00,0x00,0x00,0x06,0x01,0x73,0x00,0x06,0x00,0x00,0x00,0x3A,0x31,0x2E,0x13,0x30,0x37,0x00,0x00,0x05,0x01,0x75,0x00,0x01,0x00,0x00,0x00,0x08,0x01,0x67,0x00,0x01,0x73,0x00,0x00,0x07,0x01,0x73,0x00,0x14,0x00,0x00,0x00,0x6F,0x72,0x67,0x2E,0x66,0x72,0x65,0x65,0x64,0x65,0x73,0x6B,0x74,0x6F,0x70,0x2E,0x44,0x42,0x75,0x73,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x3A,0x31,0x2E,0x33,0x30,0x37,0x00,
                                      0x6C,0x02,0x01,0x01,0x0B,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x3D,0x00,0x00,0x00,0x06,0x01,0x73,0x00,0x06,0x00,0x00,0x00,0x3A,0x31,0x2E,0x33,0x30,0x37,0x00,0x00,0x05,0x01,0x75,0x00,0x01,0x00,0x00,0x00,0x08,0x01,0x67,0x00,0x01,0x73,0x00,0x00,0x07,0x01,0x73,0x00,0x14,0x00,0x00,0x00,0x6F,0x72,0x67,0x2E,0x66,0x72,0x65,0x65,0x64,0x65,0x73,0x6B,0x74,0x6F,0x70,0x2E,0x44,0x42,0x75,0x73,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x3A,0x31,0x2E,0x33,0x30,0x37,0x00};
constexpr static uint8_t vector3[] = {0x42,0x02,0x01,0x01,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x3D,0x06,0x01,0x73,0x00,0x00,0x00,0x00,0x06,0x3A,0x31,0x2E,0x33,0x30,0x37,0x00,0x00,0x05,0x01,0x75,0x00,0x00,0x00,0x00,0x01,0x08,0x01,0x67,0x00,0x01,0x73,0x00,0x00,0x07,0x01,0x73,0x00,0x00,0x00,0x00,0x14,0x6F,0x72,0x67,0x2E,0x66,0x72,0x65,0x65,0x64,0x65,0x73,0x6B,0x74,0x6F,0x70,0x2E,0x44,0x42,0x75,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x3A,0x31,0x2E,0x33,0x30,0x37,0x00};
constexpr static uint8_t arrayToBig[]   = {0x6C,0x02,0x01,0x01,0x0B,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x06,0x01,0x73,0x00,0x06,0x00,0x00,0x00,0x3A,0x31,0x2E,0x33,0x30,0x37,0x00,0x00,0x05,0x01,0x75,0x00,0x01,0x00,0x00,0x00,0x08,0x01,0x67,0x00,0x01,0x73,0x00,0x00,0x07,0x01,0x73,0x00,0x14,0x00,0x00,0x00,0x6F,0x72,0x67,0x2E,0x66,0x72,0x65,0x65,0x64,0x65,0x73,0x6B,0x74,0x6F,0x70,0x2E,0x44,0x42,0x75,0x73,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x3A,0x31,0x2E,0x33,0x30,0x37,0x00};
constexpr static uint8_t messageToBig[] = {0x6C,0x02,0x01,0x01,0x00,0x00,0x00,0x20,0x01,0x00,0x00,0x00,0x3D,0x00,0x00,0x00,0x06,0x01,0x73,0x00,0x06,0x00,0x00,0x00,0x3A,0x31,0x2E,0x33,0x30,0x37,0x00,0x00,0x05,0x01,0x75,0x00,0x01,0x00,0x00,0x00,0x08,0x01,0x67,0x00,0x01,0x73,0x00,0x00,0x07,0x01,0x73,0x00,0x14,0x00,0x00,0x00,0x6F,0x72,0x67,0x2E,0x66,0x72,0x65,0x65,0x64,0x65,0x73,0x6B,0x74,0x6F,0x70,0x2E,0x44,0x42,0x75,0x73,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x3A,0x31,0x2E,0x33,0x30,0x37,0x00};
constexpr static uint8_t headerSizeCheck[] = {0x6c,0x04,0x01,0x01,0x2e,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x89,0x00,0x00,0x00,0x01,0x01,0x6f,0x00,0x15,0x00,0x00,0x00,0x2f,0x6f,0x72,0x67,0x2f,0x66,0x72,0x65,0x65,0x64,0x65,0x73,0x6b,0x74,0x6f,0x70,0x2f,0x44,0x42,0x75,0x73,0x00,0x00,0x00,0x02,0x01,0x73,0x00,0x14,0x00,0x00,0x00,0x6f,0x72,0x67,0x2e,0x66,0x72,0x65,0x65,0x64,0x65,0x73,0x6b,0x74,0x6f,0x70,0x2e,0x44,0x42,0x75,0x73,0x00,0x00,0x00,0x00,0x03,0x01,0x73,0x00,0x10,0x00,0x00,0x00,0x4e,0x61,0x6d,0x65,0x4f,0x77,0x6e,0x65,0x72,0x43,0x68,0x61,0x6e,0x67,0x65,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x01,0x73,0x00,0x14,0x00,0x00,0x00,0x6f,0x72,0x67,0x2e,0x66,0x72,0x65,0x65,0x64,0x65,0x73,0x6b,0x74,0x6f,0x70,0x2e,0x44,0x42,0x75,0x73,0x00,0x00,0x00,0x00,0x08,0x01,0x67,0x00,0x03,0x73,0x73,0x73};
constexpr static uint8_t emptyBody[] = {0x6C,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x35,0x00,0x00,0x00,0x06,0x01,0x73,0x00,0x05,0x00,0x00,0x00,0x3A,0x31,0x2E,0x33,0x30,0x00,0x00,0x00,0x05,0x01,0x75,0x00,0x02,0x00,0x00,0x00,0x07,0x01,0x73,0x00,0x14,0x00,0x00,0x00,0x6F,0x72,0x67,0x2E,0x66,0x72,0x65,0x65,0x64,0x65,0x73,0x6B,0x74,0x6F,0x70,0x2E,0x44,0x42,0x75,0x73,0x00,0x00,0x00,0x00};

TEST_CASE("Process Test Vector1 in a single pass")
{
    bool resultReturned = false;
    MessageProtocol protocol;
    protocol.setMethodReturnHandler([&resultReturned](const Message::MethodReturn& method){
        std::string result = DBus::Type::asString(method.getParameter(0));
        REQUIRE(result == ":1.307");
        resultReturned = true;
    });

    OctetBuffer buffer(vector1, sizeof(vector1));
    protocol.onReceiveData(buffer);
    REQUIRE(resultReturned);
}

TEST_CASE("Process Test Vector1 byte by byte")
{
    bool resultReturned = false;
    MessageProtocol protocol;
    protocol.setMethodReturnHandler([&resultReturned](const Message::MethodReturn& method){
        std::string result = DBus::Type::asString(method.getParameter(0));
        REQUIRE(result == ":1.307");
        resultReturned = true;
    });

    for (auto i = 0; i < sizeof(vector1); ++i) {
        OctetBuffer buffer(vector1 + i, 1);
        protocol.onReceiveData(buffer);
    }
    REQUIRE(resultReturned);
}

TEST_CASE("Process Test Vector2")
{
    unsigned results = 0;
    MessageProtocol protocol;
    protocol.setMethodReturnHandler([&results](const Message::MethodReturn& method){
        std::string result = DBus::Type::asString(method.getParameter(0));
        REQUIRE(result == ":1.307");
        ++results;
    });

    OctetBuffer buffer(vector2, sizeof(vector2));
    protocol.onReceiveData(buffer);
    REQUIRE(results == 2);
}

TEST_CASE("Process Test Vector3 - big endian version of vector 1")
{
    bool resultReturned = false;
    MessageProtocol protocol;
    protocol.setMethodReturnHandler([&resultReturned](const Message::MethodReturn& method){
        std::string result = DBus::Type::asString(method.getParameter(0));
        REQUIRE(result == ":1.307");
        resultReturned = true;
    });

    OctetBuffer buffer(vector3, sizeof(vector3));
    protocol.onReceiveData(buffer);
    REQUIRE(resultReturned);
}

TEST_CASE("Message field info array too big")
{
    MessageProtocol protocol;
    OctetBuffer buffer(arrayToBig, sizeof(arrayToBig));
    CHECK_THROWS(protocol.onReceiveData(buffer));
 }

TEST_CASE("Message data too big")
{
    MessageProtocol protocol;
    OctetBuffer buffer(messageToBig, sizeof(arrayToBig));
    CHECK_THROWS(protocol.onReceiveData(buffer));
}

TEST_CASE("Empty body")
{
    bool resultReturned = false;
    MessageProtocol protocol;
    protocol.setMethodReturnHandler([&resultReturned](const Message::MethodReturn& method){
        REQUIRE(method.getParameterCount() == 0);
        resultReturned = true;
    });

    OctetBuffer buffer(emptyBody, sizeof(emptyBody));
    protocol.onReceiveData(buffer);
    REQUIRE(resultReturned);
}

TEST_CASE("Header size is correct")
{
    // This will throw if the header size calculation is incorrect
    MessageProtocol protocol;
    OctetBuffer buffer(headerSizeCheck, sizeof(headerSizeCheck));
    protocol.onReceiveData(buffer);
}

}} // namespace DBus::test

using namespace DBus::test;
