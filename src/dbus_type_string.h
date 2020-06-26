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

#ifndef DBUS_TYPE_STRING_H
#define DBUS_TYPE_STRING_H

#include "dbus_type_base.h"

namespace DBus {
class MessageOStream;
class MessageIStream;

namespace Type {

    class String : public Base {
    public:
        String();
        String(const std::string& v);

        std::string getSignature() const { return s_StaticTypeCode; }
        size_t getAlignment() const { return 4; }
        void marshall(MessageOStream& stream) const;
        void unmarshall(MessageIStream& stream);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;

        static const std::string s_StaticTypeCode;

    protected:
        std::string m_Value;
    };
} // namespace Type
} // namespace DBus

#endif
