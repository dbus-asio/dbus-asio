#include "test_unmarshall.h"
#include "dbus_type_boolean.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"

namespace DBus { namespace test {

TEST_CASE("Unmarshall boolean little endian")
{
    Type::Boolean True;
    TestUnmarshall<uint32_t, Type::Boolean>(__LITTLE_ENDIAN, 1, True);

    Type::Boolean False;
    TestUnmarshall<uint32_t, Type::Boolean>(__LITTLE_ENDIAN, 0, False);
}

TEST_CASE("Unmarshall boolean big endian")
{
    Type::Boolean True;
    TestUnmarshall<uint32_t, Type::Boolean>(__BIG_ENDIAN, 1, True);

    Type::Boolean False;
    TestUnmarshall<uint32_t, Type::Boolean>(__BIG_ENDIAN, 0, False);
}

TEST_CASE("Marshall and unmarshall boolean")
{
    MessageStream stream;
    stream.writeInt32(true);

    Type::Boolean boolean;
    TestUnmarshallFromStream<uint32_t, Type::Boolean>(1, boolean, stream);
}

}} // namespace DBus::test

using namespace DBus::test;
