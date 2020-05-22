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

#ifndef DBUS_TYPE_DICTENTRY_H
#define DBUS_TYPE_DICTENTRY_H

#include "dbus_type_base.h"

namespace DBus {

class MessageStream;
class UnmarshallingData;

namespace Type {

    class DictEntry : public Base {
    public:
        DictEntry() {}
        DictEntry(const DBus::Type::Generic& key, const DBus::Type::Generic& value);
        DictEntry(const std::string& key, std::string& value);
        DictEntry(const std::string& key, uint32_t value);

        std::string getSignature() const;
        size_t getAlignment() const { return 8; }
        void marshall(MessageStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        void set(const DBus::Type::Generic& key, const DBus::Type::Generic& value);

        static const std::string s_StaticTypeCode;

    protected:
        std::pair<Generic, Generic> m_Value;

        struct Unmarshalling {
            Unmarshalling()
                : areWeSkippingPadding(true)
                , onKeyType(true)
                , count(0)
            {
            }

            bool areWeSkippingPadding;
            bool onKeyType;
            size_t count;
        } m_Unmarshalling;
    };
}
}

#endif
