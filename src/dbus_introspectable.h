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

#ifndef DBUS_INTROSPECTABLE_H
#define DBUS_INTROSPECTABLE_H

#include <string>
#include <vector>

namespace DBus {
namespace Introspectable {
    class Interface;
    class Method;
    class Property;
    class Signal;

    class Introspection {
    public:
        void addInterface(const Interface& iface);
        std::string serialize() const;

    protected:
        std::vector<Interface> m_Interfaces;
    };

    class Interface {
    public:
        Interface(const std::string& name);
        void addMethod(const Method& method);
        void addProperty(const Property& property);
        void addSignal(const Signal& signal);
        std::string serialize() const;

    protected:
        std::vector<Method> m_Methods;
        std::vector<Property> m_Properties;
        std::vector<Signal> m_Signals;
        std::string m_Name;
    };

    class Method {
    public:
        Method(const std::string& name, const std::string& in_params,
            const std::string& out_params);

        std::string serialize() const;

        std::string m_Name;
        std::string m_InParams;
        std::string m_OutParams;
    };

    class Property {
    public:
        enum {
            PROPERTY_READ,
            PROPERTY_WRITE,
            PROPERTY_READWRITE,
        } PropertAccess;

        Property(const std::string& name, const std::string& type,
            size_t access = Property::PROPERTY_READ);

        std::string serialize() const;

    protected:
        std::string m_Name;
        std::string m_Type;
        std::string m_Access;
    };

    class Signal {
    public:
        Signal(const std::string& name, const std::string& type);

        std::string serialize() const;

    protected:
        std::string m_Name;
        std::string m_Type;
    };

    // TODO: Support names for each property and signal
} // namespace Introspectable
} // namespace DBus

#endif
