#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <string>

#include "dbus_utils.h"

void DBus::Utils::ConvertHexStringToBinary(std::string& result, const std::string& input)
{
    std::istringstream ss(input);
    std::string word;
    while (ss >> std::setw(2) >> word) {
        std::stringstream converter;
        uint16_t temp;
        converter << std::hex << word;
        converter >> temp;
        result.push_back(temp);
    }
}

void DBus::Utils::ConvertBinaryToHexString(std::string& result, const std::string& input)
{
    std::stringstream ss;

    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < input.length(); ++i) {
        ss << std::setw(2) << int(uint8_t(input[i]));
    }
    result.append(ss.str());
}

bool DBus::Utils::isAlignedTo(size_t pad, size_t size) { return (size % pad) == 0 ? true : false; }

bool DBus::Utils::isAlignedTo8(size_t offset) { return isAlignedTo(8, offset); }

// How many bytes of padding are required so that 'size' is padded to 'pad'
size_t DBus::Utils::getPadding(size_t pad, size_t size)
{
    // The zero case is generally not used but be used as code-as-comment (e.g. pad(0) to indicate none is needed)
    if (pad == 0) {
        return 0;
    }

    size_t required = pad - (size % pad);

    if (required == pad) {
        return 0;
    }
    return required;
}
