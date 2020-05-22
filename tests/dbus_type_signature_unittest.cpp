#include <catch2/catch.hpp>
#include "dbus_type_signature.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"
#include "dbus_type.h"
#include <iostream>

namespace DBus { namespace test {

void TestUnmarshallFromStream(const std::string &stream,
    unsigned byteOrder, const std::string str)
{
    // An array of structures
    Type::Signature dbusString;
    dbusString.setLittleEndian(byteOrder == __LITTLE_ENDIAN);

    UnmarshallingData data;
    for (auto i = 0; i < stream.size(); ++i) {
        data.c = stream[i];
        if (i != stream.size() - 1) {
            REQUIRE(!dbusString.unmarshall(data));
        } else {
            REQUIRE(dbusString.unmarshall(data));
        }
        ++data.offset;
    }

    REQUIRE(dbusString.asString() == str);
}

void TestUnmarshall(unsigned byteOrder, const std::string& str)
{
    std::string stream;
    uint8_t length = str.size();

    stream.push_back(length);
    stream.append(str);
    stream.push_back('\0');

    TestUnmarshallFromStream(stream, byteOrder, str);
}

TEST_CASE("Unmarshall signature - little endian")
{
    TestUnmarshall(__LITTLE_ENDIAN, "{ii}");
}

TEST_CASE("Unmarshall signature - big endian")
{
    TestUnmarshall(__BIG_ENDIAN, "{ii}");
}

TEST_CASE("Marshall and unmarshall signature")
{
    const std::string str("{ii}");
    Type::Signature dbusString(str);

    MessageStream stream;
    dbusString.marshall(stream);

    // Marshalling adds additional signature to signal a signature!
    std::string signatureSignature(stream.data.begin(), stream.data.begin() + 3);
    TestUnmarshallFromStream(signatureSignature, __LITTLE_ENDIAN, "g");

    std::string signatureValue(stream.data.begin() + 3, stream.data.end());
    TestUnmarshallFromStream(signatureValue, __LITTLE_ENDIAN, str);
}


}} // namespace DBus::test

using namespace DBus::test;
