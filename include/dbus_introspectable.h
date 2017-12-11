#ifndef DBUS_INTROSPECTABLE_H
#define DBUS_INTROSPECTABLE_H

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
        Method(const std::string& name, const std::string& in_params, const std::string& out_params);

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

        Property(const std::string& name, const std::string& type, size_t access = Property::PROPERTY_READ);

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
}
}

#endif
