#ifndef DBUS_PLATFORM_H
#define DBUS_PLATFORM_H

namespace DBus {
namespace Platform {
    // TODO: Is this abstraction truly needed?
    //typedef _pid_t _pid_t;
    //typedef dbus_uid_t _pid_t;

    uint32_t getPID();
    uint32_t getUID();

    std::string getSystemBus();
    std::string getSessionBus();
}
}

#endif
