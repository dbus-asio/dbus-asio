#include "test_unmarshall.h"
#include "dbus_type_int32.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"

namespace DBus { namespace test {

TEST_CASE("Unmarshall int32 little endian")
{
    Type::Int32 dbusType;
    const int32_t value = -257978445;
    TestUnmarshall<int32_t, Type::Int32>(__LITTLE_ENDIAN, value, dbusType);
}

TEST_CASE("Unmarshall int32 big endian")
{
    Type::Int32 dbusType;
    dbusType.setLittleEndian(false);
    const int32_t value = -257978445;
    TestUnmarshall<int32_t, Type::Int32>(__BIG_ENDIAN, value, dbusType);
}

TEST_CASE("Marshall and unmarshall int32")
{
    MessageStream stream;
    int32_t value = -257978445;
    stream.writeInt32(value);

    Type::Int32 dbusType;
    TestUnmarshallFromStream<int32_t, Type::Int32>(value, dbusType, stream);
}


}} // namespace DBus::test

using namespace DBus::test;
