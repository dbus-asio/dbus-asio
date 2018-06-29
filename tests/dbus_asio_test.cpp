#include <cstdio>
#include "dbus.h"

int main()
{
    DBus::Log::setLevel(DBus::Log::WARNING);

    DBus::Log::write(DBus::Log::INFO, "System bus: %s\n", DBus::Platform::getSystemBus().c_str());
    DBus::Log::write(DBus::Log::INFO, "Session bus: %s\n", DBus::Platform::getSessionBus().c_str());
    DBus::Native native(DBus::Platform::getSessionBus());
    sleep(1);

    native.BeginAuth(DBus::AuthenticationProtocol::AUTH_BASIC); // AUTH_BASIC or AUTH_NEGOTIATE_UNIX_FD
    return 0;
}
