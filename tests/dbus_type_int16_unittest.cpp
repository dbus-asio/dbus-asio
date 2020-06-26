#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include "dbus_type_int16.h"
#include "test_unmarshall.h"

namespace DBus {
namespace test {

    TEST_CASE("Unmarshall int16 little endian from MessageIStream")
    {
        Type::Int16 dbusType;
        const int16_t value = 12345;
        TestUnmarshallFromMessageIStream<int16_t, Type::Int16>(__LITTLE_ENDIAN, value,
            dbusType);
    }

    TEST_CASE("Unmarshall int16 big endian from MessageIStream")
    {
        Type::Int16 dbusType;
        const int16_t value = 12345;
        TestUnmarshallFromMessageIStream<int16_t, Type::Int16>(__BIG_ENDIAN, value,
            dbusType);
    }

    TEST_CASE("Marshall and unmarshall int16")
    {
        MessageOStream stream;
        int16_t value = -13456;
        stream.writeInt16(value);

        Type::Int16 dbusType;
        MessageIStream istream((uint8_t*)stream.data.data(), stream.data.size(),
            false);
        TestUnmarshallFromMessageIStream<int16_t, Type::Int16>(value, dbusType,
            istream);
    }

} // namespace test
} // namespace DBus

using namespace DBus::test;
