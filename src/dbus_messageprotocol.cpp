#include <boost/thread/recursive_mutex.hpp>

#include "dbus_log.h"
#include "dbus_utils.h"

#include "dbus_type.h"
#include "dbus_type_signature.h"
#include "dbus_type_struct.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messagestream.h"

DBus::MessageProtocol::MessageProtocol(Transport& transport)
    : m_Transport(transport)
{
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
    m_State = STATE_ENDIAN;

    m_HeaderStruct.clear();
    m_HeaderStruct.setSignature("(yyyyuua(yv))");

    m_RawStream = "";
    m_BodyStream = "";
    m_UnmarshallingData.offset = 0;
}

bool DBus::MessageProtocol::onReceiveOctet(uint8_t c)
{

    m_RawStream += c;
    m_UnmarshallingData.c = c;

    switch (m_State) {
    case STATE_ENDIAN:
        if (c == 'l' || c == 'B') { // ignore invalid endian types
            Log::write(Log::TRACE, "DBus :: Unmarshall : Endian set to %s\n", c == 'l' ? "LITTLE" : "BIG");
            m_HeaderStruct.unmarshall(m_UnmarshallingData);
            m_HeaderStruct.setLittleEndian(c == 'l' ? true : false);
            m_State = STATE_HEADER;
        } else {
            Log::write(Log::WARNING, "DBus :: Unmarshall : Spurious endian byte received (%.2x)\n", c);
        }
        break;

    case STATE_HEADER:
        if (m_HeaderStruct.unmarshall(m_UnmarshallingData)) {
            uint32_t size = Type::asUint32(m_HeaderStruct[4]);

            Log::write(Log::TRACE, "DBus :: Unmarshall : Header complete for message type %d. Size of body: %d\n", Type::asByte(m_HeaderStruct[1]), size);

            // The current stream is already correctly aligned to move onto the body
            // (if there is one.)
            // So either complete the body immediately, if it doesn't exist, or skip directly
            // to the body parser if not. Jumping into the padding processor (STATE_HEADER_PADDING)
            // will cause onReceiveOctet to read spurious octets, pushing everything out of sync.
            if (Utils::isAlignedTo8(m_RawStream.size())) {
                if (size == 0) {
                    Log::write(Log::TRACE, "DBus :: Unmarshall : Body size is 0 (and raw stream of %d is neatly padded) so message is already complete.\n",
                        m_RawStream.size());
                    onBodyComplete();
                    return true;
                }
                m_State = STATE_BODY;
            } else {
                m_State = STATE_HEADER_PADDING;
            }
        }
        break;

    case STATE_HEADER_PADDING:
        if (Utils::isAlignedTo8(m_RawStream.size())) {
            uint32_t size = Type::asUint32(m_HeaderStruct[4]);
            Log::write(Log::TRACE, "DBus :: Unmarshall : Starting to read the body (of size %d  0x%.8x)\n", size, size);
            m_State = STATE_BODY;
            if (size == 0) {
                Log::write(Log::TRACE, "DBus :: Unmarshall : Body size is 0, so message complete.\n");
                onBodyComplete();
                return true;
            }
        } else {
            Log::write(Log::TRACE, "DBus :: Unmarshall : Skipping padding byte (of size %d  0x%.2x)\n", m_RawStream.size(), c);
        }
        break;

    case STATE_BODY:
        uint32_t size = Type::asUint32(m_HeaderStruct[4]);

        m_BodyStream += c;

        if (m_BodyStream.size() >= size) {
            Log::write(Log::TRACE, "DBus :: Unmarshall : All %d bytes of body now read.\n", size);
            onBodyComplete();
            return true;
        }
        break;
    }

    ++m_UnmarshallingData.offset;

    return false;
}

void DBus::MessageProtocol::onBodyComplete()
{
    Log::write(Log::TRACE, "DBus :: Unmarshall : Body complete.\n");
    Log::writeHex(Log::TRACE, "DBus :: Header data :", m_RawStream);
    Log::writeHex(Log::TRACE, "DBus :: Body data :", m_BodyStream);

    uint8_t type = Type::asByte(m_HeaderStruct[1]);
    switch (type) {
    case TYPE_METHOD:
        m_MethodCallCallback(Message::MethodCall(m_HeaderStruct, m_BodyStream));
        break;

    case TYPE_METHOD_RETURN:
        m_MethodReturnCallback(Message::MethodReturn(m_HeaderStruct, m_BodyStream));
        break;

    case TYPE_ERROR:
        m_ErrorCallback(Message::Error(m_HeaderStruct, m_BodyStream));
        break;

    case TYPE_SIGNAL:
        m_SignalCallback(Message::Signal(m_HeaderStruct, m_BodyStream));
        break;

    default:
        Log::write(Log::WARNING, "DBus :: Unknown message type %d received\n", type);
    }

    Log::write(Log::TRACE, "DBus :: Message type %d dispatched : new message initialized\n", type);
    startMessage();
}
