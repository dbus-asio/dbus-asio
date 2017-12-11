#ifndef DBUS_TYPE_ARRAY_H
#define DBUS_TYPE_ARRAY_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {
    class Struct;
    class DictEntry;

    class Array : public Base {
    public:
        size_t size() const;

        size_t add(const DBus::Type::Struct& s);
        size_t add(const DBus::Type::DictEntry& s);

        size_t getAlignment() const { return 4; }
        void marshall(MessageStream& stream) const;
        void marshallContents(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string getSignature() const;

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        const std::vector<DBus::Type::Generic>& getContents() const;

        static const std::string s_StaticTypeCode;

    protected:
        std::vector<DBus::Type::Generic> contents;

        struct Unmarshalling {
            Unmarshalling()
                : areWeSkippingPaddingForSize(true)
                , areWeSkippingPaddingForElement(true)
                , count(0)
                , array_size(0)
            {
            }

            bool areWeSkippingPaddingForSize;
            bool areWeSkippingPaddingForElement;
            size_t count;
            size_t array_size;
            bool createType;
            std::string typeSignature;
        } m_Unmarshalling;
    };
}
}

#endif
