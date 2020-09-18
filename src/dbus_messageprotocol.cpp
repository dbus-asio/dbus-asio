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
#include "dbus_messageistream.h"
#include "dbus_messageostream.h"
#include "dbus_messageprotocol.h"

#include <byteswap.h>

namespace {

inline bool SwapRequired(uint8_t c)
{
    return (c == 'l' && __BYTE_ORDER != __LITTLE_ENDIAN) || (c == 'B' && __BYTE_ORDER != __BIG_ENDIAN);
}

inline uint32_t CorrectEndianess(uint8_t c, uint32_t value)
{
    if (SwapRequired(c)) {
        return bswap_32(value);
    }
    return value;
}

static const size_t MAX_ARRAY_SIZE = 67108864;
static const size_t MAX_MESSAGE_SIZE = 134217728;

} // namespace

DBus::MessageProtocol::MessageProtocol()
    : m_State(STATE_GETHEADERSIZE)
    , m_headerSize(0)
    , m_bodySize(0)
{
    setMethodCallHandler(std::bind(&MessageProtocol::onReceiveMethodCall, this,
        std::placeholders::_1));
    setMethodReturnHandler(std::bind(&MessageProtocol::onReceiveMethodReturn,
        this, std::placeholders::_1));
    setErrorHandler(
        std::bind(&MessageProtocol::onReceiveError, this, std::placeholders::_1));
    setSignalHandler(std::bind(&MessageProtocol::onReceiveSignal, this,
        std::placeholders::_1));

    reset();
}

void DBus::MessageProtocol::reset() { startMessage(); }

void DBus::MessageProtocol::setMethodCallHandler(
    const DBus::Message::CallbackFunctionMethodCall& callback)
{
    m_MethodCallCallback = callback;
}

void DBus::MessageProtocol::setMethodReturnHandler(
    const DBus::Message::CallbackFunctionMethodReturn& callback)
{
    m_MethodReturnCallback = callback;
}

void DBus::MessageProtocol::setErrorHandler(
    const DBus::Message::CallbackFunctionError& callback)
{
    m_ErrorCallback = callback;
}

void DBus::MessageProtocol::setSignalHandler(
    const DBus::Message::CallbackFunctionSignal& callback)
{
    m_SignalCallback = callback;
}

void DBus::MessageProtocol::startMessage()
{
    m_State = STATE_GETHEADERSIZE;

    m_HeaderStruct.clear();
    m_HeaderStruct.setSignature("(yyyyuua(yv))");
    m_headerSize = 0;
    m_bodySize = 0;
}

bool DBus::MessageProtocol::getHeaderSize(OctetBuffer& buffer)
{
    // Initial header consists of byte, byte, byte, byte, uint32_t, uint32_t
    // the next element is the size of the array of field info data
    // making a total of 16 bytes
    if (buffer.size() >= 16) {
        // Read the size of the array
        m_headerSize = *(uint32_t*)(buffer.data() + 12);
        m_headerSize = CorrectEndianess(buffer[0], m_headerSize);
        if (m_headerSize > MAX_ARRAY_SIZE) {
            throw std::out_of_range("DBus message error: Maximum size exceeded");
        }

        // Add the 16 bytes of the header
        m_headerSize += 16;
        // The header MUST finish on an 8 byte boundary
        m_headerSize += (m_headerSize % 8 == 0) ? 0 : 8 - (m_headerSize % 8);

        m_State = STATE_UNMARSHALLHEADER;
        return true;
    }
    return false;
}

bool DBus::MessageProtocol::unmarshallHeader(OctetBuffer& buffer)
{
    if (buffer.size() >= m_headerSize) {
        // When all of the header is buffered the header can be unmarshalled
        MessageIStream istream(buffer.data(), m_headerSize,
            SwapRequired(buffer[0]));
        m_HeaderStruct.unmarshall(istream);
        buffer.remove_prefix(m_headerSize);
        // The body of the message is next
        m_bodySize = Type::asUint32(m_HeaderStruct[4]);

        if ((m_headerSize + m_bodySize) > MAX_MESSAGE_SIZE) {
            throw std::out_of_range("DBus message error: Maximum size exceeded");
        }

        m_State = STATE_GETBODY;
        return true;
    }
    return false;
}

bool DBus::MessageProtocol::getBody(OctetBuffer& buffer)
{
    if (buffer.size() >= m_bodySize) {
        std::string body((const char*)buffer.data(), m_bodySize);
        buffer.remove_prefix(m_bodySize);
        onBodyComplete(body);
        return true;
    }
    return false;
}

void DBus::MessageProtocol::processData(OctetBuffer& buffer)
{
    while (buffer.size()) {
        if (m_State == STATE_GETHEADERSIZE) {
            if (!getHeaderSize(buffer)) {
                return;
            }
        }

        if (m_State == STATE_UNMARSHALLHEADER) {
            if (!unmarshallHeader(buffer)) {
                return;
            }
        }

        if (m_State == STATE_GETBODY) {
            if (!getBody(buffer)) {
                return;
            }
        }
    }
}

void DBus::MessageProtocol::onReceiveData(OctetBuffer& buffer)
{
    // If a whole message is contained in the buffer
    // it can be processed without copying but only if
    // there is no data already cached
    if (m_octetCache.empty()) {
        processData(buffer);
    }

    // Append any remaining data to the data cache
    m_octetCache.append(buffer.data(), buffer.size());
    buffer.remove_prefix(buffer.size());

    // If the data cache is not empty process it
    if (!m_octetCache.empty()) {
        OctetBuffer cachedBuffer(m_octetCache.data(), m_octetCache.size());
        processData(cachedBuffer);
        m_octetCache.erase(0, m_octetCache.size() - cachedBuffer.size());
    }
}

void DBus::MessageProtocol::onBodyComplete(const std::string& body)
{
    Log::write(Log::TRACE, "DBus :: Unmarshall : Body complete.\n");

    try
    {
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
            Log::write(Log::WARNING, "DBus :: Unknown message type %d received\n",
                type);
        }
        Log::write(Log::TRACE,
            "DBus :: Message type %d dispatched : new message initialized\n",
            type);
    }
    catch(const std::exception & e)
    {
        // Catch exceptions from std::function such as bad_function_call. No action required.
        Log::write(Log::INFO, "DBus :: Exception caught in onBodyComplete: %s\n", e.what());
    }

    startMessage();
}
