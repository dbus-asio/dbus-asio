#ifndef DBUS_TYPE_VARIANT_H
#define DBUS_TYPE_VARIANT_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {
    class ObjectPath;
    class String;

    class Variant : public Base {
    public:
        Variant();
        Variant(const DBus::Type::ObjectPath& v);
        Variant(const DBus::Type::String& v);
        Variant(const DBus::Type::Uint32& v);

        //std::string getSignature() const { return s_StaticTypeCode; }

        size_t getAlignment() const { return 8; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;
        const DBus::Type::Generic& getValue() const; // please use sparingly - this is provided as a courtesy to the Message::getHeaderField method

        static const std::string s_StaticTypeCode;

    private:
        DBus::Type::Generic m_Value;

        struct Unmarshalling {
            Unmarshalling()
                : isReadingSignature(true)
            {
            }

            bool isReadingSignature;
            DBus::Type::Signature signature;
        } m_Unmarshalling;
    };
}
}

#endif
