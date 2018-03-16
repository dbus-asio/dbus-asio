#ifndef DBUS_MESSAGEPROTOCOL
#define DBUS_MESSAGEPROTOCOL

#include "dbus_type_struct.h"

namespace DBus {

class UnmarshallingData {
public:
    uint8_t c;
    size_t offset;

    UnmarshallingData()
        : offset(0)
    {
    }
};

class MessageProtocol {
public:
    MessageProtocol();

    void reset();
    void setMethodCallHandler(const Message::CallbackFunctionMethodCall& callback);
    void setMethodReturnHandler(const Message::CallbackFunctionMethodReturn& callback);
    void setErrorHandler(const Message::CallbackFunctionError& callback);
    void setSignalHandler(const Message::CallbackFunctionSignal& callback);

    bool onReceiveOctet(uint8_t c);
    void onBodyComplete();

private:
    enum {
        STATE_ENDIAN,
        STATE_HEADER,
        STATE_HEADER_PADDING,
        STATE_BODY
    };

    enum {
        TYPE_METHOD = 1,
        TYPE_METHOD_RETURN = 2,
        TYPE_ERROR = 3,
        TYPE_SIGNAL = 4
    };

    size_t m_State;
    // TODO:?? Move the handler into a separate message class? We can't get 2 interspersed message so it's a
    // 1:1 relationship between protocol handler and its message, but it might be useful elsewhere.
    DBus::Type::Struct m_HeaderStruct;

    UnmarshallingData m_UnmarshallingData;
    std::string m_RawStream;
    std::string m_BodyStream;
    //
    Message::CallbackFunctionMethodCall m_MethodCallCallback;
    Message::CallbackFunctionMethodReturn m_MethodReturnCallback;
    Message::CallbackFunctionError m_ErrorCallback;
    Message::CallbackFunctionSignal m_SignalCallback;

    void startMessage();

    void onReceiveMethodCall(const DBus::Message::MethodCall& result) {}
    void onReceiveMethodReturn(const DBus::Message::MethodReturn& result) {}
    void onReceiveError(const DBus::Message::Error& result) {}
    void onReceiveSignal(const DBus::Message::Signal& result) {}
};
}

#endif //  DBUS_MESSAGEPROTOCOL
