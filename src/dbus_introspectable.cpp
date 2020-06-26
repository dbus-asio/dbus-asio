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

#include "dbus_introspectable.h"
#include "dbus_log.h"
#include "dbus_type.h"
#include "dbus_utils.h"

// TODO: Use a proper XML serialization library.

#define DBUS_INTROSPECT_1_0_XML_PUBLIC_IDENTIFIER \
    "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN"
#define DBUS_INTROSPECT_1_0_XML_SYSTEM_IDENTIFIER \
    "http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd"
#define DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE                        \
    "<!DOCTYPE node PUBLIC \"" DBUS_INTROSPECT_1_0_XML_PUBLIC_IDENTIFIER \
    "\"\n\"" DBUS_INTROSPECT_1_0_XML_SYSTEM_IDENTIFIER "\">\n"

void DBus::Introspectable::Introspection::addInterface(const Interface& face)
{
    m_Interfaces.push_back(face);
}

std::string DBus::Introspectable::Introspection::serialize() const
{
    std::string result(DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE);
    result += "<node>";
    for (auto it : m_Interfaces) {
        result += it.serialize();
    }
    result += "</node>";
    return result;
}

DBus::Introspectable::Interface::Interface(const std::string& name)
    : m_Name(name)
{
    // nop
}

void DBus::Introspectable::Interface::addMethod(const Method& method)
{
    m_Methods.push_back(method);
}

void DBus::Introspectable::Interface::addProperty(const Property& property)
{
    m_Properties.push_back(property);
}

void DBus::Introspectable::Interface::addSignal(const Signal& signal)
{
    m_Signals.push_back(signal);
}

std::string DBus::Introspectable::Interface::serialize() const
{
    std::string result;

    result += "<interface name='" + m_Name + "'>";
    for (auto it : m_Properties) {
        result += it.serialize();
    }
    for (auto it : m_Methods) {
        result += it.serialize();
    }
    for (auto it : m_Signals) {
        result += it.serialize();
    }
    result += "</interface>";
    return result;
}

//
// Property
//
DBus::Introspectable::Property::Property(const std::string& name,
    const std::string& type, size_t access)
    : m_Name(name)
    , m_Type(type)
{

    switch (access) {
    case PROPERTY_READ:
        m_Access = "read";
        break;

    case PROPERTY_WRITE:
        m_Access = "write";
        break;

    case PROPERTY_READWRITE:
        m_Access = "readwrite";
        break;

    default:
        DBus::Log::write(Log::WARNING,
            "Invalid access passed to property. Assuming read only.");
        m_Access = "read";
        break;
    }
}

std::string DBus::Introspectable::Property::serialize() const
{
    std::string result;

    result += "<property name='" + m_Name + "' ";
    result += "type='" + m_Type + "' ";
    result += "access='" + m_Access + "' ";
    result += "/>";

    return result;
}

//
// Signal
//
DBus::Introspectable::Signal::Signal(const std::string& name,
    const std::string& type)
    : m_Name(name)
    , m_Type(type)
{
}

std::string DBus::Introspectable::Signal::serialize() const
{
    std::string result;

    result += "<signal name='" + m_Name + "'>";
    for (auto type : m_Type) {
        result += "<arg type='";
        result += type;
        result += "'/>";
    }
    result += "</signal>";

    return result;
}

//
// Method
//
DBus::Introspectable::Method::Method(const std::string& name,
    const std::string& in_params,
    const std::string& out_params)
    : m_Name(name)
    , m_InParams(in_params)
    , m_OutParams(out_params)
{
}

std::string DBus::Introspectable::Method::serialize() const
{
    std::string result;

    result += "<method name='" + m_Name + "'>";

    size_t idx = 0;
    while (idx < m_InParams.size()) {
        std::string sig(DBus::Type::extractSignature(m_InParams, idx));
        result += "<arg direction='in' type='" + sig + "'/>\n";
        idx += sig.size();
    }
    //
    idx = 0;
    while (idx < m_OutParams.size()) {
        std::string sig(DBus::Type::extractSignature(m_OutParams, idx));
        result += "<arg direction='out' type='" + sig + "'/>\n";
        idx += sig.size();
    }

    result += "</method>";
    return result;
}
