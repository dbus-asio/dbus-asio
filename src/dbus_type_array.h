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

#ifndef DBUS_TYPE_ARRAY_H
#define DBUS_TYPE_ARRAY_H

#include "dbus_type_base.h"

namespace DBus {
class MessageOStream;
class MessageIStream;

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
    class Signature;
    class Variant;
    class Struct;
    class DictEntry;

    class Array : public Base {
    public:
        size_t size() const;

        size_t add(const DBus::Type::Byte& v);
        size_t add(const DBus::Type::Boolean& v);
        size_t add(const DBus::Type::ObjectPath& v);
        size_t add(const DBus::Type::Int16& v);
        size_t add(const DBus::Type::Uint16& v);
        size_t add(const DBus::Type::Int32& v);
        size_t add(const DBus::Type::Uint32& v);
        size_t add(const DBus::Type::Int64& v);
        size_t add(const DBus::Type::Uint64& v);
        size_t add(const DBus::Type::Double& v);
        size_t add(const DBus::Type::String& v);
        size_t add(const DBus::Type::Variant& v);
        size_t add(const DBus::Type::Signature& v);
        size_t add(const DBus::Type::Struct& s);
        size_t add(const DBus::Type::DictEntry& s);

        size_t getAlignment() const { return 4; }
        void marshall(MessageOStream& stream) const;
        void unmarshall(MessageIStream& stream);
        void marshallContents(MessageOStream& stream) const;

        std::string getSignature() const;

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        const std::vector<DBus::Type::Generic>& getContents() const;

        static const std::string s_StaticTypeCode;

    protected:
        std::vector<DBus::Type::Generic> contents;
    };
} // namespace Type
} // namespace DBus

#endif
