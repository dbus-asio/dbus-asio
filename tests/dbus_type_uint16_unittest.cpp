#include "test_unmarshall.h"
#include "dbus_type_uint16.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"

namespace DBus { namespace test {

TEST_CASE("Unmarshall uint16 little endian")
{
    Type::Uint16 dbusType;
    const uint16_t value = 12345;
    TestUnmarshall<uint16_t, Type::Uint16>(__LITTLE_ENDIAN, value, dbusType);
}

TEST_CASE("Unmarshall uint16 big endian")
{
    Type::Uint16 dbusType;
    dbusType.setLittleEndian(false);
    const uint16_t value = 45678;
    TestUnmarshall<uint16_t, Type::Uint16>(__BIG_ENDIAN, value, dbusType);
}

TEST_CASE("Marshall and unmarshall uint16")
{
    MessageStream stream;
    uint16_t value = 45678;
    stream.writeInt16(value);

    Type::Uint16 dbusType;
    TestUnmarshallFromStream<uint16_t, Type::Uint16>(value, dbusType, stream);
}

}} // namespace DBus::test

using namespace DBus::test;
