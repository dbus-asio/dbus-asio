// This file is part of dbus-asio
// Copyright 2018 Brightsign LLC
//
// This library is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, version 3, or at your
// option any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// The GNU Lesser General Public License version 3 is included in the
// file named COPYING. If you do not have this file see
// <http://www.gnu.org/licenses/>.

#include "dbus_log.h"

#include "dbus_type.h"
#include "dbus_type_base.h"
#include "dbus_type_struct.h"

#include "dbus_auth.h"
#include "dbus_transport.h"

#include "dbus_matchrule.h"
#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_native.h"
#include "dbus_transport.h"

#define MESSAGE_BUS_OBJECT "/org/freedesktop/DBus"
#define MESSAGE_BUS_INTERFACE "org.freedesktop.DBus"

// This file contains only the implementations of the primary "Message Bus Messages"
// They're included primarily as a convenience to application developers, as the implementation
// is trivial.
void DBus::Native::callHello(const DBus::Message::CallbackFunctionMethodReturn& success, const Message::CallbackFunctionError& failure)
{
    DBus::Message::MethodCall method(DBus::Message::MethodCallIdentifier(MESSAGE_BUS_OBJECT, MESSAGE_BUS_INTERFACE, "Hello"));

    sendMethodCall(DBus::Native::DBusDaemon, method, success, failure);
}

void DBus::Native::callRequestName(const std::string& name, uint32_t flags, const DBus::Message::CallbackFunctionMethodReturn& success,
    const Message::CallbackFunctionError& failure)
{
    Message::MethodCallIdentifier id(MESSAGE_BUS_OBJECT, MESSAGE_BUS_INTERFACE, "RequestName");
    Message::MethodCallParametersIn inparams(name, flags);
    Message::MethodCall method(id, inparams);

    sendMethodCall(DBus::Native::DBusDaemon, method, success, failure);
}

void DBus::Native::callReleaseName(const std::string& name, const DBus::Message::CallbackFunctionMethodReturn& success,
    const Message::CallbackFunctionError& failure)
{
    Message::MethodCallIdentifier id(MESSAGE_BUS_OBJECT, MESSAGE_BUS_INTERFACE, "ReleaseName");
    Message::MethodCallParametersIn inparams(name);
    Message::MethodCall method(id, inparams);

    sendMethodCall(DBus::Native::DBusDaemon, method, success, failure);
}

void DBus::Native::callListQueuedOwners(const std::string& bus_name, const DBus::Message::CallbackFunctionMethodReturn& success,
    const Message::CallbackFunctionError& failure)
{
    Message::MethodCallIdentifier id(MESSAGE_BUS_OBJECT, MESSAGE_BUS_INTERFACE, "ListQueuedOwners");
    Message::MethodCallParametersIn inparams(bus_name);
    Message::MethodCall method(id, inparams);

    sendMethodCall(DBus::Native::DBusDaemon, method, success, failure);
}

void DBus::Native::callListNames(const DBus::Message::CallbackFunctionMethodReturn& success, const Message::CallbackFunctionError& failure)
{
    DBus::Message::MethodCall method(Message::MethodCallIdentifier(MESSAGE_BUS_OBJECT, MESSAGE_BUS_INTERFACE, "ListNames"));

    sendMethodCall(DBus::Native::DBusDaemon, method, success, failure);
}

void DBus::Native::callListActivatableNames(const DBus::Message::CallbackFunctionMethodReturn& success, const Message::CallbackFunctionError& failure)
{
    DBus::Message::MethodCall method(Message::MethodCallIdentifier(MESSAGE_BUS_OBJECT, MESSAGE_BUS_INTERFACE, "ListActivatableNames"));

    sendMethodCall(DBus::Native::DBusDaemon, method, success, failure);
}

void DBus::Native::callNameHasOwner(const std::string& name, const DBus::Message::CallbackFunctionMethodReturn& success,
    const Message::CallbackFunctionError& failure)
{
    Message::MethodCallIdentifier id(MESSAGE_BUS_OBJECT, MESSAGE_BUS_INTERFACE, "NameHasOwner");
    Message::MethodCallParametersIn inparams(name);
    Message::MethodCall method(id, inparams);

    sendMethodCall(DBus::Native::DBusDaemon, method, success, failure);
}

void DBus::Native::callAddMatch(const std::string& rulestring, const DBus::Message::CallbackFunctionMethodReturn& success,
    const Message::CallbackFunctionError& failure, const Message::CallbackFunctionSignal& handler)
{
    // Note: Rules are specified as a string of comma separated key/value pairs
    // Possible keys you can match on are type, sender, interface, member, path, destination and numbered keys to
    // match message args (keys are 'arg0', 'arg1', etc.)
    Message::MethodCallIdentifier id(MESSAGE_BUS_OBJECT, MESSAGE_BUS_INTERFACE, "AddMatch");
    Message::MethodCallParametersIn inparams(rulestring);
    Message::MethodCall method(id, inparams);

    {
        boost::recursive_mutex::scoped_lock guard(m_RulesMapMutex);
        DBus::MatchRule newrule(rulestring, handler);
        m_RulesMap.emplace(rulestring, newrule);
    }

    sendMethodCall(DBus::Native::DBusDaemon, method, success, failure);
}

void DBus::Native::callRemoveMatch(const std::string& rule)
{
    Message::MethodCallIdentifier id(MESSAGE_BUS_OBJECT, MESSAGE_BUS_INTERFACE, "RemoveMatch");
    Message::MethodCallParametersIn inparams(rule);
    Message::MethodCall method(id, inparams, DBus::Message::Header::FLAGS_NO_REPLY_EXPECTED);

    sendMethodCall(DBus::Native::DBusDaemon, method);
}
