#ifndef DBUS_TYPE_OBJECTPATH_H
#define DBUS_TYPE_OBJECTPATH_H

namespace DBus {
class MessageStream;

namespace Type {

    class ObjectPath : public String {
    public:
        ObjectPath();
        ObjectPath(const std::string& v);

        std::string getSignature() const { return s_StaticTypeCode; }
        void marshall(MessageStream& stream) const;
        static const std::string s_StaticTypeCode;
    };
}
}

#endif
