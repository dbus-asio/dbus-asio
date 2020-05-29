#include <catch2/catch.hpp>
#include "dbus_type_signature.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"
#include "dbus_messageistream.h"
#include "dbus_type.h"
#include <iostream>

namespace DBus { namespace test {

void TestUnmarshallFromMessageIStream(const std::string &stream,
    unsigned byteOrder, const std::string str)
{
    // An array of structures
    Type::Signature signature;
    MessageIStream istream((uint8_t*)stream.data(), stream.size(), byteOrder == __BYTE_ORDER);
    signature.unmarshall(istream);

    REQUIRE(signature.asString() == str);
}

void TestUnmarshallFromMessageIStream(unsigned byteOrder, const std::string& str)
{
    std::string stream;
    uint8_t length = str.size();

    stream.push_back(length);
    stream.append(str);
    stream.push_back('\0');

    TestUnmarshallFromMessageIStream(stream, byteOrder, str);
}

TEST_CASE("Unmarshall signature - little endian from MessageIStream")
{
    TestUnmarshallFromMessageIStream(__LITTLE_ENDIAN, "{ii}");
}

TEST_CASE("Unmarshall signature - big endian from MessageIStream")
{
    TestUnmarshallFromMessageIStream(__BIG_ENDIAN, "{ii}");
}

TEST_CASE("Marshall and unmarshall signature")
{
    const std::string str("{ii}");
    Type::Signature dbusString(str);

    MessageOStream stream;
    dbusString.marshall(stream);

    // Marshalling adds additional signature to signal a signature!
    std::string signatureSignature(stream.data.begin(), stream.data.begin() + 3);
    TestUnmarshallFromMessageIStream(signatureSignature, __LITTLE_ENDIAN, "g");

    std::string signatureValue(stream.data.begin() + 3, stream.data.end());
    TestUnmarshallFromMessageIStream(signatureValue, __LITTLE_ENDIAN, str);
}


}} // namespace DBus::test

using namespace DBus::test;
