#ifndef DBUS_TYPE_INT64_H
#define DBUS_TYPE_INT64_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {

    class Int64 : public Base {
    public:
        Int64();
        Int64(int64_t v);

        size_t getAlignment() const { return 8; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        static const std::string s_StaticTypeCode;

    protected:
        int64_t m_Value;

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
