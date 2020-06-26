#include "dbus_messageistream.h"
#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"
#include "dbus_type_array.h"
#include "dbus_type_boolean.h"
#include "dbus_type_byte.h"
#include "dbus_type_dictentry.h"
#include "dbus_type_int32.h"
#include "dbus_type_string.h"
#include "dbus_type_struct.h"
#include "dbus_type_uint32.h"
#include <byteswap.h>
#include <catch2/catch.hpp>
#include <iomanip>
#include <iostream>

namespace DBus::test {

// This only tests that we can receive what we send. It doesn't prove
// that we'll interoperate with any other implementations.
TEST_CASE("Round trip complex data structure")
{
    Type::Struct dbusStruct1;
    dbusStruct1.add(Type::Byte(85));
    dbusStruct1.add(Type::String("BrightSign"));

    Type::Struct dbusStruct2;
    dbusStruct2.add(Type::Byte(42));
    dbusStruct2.add(Type::String("Arthur"));

    Type::Struct dbusStruct3;
    dbusStruct3.add(Type::Byte(99));
    dbusStruct3.add(Type::String("Red Balloons"));

    Type::Array dbusArray;
    dbusArray.add(Type::DictEntry(Type::String("KeyKey"), dbusStruct1));
    dbusArray.add(Type::DictEntry(Type::String("Another key"), dbusStruct2));
    dbusArray.add(Type::DictEntry(Type::String("Anarchy"), dbusStruct3));

    Type::Struct dbusStructTop;
    dbusStructTop.add(Type::String("This is a long string value"));
    dbusStructTop.add(Type::DictEntry(Type::String("Key"), dbusArray));

    Type::DictEntry input(Type::Int32(131073), dbusStructTop);

    REQUIRE(DBus::Type::getMarshallingSignature(input) == "{i(s{sa{s(ys)}})}");
    REQUIRE('\n' + input.toString() ==
        R"(
DictEntry ({i(s{sa{s(ys)}})}) : {
   key:      Int32 131073 (0x20001)
   value:    Struct (s{sa{s(ys)}}) <
      String (27) "This is a long string value"
      DictEntry ({sa{s(ys)}}) : {
         key:            String (3) "Key"
         value:          Array (a{s(ys)}) [
            [0] =
               DictEntry ({s(ys)}) : {
                  key:                     String (6) "KeyKey"
                  value:                   Struct (ys) <
                     Byte 85 (0x55)
                     String (10) "BrightSign"
                  >
               }
            [1] =
               DictEntry ({s(ys)}) : {
                  key:                     String (11) "Another key"
                  value:                   Struct (ys) <
                     Byte 42 (0x2a)
                     String (6) "Arthur"
                  >
               }
            [2] =
               DictEntry ({s(ys)}) : {
                  key:                     String (7) "Anarchy"
                  value:                   Struct (ys) <
                     Byte 99 (0x63)
                     String (12) "Red Balloons"
                  >
               }
         ]
      }
   >
}
)");

    MessageOStream ostream;
    input.marshall(ostream);

    REQUIRE(ostream.data.size() == 165);

    SECTION("Complete")
    {
        Type::DictEntry output;
        output.setSignature(input.getSignature());
        MessageIStream istream((uint8_t*)ostream.data.data(), ostream.data.size(),
            false);
        output.unmarshall(istream);

        REQUIRE(input.toString() == output.toString());
    }

    SECTION("Truncated input")
    {
        // We don't really care what exception is thrown as long as
        // one is and valgrind doesn't complain that we went outside
        // the bounds.
        for (const auto truncate_at : { 164, 42, 1, 0 }) {
            SECTION("Truncated")
            {
                INFO("Truncate at " << truncate_at << " from " << ostream.data.size());
                std::string data = ostream.data.substr(0, truncate_at);
                Type::DictEntry output;
                output.setSignature(input.getSignature());

                MessageIStream istream((uint8_t*)data.data(), data.size(), false);
                REQUIRE_THROWS(output.unmarshall(istream));
            }
        }
    }
}

} // namespace DBus::test
