#include "dbus_platform.h"
#include "dbus_type.h"

uint32_t DBus::Platform::getPID() { return getpid(); }

uint32_t DBus::Platform::getUID() { return getuid(); }

std::string DBus::Platform::getSystemBus()
{
    const char* bus = getenv("DBUS_SYSTEM_BUS_ADDRESS"); // AFAIK, this is not a standardised name

    if (bus) {
        std::string result(bus);
        if (result.substr(0, 10) == "unix:path=") {
            return result.substr(10);
        }
        return result;
    }

    return "/var/run/dbus/system_bus_socket";
}

std::string DBus::Platform::getSessionBus()
{
    const char* bus = getenv("DBUS_SESSION_BUS_ADDRESS");

    if (bus) {
        std::string result(bus);
        if (result.substr(0, 10) == "unix:path=") {
            return result.substr(10);
        }
        return result;
    }

    return "";
}
