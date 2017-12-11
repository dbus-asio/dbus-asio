#include <iomanip>
#include <sstream>

#include <boost/thread/recursive_mutex.hpp>

#include "dbus_type.h"
#include "dbus_type_base.h"
#include "dbus_type_uint16.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"
#include "dbus_type_struct.h"

const std::string DBus::Type::Uint16::s_StaticTypeCode("q");

DBus::Type::Uint16::Uint16()
    : m_Value(0)
{
    setSignature(s_StaticTypeCode);
}

DBus::Type::Uint16::Uint16(uint16_t v)
    : m_Value(v)
{
    setSignature(s_StaticTypeCode);
}

void DBus::Type::Uint16::marshall(MessageStream& stream) const { stream.writeUint16(m_Value); }

bool DBus::Type::Uint16::unmarshall(const UnmarshallingData& data)
{
    if (m_Unmarshalling.areWeSkippingPadding && !Utils::isAlignedTo(getAlignment(), data.offset)) {
        return false;
    }
    m_Unmarshalling.areWeSkippingPadding = false;

    m_Value <<= 8;
    m_Value += data.c;

    if (++m_Unmarshalling.count == 2) {
        m_Value = doSwap16(m_Value);
        return true;
    }
    return false;
}

std::string DBus::Type::Uint16::toString(const std::string& prefix) const
{
    std::stringstream ss;

    ss << prefix << "Uint16 ";
    ss << m_Value << " (0x" << std::hex << std::setfill('0') << std::setw(2) << m_Value << ")\n";

    return ss.str();
}

std::string DBus::Type::Uint16::asString() const
{
    std::stringstream ss;
    ss << m_Value;
    return ss.str();
}
