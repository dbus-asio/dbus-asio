#ifndef DBUS_VALIDATION_H
#define DBUS_VALIDATION_H

namespace DBus {
namespace Validation {
    void throwOnInvalidRule(const std::string& rule);

    void throwOnInvalidBoolean(uint32_t value);
    void throwOnInvalidSignature(const std::string& signature);
    void throwOnInvalidObjectPath(const std::string& path);
    void throwOnInvalidBasicType(const std::string& type);
    void throwOnInvalidSingleCompleteType(const std::string& type);
    void throwOnInvalidContainerType(const std::string& type);
    void throwOnInvalidInterfaceName(const std::string& name);
    void throwOnInvalidBusName(const std::string& name);
    void throwOnInvalidMethodName(const std::string& name);
    void throwOnInvalidErrorName(const std::string& name);

    bool isValidRule(const std::string& rule);
    bool isValidFixedType(const std::string& type);
    bool isValidStringLikeType(const std::string& type);
    bool isValidSignature(const std::string& signature);
    bool isValidObjectPath(const std::string& path);
    bool isValidBasicType(const std::string& type);
    bool isValidSingleCompleteType(const std::string& type);
    bool isValidContainerType(const std::string& type);
    bool isValidInterfaceName(const std::string& name);
    bool isValidBusName(const std::string& name);
    bool isValidMethodName(const std::string& name);
    bool isValidErrorName(const std::string& name);
}
}

#endif // DBUS_VALIDATION_H
