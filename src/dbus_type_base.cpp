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

#include "dbus_type_base.h"

void DBus::Type::Base::setSignature(const std::string& type)
{
    m_Signature = type;
}

std::string DBus::Type::Base::getSignature() const { return m_Signature; }

std::string DBus::Type::Base::toString(const std::string& prefix) const
{
    return std::string(prefix + "\n");
}

std::string DBus::Type::Base::asString() const { return ""; }
