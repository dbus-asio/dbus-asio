
#include <catch2/catch.hpp>
#include "dbus_platform.h"
#include <stdlib.h>
#include <unistd.h>

namespace DBus { namespace Platform { namespace test {

TEST_CASE("get session bus")
{
    SECTION( "DBUS_SESSION_BUS_ADDRESS is null" )
    {
        REQUIRE(getSystemBus(nullptr) == "/var/run/dbus/system_bus_socket");
    }

    SECTION( "DBUS_SESSION_BUS_ADDRESS is empty" )
    {
        REQUIRE(getSystemBus("") == "");
    }

    SECTION( "DBUS_SESSION_BUS_ADDRESS is not prefixed with a unix:path" )
    {
        REQUIRE(getSystemBus("/socket/path") == "/socket/path");
    }

    SECTION( "DBUS_SESSION_BUS_ADDRESS is prefixed with a unix:path" )
    {
        REQUIRE(getSystemBus("unix:path=/socket/path") == "/socket/path");
    }

    SECTION( "DBUS_SESSION_BUS_ADDRESS is an abstract socket path" )
    {
        std::string expected_path;
        expected_path.push_back('\0');
        expected_path.append("/tmp/dbus-ZV5ypNeDH1");
        REQUIRE(getSystemBus("unix:abstract=/tmp/dbus-ZV5ypNeDH1,guid=d7e42c8f4eb686eb7711a78f5ec27480") == expected_path);
    }

    SECTION( "DBUS_SESSION_BUS_ADDRESS is an abstract socket path without a guid" )
    {
        std::string expected_path;
        expected_path.push_back('\0');
        expected_path.append("/tmp/dbus-ZV5ypNeDH1");
        REQUIRE(getSystemBus("unix:abstract=/tmp/dbus-ZV5ypNeDH1") == expected_path);
    }
}

TEST_CASE("get system bus")
{
    SECTION( "DBUS_SYSTEM_BUS_ADDRESS is null" )
    {
        REQUIRE(getSessionBus(nullptr) == "");
    }

    SECTION( "DBUS_SYSTEM_BUS_ADDRESS is empty" )
    {
        REQUIRE(getSessionBus("") == "");
    }

    SECTION( "DBUS_SYSTEM_BUS_ADDRESS is not prefixed with a unix:path" )
    {
        REQUIRE(getSessionBus("/socket/path") == "/socket/path");
    }

    SECTION( "DBUS_SYSTEM_BUS_ADDRESS is prefixed with a unix:path" )
    {
        REQUIRE(getSessionBus("unix:path=/socket/path") == "/socket/path");
    }

    SECTION( "DBUS_SYSTEM_BUS_ADDRESS is an abstract socket path" )
    {
        std::string expected_path;
        expected_path.push_back('\0');
        expected_path.append("/tmp/dbus-ZV5ypNeDH1");
        REQUIRE(getSessionBus("unix:abstract=/tmp/dbus-ZV5ypNeDH1,guid=d7e42c8f4eb686eb7711a78f5ec27480") == expected_path);
    }

    SECTION( "DBUS_SYSTEM_BUS_ADDRESS is an abstract socket path without a guid" )
    {
        std::string expected_path;
        expected_path.push_back('\0');
        expected_path.append("/tmp/dbus-ZV5ypNeDH1");
        REQUIRE(getSessionBus("unix:abstract=/tmp/dbus-ZV5ypNeDH1") == expected_path);
    }
}

TEST_CASE("get unix IDs")
{
    REQUIRE(DBus::Platform::getPID() == getpid());
    REQUIRE(DBus::Platform::getUID() == getuid());
}

}}} // namespace DBus::Platform::test

using namespace DBus::Platform::test;
