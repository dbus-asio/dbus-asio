#ifndef DBUS_TYPE_INT16_H
#define DBUS_TYPE_INT16_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {

    class Int16 : public Base {
    public:
        Int16();
        Int16(int16_t v);

        size_t getAlignment() const { return 2; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        static const std::string s_StaticTypeCode;

    protected:
        int16_t m_Value;

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
