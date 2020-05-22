#include "test_unmarshall.h"
#include "dbus_type_int64.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"

namespace DBus { namespace test {

TEST_CASE("Unmarshall int64 little endian")
{
    Type::Int64 dbusType;
    const int64_t value = 98987654549204454LL;
    TestUnmarshall<int64_t, Type::Int64>(__LITTLE_ENDIAN, value, dbusType);
}

TEST_CASE("Unmarshall int64 big endian")
{
    Type::Int64 dbusType;
    dbusType.setLittleEndian(false);
    const int64_t value = 98987654549204454LL;
    TestUnmarshall<int64_t, Type::Int64>(__BIG_ENDIAN, value, dbusType);
}

TEST_CASE("Marshall and unmarshall int 64")
{
    MessageOStream stream;
    int64_t value = 98987654549204454LL;
    stream.writeInt64(value);

    Type::Int64 dbusType;
    TestUnmarshallFromStream<int64_t, Type::Int64>(value, dbusType, stream);
}


}} // namespace DBus::test

using namespace DBus::test;
