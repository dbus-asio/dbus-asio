#include <boost/thread/recursive_mutex.hpp>

#include "dbus_type.h"
#include "dbus_type_base.h"
#include "dbus_type_helpers.h"
#include "dbus_type_objectpath.h"
#include "dbus_type_string.h"
#include "dbus_type_struct.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"
#include "dbus_validation.h"

const std::string DBus::Type::ObjectPath::s_StaticTypeCode("o");

DBus::Type::ObjectPath::ObjectPath() { setSignature(s_StaticTypeCode); }

DBus::Type::ObjectPath::ObjectPath(const std::string& v)
    : String(v)
{
    setSignature(s_StaticTypeCode);
}

void DBus::Type::ObjectPath::marshall(MessageStream& stream) const
{
    Validation::throwOnInvalidObjectPath(m_Value);

    // Exactly the same as STRING except the content must be a valid object path (see above).
    Type::String::marshall(stream);
}
