#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include "dbus_type_uint64.h"
#include "test_unmarshall.h"

namespace DBus {
namespace test {

    TEST_CASE("Unmarshall uint64 little endian from MessageIStream")
    {
        Type::Uint64 dbusType;
        const uint64_t value = 98987654549204454LL;
        TestUnmarshallFromMessageIStream<uint64_t, Type::Uint64>(__LITTLE_ENDIAN,
            value, dbusType);
    }

    TEST_CASE("Unmarshall uint64 big endian from MessageIStream")
    {
        Type::Uint64 dbusType;
        const uint64_t value = 98987654549204454LL;
        TestUnmarshallFromMessageIStream<uint64_t, Type::Uint64>(__BIG_ENDIAN, value,
            dbusType);
    }

    TEST_CASE("Marshall and unmarshall uint64")
    {
        MessageOStream stream;
        uint64_t value = 98987654549204454LL;
        stream.writeInt64(value);

        Type::Uint64 dbusType;
        MessageIStream istream((uint8_t*)stream.data.data(), stream.data.size(),
            false);
        TestUnmarshallFromMessageIStream<uint64_t, Type::Uint64>(value, dbusType,
            istream);
    }

} // namespace test
} // namespace DBus

using namespace DBus::test;
