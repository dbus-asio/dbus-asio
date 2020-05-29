#include "test_unmarshall.h"
#include "dbus_type_boolean.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"

namespace DBus { namespace test {

TEST_CASE("Unmarshall boolean little endian from MessageIStream")
{
    Type::Boolean True;
    TestUnmarshallFromMessageIStream<uint32_t, Type::Boolean>(__LITTLE_ENDIAN, 1, True);

    Type::Boolean False;
    TestUnmarshallFromMessageIStream<uint32_t, Type::Boolean>(__LITTLE_ENDIAN, 0, False);
}

TEST_CASE("Unmarshall boolean big endian from MessageIStream")
{
    Type::Boolean True;
    TestUnmarshallFromMessageIStream<uint32_t, Type::Boolean>(__BIG_ENDIAN, 1, True);

    Type::Boolean False;
    TestUnmarshallFromMessageIStream<uint32_t, Type::Boolean>(__BIG_ENDIAN, 0, False);
}

TEST_CASE("Marshall and unmarshall boolean")
{
    MessageOStream stream;
    stream.writeInt32(true);

    Type::Boolean boolean;
    MessageIStream istream((uint8_t*)stream.data.data(), stream.data.size(), false);
    TestUnmarshallFromMessageIStream<uint32_t, Type::Boolean>(1, boolean, istream);
}

}} // namespace DBus::test

using namespace DBus::test;
