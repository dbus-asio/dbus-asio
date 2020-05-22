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

#ifndef DBUS_TYPE_SIGNATURE_H
#define DBUS_TYPE_SIGNATURE_H

#include "dbus_type.h"

namespace DBus {
class UnmarshallingData;
class MessageOStream;

namespace Type {

    class Signature : public Base {
    public:
        Signature();
        Signature(const std::string& v);

        void marshall(MessageOStream& stream) const;
        bool unmarshall(const UnmarshallingData& data);

        std::string toString(const std::string& prefix = "") const;
        std::string asString() const;
        const std::string& getValue() const;

        static const std::string s_StaticTypeCode;

    protected:
        std::string m_Value;

        struct Unmarshalling {
            Unmarshalling()
                : count(0)
            {
            }

            size_t count;
            size_t signature_size;
        } m_Unmarshalling;
    };
}
}

#endif
