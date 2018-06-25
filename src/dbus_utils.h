#ifndef DBUS_UTILS_H
#define DBUS_UTILS_H

namespace DBus {
namespace Utils {
    void ConvertHexStringToBinary(std::string& result, const std::string& input);
    void ConvertBinaryToHexString(std::string& result, const std::string& input);
    bool isAlignedTo(size_t pad, size_t size);
    bool isAlignedTo8(size_t offset);
    size_t getPadding(size_t pad, size_t size);
}
}

#endif // DBUS_UTILS_H