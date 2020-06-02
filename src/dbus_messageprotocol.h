// This file is part of dbus-asio
// Copyright 2018 Brightsign LLC
//
// This library is free software: you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation, version 3, or at your
// option any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// The GNU Lesser General Public License version 3 is included in the
// file named COPYING. If you do not have this file see
// <http://www.gnu.org/licenses/>.

#ifndef DBUS_MESSAGEPROTOCOL
#define DBUS_MESSAGEPROTOCOL

#include "dbus_type_struct.h"
#include "dbus_message.h"
#include "dbus_octetbuffer.h"

namespace DBus {



class MessageProtocol {
public:
    MessageProtocol();

    void reset();
    void setMethodCallHandler(const Message::CallbackFunctionMethodCall& callback);
    void setMethodReturnHandler(const Message::CallbackFunctionMethodReturn& callback);
    void setErrorHandler(const Message::CallbackFunctionError& callback);
    void setSignalHandler(const Message::CallbackFunctionSignal& callback);

    void onReceiveData(OctetBuffer& buffer);
    void onBodyComplete(const std::string& body);

private:
    enum {
        STATE_GETHEADERSIZE,
        STATE_UNMARSHALLHEADER,
        STATE_GETBODY
    };

    enum {
        TYPE_METHOD = 1,
        TYPE_METHOD_RETURN = 2,
        TYPE_ERROR = 3,
        TYPE_SIGNAL = 4
    };

    size_t m_State;
    size_t m_headerSize;
    size_t m_bodySize;
    // TODO:?? Move the handler into a separate message class? We can't get 2 interspersed message so it's a
    // 1:1 relationship between protocol handler and its message, but it might be useful elsewhere.
    DBus::Type::Struct m_HeaderStruct;
    std::basic_string<uint8_t> m_octetCache;

    //
    Message::CallbackFunctionMethodCall m_MethodCallCallback;
    Message::CallbackFunctionMethodReturn m_MethodReturnCallback;
    Message::CallbackFunctionError m_ErrorCallback;
    Message::CallbackFunctionSignal m_SignalCallback;

    void startMessage();

    bool getHeaderSize(OctetBuffer& buffer);
    bool unmarshallHeader(OctetBuffer& buffer);
    bool getBody(OctetBuffer& buffer);
    void processData(OctetBuffer& buffer);

    void onReceiveMethodCall(const DBus::Message::MethodCall& result) {}
    void onReceiveMethodReturn(const DBus::Message::MethodReturn& result) {}
    void onReceiveError(const DBus::Message::Error& result) {}
    void onReceiveSignal(const DBus::Message::Signal& result) {}
};
}

#endif //  DBUS_MESSAGEPROTOCOL
