#include <iomanip>
#include <sstream>

#include <boost/thread/recursive_mutex.hpp>

#include "dbus_type.h"
#include "dbus_type_base.h"
#include "dbus_type_boolean.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"
#include "dbus_type_struct.h"

const std::string DBus::Type::Boolean::s_StaticTypeCode("b");

DBus::Type::Boolean::Boolean()
    : m_Value(0)
{
    setSignature(s_StaticTypeCode);
}

DBus::Type::Boolean::Boolean(uint32_t v)
    : m_Value(v)
{
    setSignature(s_StaticTypeCode);
}

void DBus::Type::Boolean::marshall(MessageStream& stream) const { stream.writeBoolean(m_Value ? true : false); }

bool DBus::Type::Boolean::unmarshall(const UnmarshallingData& data)
{
    if (m_Unmarshalling.areWeSkippingPadding && !Utils::isAlignedTo(getAlignment(), data.offset)) {
        return false;
    }
    m_Unmarshalling.areWeSkippingPadding = false;

    m_Value <<= 8;
    m_Value += data.c;

    if (++m_Unmarshalling.count == 4) {
        m_Value = doSwap32(m_Value) ? true : false;
        DBus::Validation::throwOnInvalidBoolean(m_Value);
        return true;
    }
    return false;
}

std::string DBus::Type::Boolean::toString(const std::string& prefix) const
{
    std::stringstream ss;

    ss << prefix << "Boolean ";
    ss << (m_Value ? "True" : "Talse") << "\n";

    return ss.str();
}

std::string DBus::Type::Boolean::asString() const
{
    std::stringstream ss;
    ss << (size_t)m_Value;
    return ss.str();
}
