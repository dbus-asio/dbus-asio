#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include "dbus_type_uint16.h"
#include "test_unmarshall.h"

namespace DBus {
namespace test {

    TEST_CASE("Unmarshall uint16 little endian from MessageIStream")
    {
        Type::Uint16 dbusType;
        const uint16_t value = 12345;
        TestUnmarshallFromMessageIStream<uint16_t, Type::Uint16>(__LITTLE_ENDIAN,
            value, dbusType);
    }

    TEST_CASE("Unmarshall uint16 big endian from MessageIStream")
    {
        Type::Uint16 dbusType;
        const uint16_t value = 45678;
        TestUnmarshallFromMessageIStream<uint16_t, Type::Uint16>(__BIG_ENDIAN, value,
            dbusType);
    }

    TEST_CASE("Marshall and unmarshall uint16")
    {
        MessageOStream stream;
        uint16_t value = 45678;
        stream.writeInt16(value);

        Type::Uint16 dbusType;
        MessageIStream istream((uint8_t*)stream.data.data(), stream.data.size(),
            false);
        TestUnmarshallFromMessageIStream<uint16_t, Type::Uint16>(value, dbusType,
            istream);
    }

} // namespace test
} // namespace DBus

using namespace DBus::test;
