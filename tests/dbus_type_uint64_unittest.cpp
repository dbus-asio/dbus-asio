#include "test_unmarshall.h"
#include "dbus_type_uint64.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"

namespace DBus { namespace test {

TEST_CASE("Unmarshall uint64 little endian")
{
    Type::Uint64 dbusType;
    const uint64_t value = 98987654549204454LL;
    TestUnmarshall<uint64_t, Type::Uint64>(__LITTLE_ENDIAN, value, dbusType);
}

TEST_CASE("Unmarshall uint64 big endian")
{
    Type::Uint64 dbusType;
    dbusType.setLittleEndian(false);
    const uint64_t value = 98987654549204454LL;
    TestUnmarshall<uint64_t, Type::Uint64>(__BIG_ENDIAN, value, dbusType);
}

TEST_CASE("Marshall and unmarshall uint64")
{
    MessageOStream stream;
    uint64_t value = 98987654549204454LL;
    stream.writeInt64(value);

    Type::Uint64 dbusType;
    TestUnmarshallFromStream<uint64_t, Type::Uint64>(value, dbusType, stream);
}


}} // namespace DBus::test

using namespace DBus::test;
