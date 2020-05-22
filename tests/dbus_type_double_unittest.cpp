#include "test_unmarshall.h"
#include "dbus_type_double.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"

namespace DBus { namespace test {

void TestUnmarshall(Type::Double& dbusType, double value)
{
    UnmarshallingData data;
    char * bytePtr = (char*)&value;
    for (auto i = 0; i < sizeof(double); ++i, ++bytePtr) {
        data.c = *bytePtr;
        if (i != sizeof(double) - 1) {
            REQUIRE(!dbusType.unmarshall(data));
        } else {
            REQUIRE(dbusType.unmarshall(data));
        }
        ++data.offset;
    }

    std::stringstream ss;
    ss << value;
    REQUIRE(dbusType.asString() == ss.str());
}

TEST_CASE("Unmarshall double little endian")
{
    Type::Double dbusType;
    TestUnmarshall(dbusType, 5346.12);
}

TEST_CASE("Unmarshall double big endian")
{
    Type::Double dbusType;
    dbusType.setLittleEndian(false);
    TestUnmarshall(dbusType, 67.25);
}

TEST_CASE("Marshall and unmarshall double")
{
    MessageOStream stream;
    double value = 1.2234;
    stream.writeDouble(value);

    Type::Double dbusType;
    UnmarshallingData data;
    std::for_each(stream.data.begin(), stream.data.end(),
        [&data, &dbusType](char c) {
            data.c = c;
            dbusType.unmarshall(data);
        });

    std::stringstream ss;
    ss << value;
    REQUIRE(dbusType.asString() == ss.str());
}


}} // namespace DBus::test

using namespace DBus::test;
