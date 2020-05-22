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

#ifndef DBUS_MESSAGE
#define DBUS_MESSAGE

#include <functional>
#include <boost/thread/recursive_mutex.hpp>

namespace DBus {
class MessageStream;

namespace Message {

    class MethodCallIdentifier {
    public:
        MethodCallIdentifier(const std::string& object, const std::string& interface, const std::string& method)
            : m_Object(object)
            , m_Interface(interface)
            , m_Method(method)
        {
        }

        std::string m_Object;
        std::string m_Interface;
        std::string m_Method;
    };

    //

    class MethodCallParameters {
    public:
        std::string getMarshallingSignature() const;
        size_t getParameterCount() const;
        void marshallData(MessageStream& stream) const;

        const Type::Generic& getParameter(size_t idx) const;

    protected:
        Type::CompositeBlock m_Contents;
    };

    class MethodCallParametersIn : public MethodCallParameters {
    public:
        // We have a series of ctor's to permit the basic prototypes to be declared inline, with
        // MethodCallParametersIn("param") etc. Everyone else will need to use a temporary variable
        // and add()
        MethodCallParametersIn() {}
        MethodCallParametersIn(const Type::Generic& v);
        MethodCallParametersIn(const std::string& v);
        MethodCallParametersIn(const std::string& v1, uint32_t v2);

        void add(const Type::Generic& value);

        void add(uint8_t value);
        void add(uint32_t value);
        void add(const std::string& value);
    };

    class Header {
    public:
        typedef enum {
            TYPE_INVALID = 0,
            TYPE_METHOD_CALL = 1,
            TYPE_METHOD_RETURN = 2,
            TYPE_ERROR = 3,
            TYPE_SIGNAL = 4,
        } Type;

        typedef enum {
            FLAGS_NONE = 0,
            FLAGS_NO_REPLY_EXPECTED = 0x01,
            FLAGS_NO_AUTO_START = 0x02,
            FLAGS_ALLOW_INTERACTIVE_AUTHORIZATION = 0x04,
            //
            FLAGS_MASK = 0x07,
        } Flags;

        enum {
            HEADER_INVALID,
            HEADER_PATH, // The object to send a call to, or the object a signal is emitted from.
            HEADER_INTERFACE, // The interface to invoke a method call on, or that a signal is emitted from. Optional for method calls, required for signals.
            HEADER_MEMBER, // The member, either the method name or signal name.
            HEADER_ERROR_NAME, // The name of the error that occurred, for errors
            HEADER_REPLY_SERIAL, // The serial number of the message this message is a reply to.
            HEADER_DESTINATION, // The name of the connection this message is intended for.
            HEADER_SENDER, // Unique name of the sending connection.
            HEADER_SIGNATURE, // The signature of the message body. If omitted, it is assumed to be the empty signature "" (i.e. the body must be 0-length).
            HEADER_UNIX_FDS, // The number of Unix file descriptors that accompany the message.
            //
            HEADER_FIELD_COUNT // = HEADER_UNIX_FDS+1
        } HeaderFields;

        uint8_t flags;
        uint8_t type;
        uint32_t serial;

        std::string path;
        std::string interface;
        std::string member;
        std::string destination;
        std::string sender;
        //
        DBus::Type::Generic field[HEADER_FIELD_COUNT];
    };

    Type::Generic getHeaderField(const DBus::Type::Struct& header, size_t type);

    class Base {
    public:
        Base(uint32_t serial = 0);
        Base(const DBus::Type::Struct& header, const std::string& body);

        uint32_t getSerial() const
        {
            return m_Header.serial;
        }

        std::string getHeaderPath() const
        {
            return m_Header.path;
        }

        std::string getHeaderInterface() const
        {
            return m_Header.interface;
        }

        std::string getHeaderMember() const
        {
            return m_Header.member;
        }

        std::string getHeaderSender() const
        {
            return m_Header.sender;
        }

        std::string getHeaderDestination() const
        {
            return m_Header.destination;
        }

        Type::Generic getHeaderField(size_t type) const
        {
            return m_Header.field[type];
        }

        const Type::Generic& getParameter(size_t idx) const
        {
            return m_Parameters.getParameter(idx);
        }

        const size_t getParameterCount() const
        {
            return m_Parameters.getParameterCount();
        }

        const bool isReplyExpected() const
        {
            return !(m_Header.flags & DBus::Message::Header::FLAGS_NO_REPLY_EXPECTED) ? true : false;
        }

    protected:
        std::string marshallMessage(const DBus::Type::Array& header_fields) const;

        Header m_Header;
        MethodCallParametersIn m_Parameters;

    private:
        static uint32_t m_SerialCounter;
        static boost::recursive_mutex m_SerialCounterMutex;

        void parseParameters(bool isLittleEndian, const std::string& bodydata);
    };

    class MethodCall : public Message::Base {
    public:
        // This is for outgoing calls
        MethodCall(const MethodCallIdentifier& name, const MethodCallParametersIn& params = MethodCallParametersIn(), uint32_t flags = 0);
        // This is for receeiving method calls
        MethodCall(const DBus::Type::Struct& header, const std::string& body);

        std::string getFullName() const;
        std::string getObject() const;
        std::string getInterface() const;
        std::string getMethod() const;

        std::string marshall(const std::string& destination) const;

    private:
        MethodCallIdentifier m_Name;
    };

    class MethodReturn : public Message::Base {
    public:
        // This is for sending outgoing calls
        MethodReturn(uint32_t serial);
        // This is for receeiving method calls
        MethodReturn(const DBus::Type::Struct& header, const std::string& body);

        void addParameter(const Type::Generic& value);

        std::string marshall(const std::string& destination) const;

        uint32_t m_SerialReplyingTo;
    };

    class Error : public Message::Base {
    public:
        // This is for outgoing calls
        Error(uint32_t serial, const std::string& name, const std::string& message);
        // This is for receeiving method calls
        Error(const DBus::Type::Struct& header, const std::string& body);

        uint32_t getSerialOfReply() const;
        std::string getMessage() const;

        std::string marshall(const std::string& destination) const;

    private:
        uint32_t m_SerialReplyingTo; //NOTE: Means the query one, on rcvr side
        std::string m_Errorname; // e.g. biz.brightsign.Error.InvalidParameters
        std::string m_Message; // i.e. the user text to display
    };

    class Signal : public Base {
    public:
        // This is for outgoing calls
        Signal(const MethodCallIdentifier& name);
        // This is for receeiving method calls
        Signal(const DBus::Type::Struct& header, const std::string& body);

        void addParameter(const Type::Generic& value);

        std::string marshall(const std::string& destination) const;

    private:
        MethodCallIdentifier m_SignalName;
    };

    typedef std::function<void(const Message::MethodCall& method)> CallbackFunctionMethodCall;
    typedef std::function<void(const Message::MethodReturn& reply)> CallbackFunctionMethodReturn;
    typedef std::function<void(const Message::Error& error)> CallbackFunctionError;
    typedef std::function<void(const Message::Signal& signal)> CallbackFunctionSignal;
}
}

#endif //  DBUS_MESSAGE
