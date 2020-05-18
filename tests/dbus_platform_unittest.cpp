#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include "dbus_platform.h"
#include <stdlib.h>

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
}

TEST_CASE("get unix IDs")
{
    REQUIRE(DBus::Platform::getPID() == getpid());
    REQUIRE(DBus::Platform::getUID() == getuid());
}

}}} // namespace DBus::Platform::test

using namespace DBus::Platform::test;
