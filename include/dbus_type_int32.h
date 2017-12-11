#ifndef DBUS_TYPE_INT32_H
#define DBUS_TYPE_INT32_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {

    class Int32 : public Base {
    public:
        Int32();
        Int32(int32_t v);

        size_t getAlignment() const { return 4; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        static const std::string s_StaticTypeCode;

    protected:
        int32_t m_Value;

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
