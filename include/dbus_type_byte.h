#ifndef DBUS_TYPE_BYTE_H
#define DBUS_TYPE_BYTE_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {

    class Byte : public Base {
    public:
        Byte();
        Byte(size_t v);
        Byte(const Byte& b);

        std::string getSignature() const { return s_StaticTypeCode; }

        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        static const std::string s_StaticTypeCode;

    protected:
        uint8_t m_Value;
    };
}
}

#endif
