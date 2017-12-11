#include <iomanip>
#include <sstream>

#include <boost/thread/recursive_mutex.hpp>

#include "dbus_type.h"
#include "dbus_type_base.h"
#include "dbus_type_byte.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"
#include "dbus_type_struct.h"

const std::string DBus::Type::Byte::s_StaticTypeCode("y");

DBus::Type::Byte::Byte()
    : m_Value(0)
{
    setSignature(s_StaticTypeCode);
}

DBus::Type::Byte::Byte(size_t v)
    : m_Value(v)
{
    setSignature(s_StaticTypeCode);
}

DBus::Type::Byte::Byte(const Byte& b) { m_Value = b.m_Value; }

void DBus::Type::Byte::marshall(MessageStream& stream) const { stream.writeByte(m_Value); }

bool DBus::Type::Byte::unmarshall(const UnmarshallingData& data)
{
    m_Value = data.c;
    return true;
}

std::string DBus::Type::Byte::toString(const std::string& prefix) const
{
    std::stringstream ss;

    ss << prefix << "Byte ";
    ss << (size_t)m_Value << " (0x" << std::hex << std::setfill('0') << std::setw(2) << int(uint8_t(m_Value)) << ")\n";

    return ss.str();
}

std::string DBus::Type::Byte::asString() const
{
    std::stringstream ss;
    ss << (size_t)m_Value;
    return ss.str();
}
