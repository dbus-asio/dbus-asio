#include "dbus_type.h"
#include <byteswap.h>

bool DBus::Type::Base::isSwapRequired() const
{
    if (__BYTE_ORDER == __LITTLE_ENDIAN && m_isLittleEndian) {
        return true;
    }
    if (__BYTE_ORDER == __BIG_ENDIAN && !m_isLittleEndian) {
        return true;
    }
    return false;
}

uint16_t DBus::Type::Base::doSwap16(uint16_t v) const
{
    if (isSwapRequired()) {
        return bswap_16(v);
    }
    return v;
}

uint32_t DBus::Type::Base::doSwap32(uint32_t v) const
{
    if (isSwapRequired()) {
        return bswap_32(v);
    }
    return v;
}

uint64_t DBus::Type::Base::doSwap64(uint64_t v) const
{
    if (isSwapRequired()) {
        return bswap_64(v);
    }
    return v;
}

void DBus::Type::Base::setSignature(const std::string& type) { m_Signature = type; }

std::string DBus::Type::Base::getSignature() const { return m_Signature; }

std::string DBus::Type::Base::toString(const std::string& prefix) const { return std::string(prefix + "\n"); }

std::string DBus::Type::Base::asString() const { return ""; }
