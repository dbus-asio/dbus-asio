#include "test_unmarshall.h"
#include "dbus_type_int16.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"

namespace DBus { namespace test {

TEST_CASE("Unmarshall int16 little endian")
{
    Type::Int16 dbusType;
    const int16_t value = 12345;
    TestUnmarshall<int16_t, Type::Int16>(__LITTLE_ENDIAN, value, dbusType);
}

TEST_CASE("Unmarshall int16 big endian")
{
    Type::Int16 dbusType;
    dbusType.setLittleEndian(false);
    const int16_t value = 12345;
    TestUnmarshall<int16_t, Type::Int16>(__BIG_ENDIAN, value, dbusType);
}

TEST_CASE("Marshall and unmarshall int16")
{
    MessageOStream stream;
    int16_t value = -13456;
    stream.writeInt16(value);

    Type::Int16 dbusType;
    TestUnmarshallFromStream<int16_t, Type::Int16>(value, dbusType, stream);
}


}} // namespace DBus::test

using namespace DBus::test;
