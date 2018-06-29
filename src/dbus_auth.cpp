#include "dbus_auth.h"
#include "dbus_log.h"
#include "dbus_platform.h"
#include "dbus_transport.h"
#include "dbus_type.h"
#include "dbus_utils.h"

/*
The AUTH handshake is:

Server        Client
        <---  AUTH
OK      ---->
        <---  (1) BEGIN (or)
        <---  (2) NEGOTIATE_UNIX_FD
AGREE   ---->
        <---  BEGIN
*/

DBus::AuthenticationProtocol::AuthenticationProtocol(std::shared_ptr<Transport>& transport)
    : m_Transport(transport)
    , m_LastOctetSeen(0)
{
}

void DBus::AuthenticationProtocol::reset()
{
    m_LastOctetSeen = 0;
    sendAuth(m_AuthType);
}

void DBus::AuthenticationProtocol::sendAuthListMethods() { sendWire(std::string("AUTH\r\n")); }

// RCVD: REJECTED EXTERNAL DBUS_COOKIE_SHA1 ANONYMOUS
void DBus::AuthenticationProtocol::sendAuth(DBus::AuthenticationProtocol::AuthRequired type /* = AUTH_BASIC*/)
{
    {
        boost::recursive_mutex::scoped_lock guard(m_AuthTypeMutex);
        m_AuthType = type;
    }

    std::string auth("AUTH EXTERNAL ");
    DBus::Utils::ConvertBinaryToHexString(auth, std::to_string(Platform::getUID()));
    auth += "\r\n";
    sendWire(auth);
}

bool DBus::AuthenticationProtocol::onReceiveOctet(uint8_t c)
{
    // rt indicates whether we have complete authentication, and can switch mode into
    // message handling. I don't like this ATM, since the switch is governed by the
    // sendBegin message, not the receiving of the incoming command. They are 100%
    // equivalent (i.e. receiving AGREE_UNIX_FD is the same as sending BEGIN) but it
    // feels icky.
    bool rt = false;

    m_CurrentCommand += c;

    if (m_LastOctetSeen == '\r' && c == '\n') {
        Log::write(Log::TRACE, "DBus :: RCVD: %s", m_CurrentCommand.c_str());

        // Pass a temporary version of the string, in case we receive a second call to
        // onReceiveOctet before onCommand returns.
        std::string tmp_command(m_CurrentCommand);
        m_CurrentCommand = "";
        rt = onCommand(tmp_command);
    }

    m_LastOctetSeen = c;
    return rt;
}

//
// Protected methods
//
bool DBus::AuthenticationProtocol::onOK(const std::string& guid /* unused in this case */)
{
    std::string address;
    DBus::Utils::ConvertHexStringToBinary(address, guid);

    bool send_begin = false;

    {
        boost::recursive_mutex::scoped_lock guard(m_AuthTypeMutex);
        if (m_AuthType == AUTH_BASIC) {
            send_begin = true;
        }
    }

    if (send_begin) {
        sendBegin();
        return true;
    }

    sendNegotiateUnixFD();
    return false;
}

bool DBus::AuthenticationProtocol::onError(const std::string& error_message)
{
    DBus::Log::write(DBus::Log::ERROR, "DBus :: onError : %s\n", error_message.c_str());
    return false;
}

bool DBus::AuthenticationProtocol::onRejected(const std::string& error_message)
{
    // Split by space
    // Store the list of available auth methods
    // Optionally, retry. (determin by cb?)
    DBus::Log::write(DBus::Log::WARNING, "DBus :: Reject : %s\n", error_message.c_str());
    return false;
}

bool DBus::AuthenticationProtocol::onAgreeUnixFD()
{
    DBus::Log::write(DBus::Log::INFO, "DBus :: onAgreeUnixFD\n");
    sendBegin();
    return true;
}

bool DBus::AuthenticationProtocol::onAuth(const std::string&) { return false; }

bool DBus::AuthenticationProtocol::onData(const std::string&) { return false; }

bool DBus::AuthenticationProtocol::onCancel() { return false; }

bool DBus::AuthenticationProtocol::onNegotiateUnixFD(const std::string&) { return false; }

void DBus::AuthenticationProtocol::sendNegotiateUnixFD() { sendWire(std::string("NEGOTIATE_UNIX_FD\r\n")); }

void DBus::AuthenticationProtocol::sendBegin()
{
    sendWire(std::string("BEGIN\r\n"));
    m_Transport->onAuthComplete();
}

void DBus::AuthenticationProtocol::sendData(std::string& data)
{
    std::string packet("DATA ");
    DBus::Utils::ConvertBinaryToHexString(packet, data);
    packet += "\r\n";

    sendWire(packet);
}

void DBus::AuthenticationProtocol::sendWire(const std::string& data) { m_Transport->sendStringDirect(data); }

// Return true once we have completed the auth state
bool DBus::AuthenticationProtocol::onCommand(const std::string& command)
{
    DBus::Log::write(DBus::Log::TRACE, "DBus :: CMD: %s\n", command.c_str());
    DBus::Log::writeHex(DBus::Log::TRACE, "DBus :: CMD: ", command);

    std::pair<const char*, std::function<bool(const std::string&)> > callback_list[8] = {
        // Client
        std::make_pair("OK", std::bind(&DBus::AuthenticationProtocol::onOK, this, std::placeholders::_1)),
        std::make_pair("ERROR", std::bind(&DBus::AuthenticationProtocol::onError, this, std::placeholders::_1)),
        std::make_pair("REJECTED", std::bind(&DBus::AuthenticationProtocol::onRejected, this, std::placeholders::_1)),
        std::make_pair("AGREE_UNIX_FD", std::bind(&DBus::AuthenticationProtocol::onAgreeUnixFD, this)),
        std::make_pair("DATA", std::bind(&DBus::AuthenticationProtocol::onData, this, std::placeholders::_1)),
        // Server (TODO)
        std::make_pair("AUTH", std::bind(&DBus::AuthenticationProtocol::onAuth, this, std::placeholders::_1)),
        std::make_pair("NEGOTIATE_UNIX_FD", std::bind(&DBus::AuthenticationProtocol::onNegotiateUnixFD, this, std::placeholders::_1)),
        std::make_pair("CANCEL", std::bind(&DBus::AuthenticationProtocol::onCancel, this))
    };

    for (size_t i = 0; i < 8; ++i) {
        if (command.substr(0, strlen(callback_list[i].first)) == callback_list[i].first) {
            return callback_list[i].second(command.substr(strlen(callback_list[i].first) + 1));
        }
    }

    DBus::Log::write(DBus::Log::WARNING, "DBus :: CMD: %s did not execute anything, so please implement the method.\n", command.c_str());

    return false;
}
