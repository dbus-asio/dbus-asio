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

#ifndef DBUS_VALIDATION_H
#define DBUS_VALIDATION_H

namespace DBus {
namespace Validation {
    void throwOnInvalidRule(const std::string& rule);

    void throwOnInvalidBoolean(uint32_t value);
    void throwOnInvalidSignature(const std::string& signature);
    void throwOnInvalidObjectPath(const std::string& path);
    void throwOnInvalidBasicType(const std::string& type);
    void throwOnInvalidSingleCompleteType(const std::string& type);
    void throwOnInvalidContainerType(const std::string& type);
    void throwOnInvalidInterfaceName(const std::string& name);
    void throwOnInvalidBusName(const std::string& name);
    void throwOnInvalidMethodName(const std::string& name);
    void throwOnInvalidErrorName(const std::string& name);

    bool isValidRule(const std::string& rule);
    bool isValidFixedType(const std::string& type);
    bool isValidStringLikeType(const std::string& type);
    bool isValidSignature(const std::string& signature);
    bool isValidObjectPath(const std::string& path);
    bool isValidBasicType(const std::string& type);
    bool isValidSingleCompleteType(const std::string& type);
    bool isValidContainerType(const std::string& type);
    bool isValidInterfaceName(const std::string& name);
    bool isValidBusName(const std::string& name);
    bool isValidMethodName(const std::string& name);
    bool isValidErrorName(const std::string& name);
}
}

#endif // DBUS_VALIDATION_H
