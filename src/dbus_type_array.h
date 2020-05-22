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
class UnmarshallingData;
class MessageOStream;

namespace Type {
    class Struct;
    class DictEntry;

    class Array : public Base {
    public:
        size_t size() const;

        size_t add(const DBus::Type::Struct& s);
        size_t add(const DBus::Type::DictEntry& s);

        size_t getAlignment() const { return 4; }
        void marshall(MessageOStream& stream) const;
        void marshallContents(MessageOStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string getSignature() const;

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        const std::vector<DBus::Type::Generic>& getContents() const;

        static const std::string s_StaticTypeCode;

    protected:
        std::vector<DBus::Type::Generic> contents;

        struct Unmarshalling {
            Unmarshalling()
                : areWeSkippingPaddingForSize(true)
                , areWeSkippingPaddingForElement(true)
                , count(0)
                , array_size(0)
            {
            }

            bool areWeSkippingPaddingForSize;
            bool areWeSkippingPaddingForElement;
            size_t count;
            size_t array_size;
            bool createType;
            std::string typeSignature;
        } m_Unmarshalling;
    };
}
}

#endif
