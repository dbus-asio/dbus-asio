#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include "dbus_type_uint32.h"
#include "test_unmarshall.h"

namespace DBus {
namespace test {

    TEST_CASE("Unmarshall uint32 little endian from MessageIStream")
    {
        Type::Uint32 dbusType;
        const uint32_t value = 257978445;
        TestUnmarshallFromMessageIStream<uint32_t, Type::Uint32>(__LITTLE_ENDIAN,
            value, dbusType);
    }

    TEST_CASE("Unmarshall uint32 big endian from MessageIStream")
    {
        Type::Uint32 dbusType;
        const uint32_t value = 257978445;
        TestUnmarshallFromMessageIStream<uint32_t, Type::Uint32>(__BIG_ENDIAN, value,
            dbusType);
    }

    TEST_CASE("Marshall and unmarshall uint32")
    {
        MessageOStream stream;
        uint32_t value = -257978445;
        stream.writeInt32(value);

        Type::Uint32 dbusType;
        MessageIStream istream((uint8_t*)stream.data.data(), stream.data.size(),
            false);
        TestUnmarshallFromMessageIStream<uint32_t, Type::Uint32>(value, dbusType,
            istream);
    }

} // namespace test
} // namespace DBus

using namespace DBus::test;
