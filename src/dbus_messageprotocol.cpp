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

#include <boost/thread/recursive_mutex.hpp>

#include "dbus_log.h"
#include "dbus_utils.h"

#include "dbus_type.h"
#include "dbus_type_signature.h"
#include "dbus_type_struct.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"
#include "dbus_messageistream.h"
#include "dbus_transport.h"

#include <byteswap.h>

DBus::MessageProtocol::MessageProtocol()
{
    m_State = STATE_BUFFERINGHEADERSIZE;

    setMethodCallHandler(std::bind(&MessageProtocol::onReceiveMethodCall, this, std::placeholders::_1));
    setMethodReturnHandler(std::bind(&MessageProtocol::onReceiveMethodReturn, this, std::placeholders::_1));
    setErrorHandler(std::bind(&MessageProtocol::onReceiveError, this, std::placeholders::_1));
    setSignalHandler(std::bind(&MessageProtocol::onReceiveSignal, this, std::placeholders::_1));
    //
    reset();
}

void DBus::MessageProtocol::reset() { startMessage(); }

void DBus::MessageProtocol::setMethodCallHandler(const DBus::Message::CallbackFunctionMethodCall& callback) { m_MethodCallCallback = callback; }

void DBus::MessageProtocol::setMethodReturnHandler(const DBus::Message::CallbackFunctionMethodReturn& callback) { m_MethodReturnCallback = callback; }

void DBus::MessageProtocol::setErrorHandler(const DBus::Message::CallbackFunctionError& callback) { m_ErrorCallback = callback; }

void DBus::MessageProtocol::setSignalHandler(const DBus::Message::CallbackFunctionSignal& callback) { m_SignalCallback = callback; }

void DBus::MessageProtocol::startMessage()
{
    m_State = STATE_BUFFERINGHEADERSIZE;

    m_HeaderStruct.clear();
    m_HeaderStruct.setSignature("(yyyyuua(yv))");

    //m_BodyStream.clear();
    m_bufferSize = 0;
}

void DBus::MessageProtocol::processData(OctetBuffer& buffer)
{
    while (buffer.size()) {
        if (m_State == STATE_BUFFERINGHEADERSIZE) {
            if (buffer.size() < 16) {
                return;
            }

            m_bufferSize = *(uint32_t*)(buffer.data() + 12);
            if ((buffer[0] == 'l' && __BYTE_ORDER != __LITTLE_ENDIAN) ||
                (buffer[0] == 'B' && __BYTE_ORDER != __BIG_ENDIAN)) {
                m_bufferSize = bswap_32(m_bufferSize);
            }
            m_bufferSize+= 16; // Header bytes
            m_bufferSize+= (m_bufferSize % 8 == 0) ? 0 : 8 - (m_bufferSize % 8);
            m_State = STATE_BUFFERINGHEADER;
        }


        if (m_State == STATE_BUFFERINGHEADER) {
            if (buffer.size() < m_bufferSize) {
                return;
            }

            MessageIStream istream((uint8_t*)buffer.data(), m_bufferSize,
                (buffer[0] == 'l' && __BYTE_ORDER != __LITTLE_ENDIAN) ||
                (buffer[0] == 'B' && __BYTE_ORDER != __BIG_ENDIAN));
            m_HeaderStruct.setLittleEndian(buffer[0] == 'l'  ? true : false);
            m_HeaderStruct.unmarshall(istream);
            buffer.remove_prefix(m_bufferSize);
            m_bufferSize = Type::asUint32(m_HeaderStruct[4]);
            m_State = STATE_BODY;
        }

        if (m_State == STATE_BODY) {
            if (buffer.size() < m_bufferSize) {
                return;
            }

            std::string body((const char*)buffer.data(), m_bufferSize);
            buffer.remove_prefix(m_bufferSize);
            onBodyComplete(body);
        }
    }
}

void DBus::MessageProtocol::onReceiveData(OctetBuffer& buffer)
{
    if (m_octetCache.empty()) {
        processData(buffer);
    }

    m_octetCache.append(buffer.data(), buffer.size());
    buffer.remove_prefix(buffer.size());

    if (!m_octetCache.empty()) {
        OctetBuffer cachedBuffer(m_octetCache.data(), m_octetCache.size());
        processData(cachedBuffer);
        m_octetCache.erase(0,  m_octetCache.size() - cachedBuffer.size());
    }
}

void DBus::MessageProtocol::onBodyComplete(const std::string& body)
{
    Log::write(Log::TRACE, "DBus :: Unmarshall : Body complete.\n");

    uint8_t type = Type::asByte(m_HeaderStruct[1]);
    switch (type) {
    case TYPE_METHOD:
        m_MethodCallCallback(Message::MethodCall(m_HeaderStruct, body));
        break;

    case TYPE_METHOD_RETURN:
        m_MethodReturnCallback(Message::MethodReturn(m_HeaderStruct, body));
        break;

    case TYPE_ERROR:
        m_ErrorCallback(Message::Error(m_HeaderStruct, body));
        break;

    case TYPE_SIGNAL:
        m_SignalCallback(Message::Signal(m_HeaderStruct, body));
        break;

    default:
        Log::write(Log::WARNING, "DBus :: Unknown message type %d received\n", type);
    }

    Log::write(Log::TRACE, "DBus :: Message type %d dispatched : new message initialized\n", type);
    startMessage();
}
