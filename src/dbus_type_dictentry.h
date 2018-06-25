#ifndef DBUS_TYPE_DICTENTRY_H
#define DBUS_TYPE_DICTENTRY_H

namespace DBus {

namespace Type {

    class DictEntry : public Base {
    public:
        DictEntry() {}
        DictEntry(const DBus::Type::Generic& key, const DBus::Type::Generic& value);
        DictEntry(const std::string& key, std::string& value);
        DictEntry(const std::string& key, uint32_t value);

        std::string getSignature() const;
        size_t getAlignment() const { return 8; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        void set(const DBus::Type::Generic& key, const DBus::Type::Generic& value);

        static const std::string s_StaticTypeCode;

    protected:
        std::pair<Generic, Generic> m_Value;

        struct Unmarshalling {
            Unmarshalling()
                : areWeSkippingPadding(true)
                , onKeyType(true)
                , count(0)
            {
            }

            bool areWeSkippingPadding;
            bool onKeyType;
            size_t count;
        } m_Unmarshalling;
    };
}
}

#endif
