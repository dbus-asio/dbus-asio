#ifndef DBUS_TYPE_UINT32_H
#define DBUS_TYPE_UINT32_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {

    class Uint32 : public Base {
    public:
        Uint32();
        Uint32(uint32_t v);

        //			std::string getSignature() const { return s_StaticTypeCode; }
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
