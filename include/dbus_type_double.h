#ifndef DBUS_TYPE_DOUBLE_H
#define DBUS_TYPE_DOUBLE_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {

    class Double : public Base {
    public:
        Double();
        Double(double v);

        size_t getAlignment() const { return 8; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        static const std::string s_StaticTypeCode;

    protected:
        double m_Value;

        struct Unmarshalling {
            Unmarshalling()
                : areWeSkippingPadding(true)
                , count(0)
                , value(0)
            {
            }

            bool areWeSkippingPadding;
            size_t count;
            uint64_t value;
        } m_Unmarshalling;
    };
}
}

#endif
