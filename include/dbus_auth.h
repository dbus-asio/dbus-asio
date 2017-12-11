#ifndef DBUS_AUTH_H
#define DBUS_AUTH_H

namespace DBus {
class Transport;

// See Authentication state diagrams
class AuthenticationProtocol {
public:
    typedef enum AuthRequired {
        AUTH_BASIC,
        AUTH_NEGOTIATE_UNIX_FD,
    } AuthRequired;

    AuthenticationProtocol(Transport& transport);

    void reset();
    void sendAuthListMethods();
    void sendAuth(AuthenticationProtocol::AuthRequired type = AUTH_BASIC);
    bool onReceiveOctet(uint8_t c);

protected:
    bool onOK(const std::string& guid /* unused in this case */);
    bool onError(const std::string& error_message);
    bool onRejected(const std::string& error_message);
    bool onAgreeUnixFD();

    bool onAuth(const std::string&);
    bool onData(const std::string&);
    bool onCancel();
    bool onNegotiateUnixFD(const std::string&);

    void sendNegotiateUnixFD();
    void sendBegin();
    void sendData(std::string& data);
    void sendWire(const std::string& data);
    // Return true once we have completed the auth state
    bool onCommand(const std::string& command);

private:
    Transport& m_Transport;
    uint8_t m_LastOctetSeen;
    std::string m_CurrentCommand;
    AuthenticationProtocol::AuthRequired m_AuthType;
    mutable boost::recursive_mutex m_AuthTypeMutex;
};
}

#endif // DBUS_AUTH_H
