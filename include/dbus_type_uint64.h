#ifndef DBUS_TYPE_UINT64_H
#define DBUS_TYPE_UINT64_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {

    class Uint64 : public Base {
    public:
        Uint64();
        Uint64(uint64_t v);

        size_t getAlignment() const { return 8; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        static const std::string s_StaticTypeCode;

    protected:
        uint64_t m_Value;

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
