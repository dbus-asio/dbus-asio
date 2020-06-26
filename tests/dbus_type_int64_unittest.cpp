#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include "dbus_type_int64.h"
#include "test_unmarshall.h"

namespace DBus {
namespace test {

    TEST_CASE("Unmarshall int64 little endian from MessageIStream")
    {
        Type::Int64 dbusType;
        const int64_t value = 98987654549204454LL;
        TestUnmarshallFromMessageIStream<int64_t, Type::Int64>(__LITTLE_ENDIAN, value,
            dbusType);
    }

    TEST_CASE("Unmarshall int64 big endian from MessageIStream")
    {
        Type::Int64 dbusType;
        const int64_t value = 98987654549204454LL;
        TestUnmarshallFromMessageIStream<int64_t, Type::Int64>(__BIG_ENDIAN, value,
            dbusType);
    }

    TEST_CASE("Marshall and unmarshall int 64")
    {
        MessageOStream stream;
        int64_t value = 98987654549204454LL;
        stream.writeInt64(value);

        Type::Int64 dbusType;
        MessageIStream istream((uint8_t*)stream.data.data(), stream.data.size(),
            false);
        TestUnmarshallFromMessageIStream<int64_t, Type::Int64>(value, dbusType,
            istream);
    }

} // namespace test
} // namespace DBus

using namespace DBus::test;
