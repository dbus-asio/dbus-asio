#include "test_unmarshall.h"
#include "dbus_type_int32.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"

namespace DBus { namespace test {

TEST_CASE("Unmarshall int32 little endian from MessageIStream")
{
    Type::Int32 dbusType;
    const int32_t value = -257978445;
    TestUnmarshallFromMessageIStream<int32_t, Type::Int32>(__LITTLE_ENDIAN, value, dbusType);
}

TEST_CASE("Unmarshall int32 big endian from MessageIStream")
{
    Type::Int32 dbusType;
    const int32_t value = -257978445;
    TestUnmarshallFromMessageIStream<int32_t, Type::Int32>(__BIG_ENDIAN, value, dbusType);
}

TEST_CASE("Marshall and unmarshall int32")
{
    MessageOStream stream;
    int32_t value = -257978445;
    stream.writeInt32(value);

    Type::Int32 dbusType;
    MessageIStream istream((uint8_t*)stream.data.data(), stream.data.size(), false);
    TestUnmarshallFromMessageIStream<int32_t, Type::Int32>(value, dbusType, istream);
}


}} // namespace DBus::test

using namespace DBus::test;
