#include "dbus_octetbuffer.h"
#include <catch2/catch.hpp>
#include <string.h>

namespace DBus {
namespace test {

    TEST_CASE("Construction")
    {
        const std::string data("Hello world");
        OctetBuffer buffer((const uint8_t*)data.data(), data.size());

        REQUIRE(buffer.size() == data.size());
        REQUIRE(memcmp(data.data(), buffer.data(), buffer.size()) == 0);
    }

    TEST_CASE("Remove prefix")
    {
        const std::string data("Hello world");
        OctetBuffer buffer((const uint8_t*)data.data(), data.size());

        buffer.remove_prefix(6);
        REQUIRE(memcmp("world", buffer.data(), buffer.size()) == 0);

        CHECK_THROWS(buffer.remove_prefix(6));
    }

    TEST_CASE("Empty")
    {
        const std::string data("Test");
        OctetBuffer buffer((const uint8_t*)data.data(), data.size());

        REQUIRE(false == buffer.empty());
        buffer.remove_prefix(buffer.size());
        REQUIRE(true == buffer.empty());
    }

    TEST_CASE("[] operator")
    {
        const std::string data("123");
        OctetBuffer buffer((const uint8_t*)data.data(), data.size());
        REQUIRE('1' == buffer[0]);
        REQUIRE('2' == buffer[1]);
        REQUIRE('3' == buffer[2]);
        CHECK_THROWS(buffer[4]);
    }

    TEST_CASE("Copy")
    {
        const std::string data("Test Data");
        OctetBuffer buffer((const uint8_t*)data.data(), data.size());

        uint8_t copyData[5];
        buffer.copy(copyData, 5);
        REQUIRE(memcmp(copyData, data.data(), 5) == 0);
        CHECK_THROWS(buffer.copy(copyData, 255));
    }

    TEST_CASE("Find")
    {
        const std::string data("Test Data");
        OctetBuffer buffer((const uint8_t*)data.data(), data.size());

        REQUIRE(buffer.find('D') == 5);
        REQUIRE(buffer.find('z') == std::string::npos);
    }

} // namespace test
} // namespace DBus

using namespace DBus::test;
