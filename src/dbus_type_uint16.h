#ifndef DBUS_TYPE_UINT16_H
#define DBUS_TYPE_UINT16_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {

    class Uint16 : public Base {
    public:
        Uint16();
        Uint16(uint16_t v);

        size_t getAlignment() const { return 2; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        static const std::string s_StaticTypeCode;

    protected:
        uint16_t m_Value;

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
