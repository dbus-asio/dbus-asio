#ifndef DBUS_NATIVE
#define DBUS_NATIVE

#include "dbus_transport.h"

namespace DBus {

class Native {
public:
    Native(const std::string& busname);
    ~Native();

    void BeginAuth(AuthenticationProtocol::AuthRequired type);
    void onReceiveAuthOctet(uint8_t c);
    void onReceiveMessageOctet(uint8_t c);

    void registerMethodCallHandler(const std::string& name, const Message::CallbackFunctionMethodCall& handler);
    // registerSignalHandler checks only for matching interfaces, and is a convenience method.
    // It is preferably to use callAddMatch in most cases.
    void registerSignalHandler(const std::string& name, const Message::CallbackFunctionSignal& handler);

    void sendMethodCall(const std::string& destination, const DBus::Message::MethodCall& msg, const DBus::Message::CallbackFunctionMethodReturn& success = [](const DBus::Message::MethodReturn& msg) {}, const DBus::Message::CallbackFunctionError& failure = [](const DBus::Message::Error& msg) {});
    void sendMethodReturn(const std::string& destination, const DBus::Message::MethodReturn& result);
    void sendError(const std::string& destination, const DBus::Message::Error& err);
    void sendSignal(const std::string& destination, const DBus::Message::Signal& signal);

    void onReceiveMethodCall(const DBus::Message::MethodCall& method);
    void onReceiveMethodReturn(const Message::MethodReturn& reply);
    void onReceiveError(const Message::Error& error);
    void onReceiveSignal(const Message::Signal& signal);

    // Message Bus Messages (see dbus_native_messages.cpp for implementation)
    // Q. Move elsewhere? (because of the myriad flag definitions these need)
    void callHello(const Message::CallbackFunctionMethodReturn& success, const Message::CallbackFunctionError& failure);
    void callRequestName(const std::string& name, uint32_t flags, const Message::CallbackFunctionMethodReturn& success, const Message::CallbackFunctionError& failure);
    void callReleaseName(const std::string& name, const Message::CallbackFunctionMethodReturn& success, const Message::CallbackFunctionError& failure);
    void callListQueuedOwners(const std::string& bus_name, const Message::CallbackFunctionMethodReturn& success, const Message::CallbackFunctionError& failure);
    void callListNames(const DBus::Message::CallbackFunctionMethodReturn& success, const Message::CallbackFunctionError& failure);
    void callListActivatableNames(const DBus::Message::CallbackFunctionMethodReturn& success, const Message::CallbackFunctionError& failure);
    void callNameHasOwner(const std::string& name, const Message::CallbackFunctionMethodReturn& success, const Message::CallbackFunctionError& failure);
    void callAddMatch(const std::string& rule, const DBus::Message::CallbackFunctionMethodReturn& success, const Message::CallbackFunctionError& failure, const Message::CallbackFunctionSignal& handler);
    void callRemoveMatch(const std::string& rule);

    std::string getStats() const;

    enum {
        DBUS_NAME_FLAG_ALLOW_REPLACEMENT = 0x01,
        DBUS_NAME_FLAG_REPLACE_EXISTING = 0x02,
        DBUS_NAME_FLAG_DO_NOT_QUEUE = 0x04
    } RequestNameFlagsIn;

    enum {
        DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER = 0x01,
        DBUS_REQUEST_NAME_REPLY_IN_QUEUE = 0x02,
        DBUS_REQUEST_NAME_REPLY_EXISTS = 0x03,
        DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER = 0x04
    } RequestNameReplyOut;

    enum {
        DBUS_RELEASE_NAME_REPLY_RELEASED = 0x01,
        DBUS_RELEASE_NAME_REPLY_NON_EXISTENT = 0x02,
        DBUS_RELEASE_NAME_REPLY_NOT_OWNER = 0x03
    } ReleaseNameReplyOut;

private:
    Transport m_Transport;
    AuthenticationProtocol m_AuthenticationProtocol;
    MessageProtocol m_MessageProtocol;

    struct CallbackPair {
        CallbackPair(const Message::CallbackFunctionMethodReturn& s, const Message::CallbackFunctionError& f)
            : success(s)
            , failure(f)
        {
        }

        Message::CallbackFunctionMethodReturn success;
        Message::CallbackFunctionError failure;
    };

    struct Stats {
        size_t bytes_auth;
        size_t bytes_message;
        size_t count_sent_methodcalls;
        size_t count_sent_methodreturns;
        size_t count_sent_errors;
        size_t count_sent_signals;
        size_t count_receive_methodcalls;
        size_t count_receive_methodreturns;
        size_t count_receive_errors;
        size_t count_receive_signals;
    } m_Stats;

    mutable boost::recursive_mutex m_MessageQueueMutex;
    std::map<uint32_t, CallbackPair> m_MessageQueue;

    mutable boost::recursive_mutex m_MethodCallMapMutex;
    std::map<std::string, Message::CallbackFunctionMethodCall> m_MethodCallMap;

    mutable boost::recursive_mutex m_SignalMapMutex;
    std::map<std::string, Message::CallbackFunctionSignal> m_SignalMap;

    mutable boost::recursive_mutex m_RulesMapMutex;
    std::map<std::string, MatchRule> m_RulesMap;

public:
    static std::string DBusDaemon;
};
}

#endif
