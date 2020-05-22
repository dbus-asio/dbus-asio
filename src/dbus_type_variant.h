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

#ifndef DBUS_TYPE_VARIANT_H
#define DBUS_TYPE_VARIANT_H

#include "dbus_type_signature.h"

namespace DBus {
class UnmarshallingData;
class MessageStream;

namespace Type {
    class ObjectPath;
    class String;
    class Uint32;

    class Variant : public Base {
    public:
        Variant();
        Variant(const DBus::Type::ObjectPath& v);
        Variant(const DBus::Type::String& v);
        Variant(const DBus::Type::Uint32& v);

        //std::string getSignature() const { return s_StaticTypeCode; }

        size_t getAlignment() const { return 8; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;
        const DBus::Type::Generic& getValue() const; // please use sparingly - this is provided as a courtesy to the Message::getHeaderField method

        static const std::string s_StaticTypeCode;

    private:
        DBus::Type::Generic m_Value;

        struct Unmarshalling {
            Unmarshalling()
                : isReadingSignature(true)
            {
            }

            bool isReadingSignature;
            DBus::Type::Signature signature;
        } m_Unmarshalling;
    };
}
}

#endif
