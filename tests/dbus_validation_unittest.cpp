#include "dbus_validation.h"
#include <catch2/catch.hpp>

TEST_CASE("Basic types char")
{
    using namespace DBus::Validation;

    CHECK_NOTHROW(throwOnInvalidBasicType('y'));
    CHECK_NOTHROW(throwOnInvalidBasicType('b'));
    CHECK_NOTHROW(throwOnInvalidBasicType('n'));
    CHECK_NOTHROW(throwOnInvalidBasicType('q'));
    CHECK_NOTHROW(throwOnInvalidBasicType('u'));
    CHECK_NOTHROW(throwOnInvalidBasicType('x'));
    CHECK_NOTHROW(throwOnInvalidBasicType('t'));
    CHECK_NOTHROW(throwOnInvalidBasicType('d'));
    CHECK_NOTHROW(throwOnInvalidBasicType('h'));
    CHECK_NOTHROW(throwOnInvalidBasicType('s'));
    CHECK_NOTHROW(throwOnInvalidBasicType('o'));
    CHECK_NOTHROW(throwOnInvalidBasicType('g'));

    CHECK_THROWS(throwOnInvalidBasicType('a'));
    CHECK_THROWS(throwOnInvalidBasicType('v'));
    CHECK_THROWS(throwOnInvalidBasicType('('));
    CHECK_THROWS(throwOnInvalidBasicType('{'));
    CHECK_THROWS(throwOnInvalidBasicType('m'));
    CHECK_THROWS(throwOnInvalidBasicType('*'));
    CHECK_THROWS(throwOnInvalidBasicType('?'));
    CHECK_THROWS(throwOnInvalidBasicType('@'));
    CHECK_THROWS(throwOnInvalidBasicType('&'));
    CHECK_THROWS(throwOnInvalidBasicType('^'));
}

TEST_CASE("Basic types string")
{
    using namespace DBus::Validation;

    CHECK_NOTHROW(throwOnInvalidBasicType("y"));
    CHECK_NOTHROW(throwOnInvalidBasicType("g"));

    CHECK_THROWS(throwOnInvalidBasicType(""));
    CHECK_THROWS(throwOnInvalidBasicType("ai"));
    CHECK_THROWS(throwOnInvalidBasicType("v"));
    CHECK_THROWS(throwOnInvalidBasicType("(ii)"));
    CHECK_THROWS(throwOnInvalidBasicType("{su}"));
}
