#include "test_unmarshall.h"
#include "dbus_type_uint32.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"

namespace DBus { namespace test {

TEST_CASE("Unmarshall uint32 little endian")
{
    Type::Uint32 dbusType;
    const uint32_t value = 257978445;
    TestUnmarshall<uint32_t, Type::Uint32>(__LITTLE_ENDIAN, value, dbusType);
}

TEST_CASE("Unmarshall uint32 big endian")
{
    Type::Uint32 dbusType;
    dbusType.setLittleEndian(false);
    const uint32_t value = 257978445;
    TestUnmarshall<uint32_t, Type::Uint32>(__BIG_ENDIAN, value, dbusType);
}

TEST_CASE("Marshall and unmarshall uint32")
{
    MessageStream stream;
    uint32_t value = -257978445;
    stream.writeInt32(value);

    Type::Uint32 dbusType;
    TestUnmarshallFromStream<uint32_t, Type::Uint32>(value, dbusType, stream);
}

}} // namespace DBus::test

using namespace DBus::test;
