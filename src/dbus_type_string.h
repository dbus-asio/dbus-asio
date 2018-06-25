#ifndef DBUS_TYPE_STRING_H
#define DBUS_TYPE_STRING_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {

    class String : public Base {
    public:
        String();
        String(const std::string& v);

        std::string getSignature() const { return s_StaticTypeCode; }
        size_t getAlignment() const { return 4; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        static const std::string s_StaticTypeCode;

    protected:
        std::string m_Value;

        struct Unmarshalling {
            Unmarshalling()
                : areWeSkippingPadding(true)
                , size(0)
                , count(0)
            {
            }

            bool areWeSkippingPadding;
            size_t size;
            size_t count;
        } m_Unmarshalling;
    };
}
}

#endif
