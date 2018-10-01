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

#ifndef DBUS_TYPE_STRUCT_H
#define DBUS_TYPE_STRUCT_H

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {
    class Byte;
    class Boolean;
    class ObjectPath;
    class Int16;
    class Uint16;
    class Int32;
    class Uint32;
    class Int64;
    class Uint64;
    class Double;
    class String;
    class Variant;

    class Struct : public Base {
    public:
        Struct();

        void clear();

        void add(const DBus::Type::Byte& v);
        void add(const DBus::Type::Boolean& v);
        void add(const DBus::Type::ObjectPath& v);
        void add(const DBus::Type::Int16& v);
        void add(const DBus::Type::Uint16& v);
        void add(const DBus::Type::Int32& v);
        void add(const DBus::Type::Uint32& v);
        void add(const DBus::Type::Int64& v);
        void add(const DBus::Type::Uint64& v);
        void add(const DBus::Type::Double& v);
        void add(const DBus::Type::String& v);
        void add(const DBus::Type::Variant& v);
        void add(const DBus::Type::Signature& v);

        std::string getSignature() const
        {
            // if no entries???
            return "(" + DBus::Type::getMarshallingSignature(m_Value) + ")";
        }

        size_t getAlignment() const { return 8; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        size_t getEntries() const { return m_Value.size(); }
        const Generic& operator[](std::size_t idx) const { return m_Value[idx]; }

        static std::string s_StaticTypeCode;

    private:
        std::vector<Generic> m_Value;

        struct Unmarshalling {
            Unmarshalling()
            {
                reset();
            }
            // This type needs 'reset' because it gets reused (as opposed to newly constructed)
            // when we parse the incoming data.
            void reset()
            {
                areWeSkippingPadding = true;
                signatureIndex = 1; // because the first character is '(' in struct
                createNewType = true;
                signature = "";
            }
            bool areWeSkippingPadding;
            size_t signatureIndex;
            bool createNewType;
            std::string signature;
        } m_Unmarshalling;
    };
}
}

#endif
