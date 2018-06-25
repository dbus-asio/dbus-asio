#ifndef DBUS_TYPE_BOOLEAN_H
#define DBUS_TYPE_BOOLEAN_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {

    class Boolean : public Base {
    public:
        Boolean();
        Boolean(uint32_t v);

        size_t getAlignment() const { return 4; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        static const std::string s_StaticTypeCode;

    protected:
        uint32_t m_Value;

        struct Unmarshalling {
            Unmarshalling()
                : areWeSkippingPadding(true)
                , count(0)
            {
            }

            bool areWeSkippingPadding;
            size_t count;
        } m_Unmarshalling;
    };
}
}

#endif
