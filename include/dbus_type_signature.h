#ifndef DBUS_TYPE_SIGNATURE_H
#define DBUS_TYPE_SIGNATURE_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {

    class Signature : public Base {
    public:
        Signature();
        Signature(const std::string& v);

        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;
        const std::string& getValue() const;

        static const std::string s_StaticTypeCode;

    protected:
        std::string m_Value;

        struct Unmarshalling {
            Unmarshalling()
                : count(0)
            {
            }

            size_t count;
            size_t signature_size;
        } m_Unmarshalling;
    };
}
}

#endif
