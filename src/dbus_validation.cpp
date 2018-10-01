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

#include <iomanip>
#include <regex>
#include <sstream>
#include <stdarg.h>
#include <string>

#include "dbus_validation.h"

#define DBUS_MAXIMUM_MATCH_RULE_LENGTH 1024
#define DBUS_MAXIMUM_NAME_LENGTH 255

// NOTE: The validation checks here are quick and dirty checks, and not parses. This is for the performance
// reasons. This is most notable in the checks for composite types, and a({) would be considered valid, since
// the (parentheses) match, but the {braces dont.

//
// Exception versions
//
void DBus::Validation::throwOnInvalidBoolean(uint32_t value)
{
    if (value != 0 && value != 1) {
        throw std::runtime_error(std::string("Invalid boolean : ") + std::to_string(value));
    }
}

void DBus::Validation::throwOnInvalidRule(const std::string& rule)
{
    if (!(isValidRule(rule))) {
        throw std::runtime_error(std::string("Invalid rule : ") + rule);
    }
}

void DBus::Validation::throwOnInvalidSignature(const std::string& signature)
{
    if (!(isValidSignature(signature))) {
        throw std::runtime_error(std::string("Invalid signature : ") + signature);
    }
}

void DBus::Validation::throwOnInvalidObjectPath(const std::string& path)
{
    if (!(isValidObjectPath(path))) {
        throw std::runtime_error(std::string("Invalid object path : ") + path);
    }
}

void DBus::Validation::throwOnInvalidBasicType(const std::string& type)
{
    if (!(isValidBasicType(type))) {
        throw std::runtime_error(std::string("Invalid basic type : ") + type);
    }
}

void DBus::Validation::throwOnInvalidSingleCompleteType(const std::string& type)
{
    if (!(isValidSingleCompleteType(type))) {
        throw std::runtime_error(std::string("Invalid complete type : ") + type);
    }
}

void DBus::Validation::throwOnInvalidContainerType(const std::string& type)
{
    if (!(isValidContainerType(type))) {
        throw std::runtime_error(std::string("Invalid container type : ") + type);
    }
}

void DBus::Validation::throwOnInvalidInterfaceName(const std::string& name)
{
    if (!(isValidInterfaceName(name))) {
        throw std::runtime_error(std::string("Invalid interface name : ") + name);
    }
}

void DBus::Validation::throwOnInvalidBusName(const std::string& name)
{
    if (!(isValidBusName(name))) {
        throw std::runtime_error(std::string("Invalid bus name : ") + name);
    }
}

void DBus::Validation::throwOnInvalidMethodName(const std::string& name)
{
    if (!(isValidMethodName(name))) {
        throw std::runtime_error(std::string("Invalid method name : ") + name);
    }
}

void DBus::Validation::throwOnInvalidErrorName(const std::string& name)
{
    if (!(isValidErrorName(name))) {
        throw std::runtime_error(std::string("Invalid error name : ") + name);
    }
}

//
// Logical versions
//
bool DBus::Validation::isValidRule(const std::string& rule) { return rule.size() < DBUS_MAXIMUM_MATCH_RULE_LENGTH; }

bool DBus::Validation::isValidSignature(const std::string& signature)
{
    // TODO: This validation is incomplete. It should contain Zero or more single complete types
    // (if the signature is 'a(', for example, this should fail.) Implementation of this would
    // require a genuine parser.
    return signature.size() < 256;
}

bool DBus::Validation::isValidObjectPath(const std::string& path)
{
    // https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-marshaling-object-path

    // "The path must begin with an ASCII '/' (integer 47) character, and must consist of elements separated by slash characters."
    if (path.at(0) != '/') {
        return false;
    }

    // NOTE: Although the spec suggests we should consider each element separately (i.e. the text between
    // each slash) we can simulate the same checks by allowing the slash in the original regex and
    // checking for the // case afterwards

    // "Each element must only contain the ASCII characters "[A-Z][a-z][0-9]_" "
    if (!std::regex_match(path, std::regex("^[A-Za-z0-9_\\/]+$"))) {
        return false;
    }

    // "No element may be the empty string."
    // "Multiple '/' characters cannot occur in sequence."
    // (These are equivalent in our implementation)
    if (std::regex_match(path, std::regex("\\/\\/"))) {
        return false;
    }

    // "A trailing '/' character is not allowed unless the path is the root path (a single '/' character)."
    if (path.size() > 1 && path.back() == '/') {
        return false;
    }

    return true;
}

bool DBus::Validation::isValidSingleCompleteType(const std::string& type)
{
    // https://dbus.freedesktop.org/doc/dbus-specification.html#basic-types

    // "A single complete type is a sequence of type codes that fully describes one type: either a basic type,
    // or a single fully-described container type."
    if (isValidBasicType(type)) {
        return true;
    } else if (isValidContainerType(type)) {
        return true;
    }

    return false;
}

bool DBus::Validation::isValidBasicType(const std::string& type)
{
    // https://dbus.freedesktop.org/doc/dbus-specification.html#basic-types

    // "Basic types consist of fixed types and string-like types."
    return isValidFixedType(type) || isValidStringLikeType(type);
}

bool DBus::Validation::isValidFixedType(const std::string& type)
{
    if (!std::regex_match(type, std::regex("^[ybnqiuxtdh]$"))) {
        return false;
    }

    return true;
}

bool DBus::Validation::isValidStringLikeType(const std::string& type)
{
    if (!std::regex_match(type, std::regex("^[sog]$"))) {
        return false;
    }
    return true;
}

bool DBus::Validation::isValidContainerType(const std::string& type)
{
    // https://dbus.freedesktop.org/doc/dbus-specification.html#container-types

    // There are four container types: STRUCT, ARRAY, VARIANT, and DICT_ENTRY.
    auto matching = [](char bracket_open, char bracket_close, const std::string& name) {
        if (name.at(0) != bracket_open) {
            return false;
        }
        size_t bracket_count = 0;
        for (size_t i = 0; i < name.size(); ++i) {
            if (name.at(i) == bracket_open) {
                ++bracket_count;
            } else if (name.at(i) == bracket_close) {
                if (--bracket_count == 0) {
                    return i == name.size() - 1;
                }
            }
        }
        return false;
    };

    // Struct
    // "Empty structures are not allowed; there must be at least one type code between the parentheses."
    if (matching('(', ')', type)) {
        return type.size() > 2 ? true : false;
    }

    // Array
    // "ARRAY has ASCII character 'a' as type code. The array type code must be followed by a single complete type"
    if (type.at(0) == 'a' && isValidContainerType(type.substr(1))) {
        return true;
    }

    // VARIANT has ASCII character 'v' as its type code.
    // "A marshaled value of type VARIANT will have the signature of a single complete type as part of the value.
    // This signature will be followed by a marshaled value of that type."
    if (type == "v") {
        return true;
    }

    // Dictionary entry
    if (matching('{', '}', type)) {
        return true;
    }

    return false;
}

bool DBus::Validation::isValidInterfaceName(const std::string& name)
{
    // https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-names-interface

    // "Each element must only contain the ASCII characters "[A-Z][a-z][0-9]_" and must not begin with a digit."
    if (!std::regex_match(name, std::regex("^[A-Za-z0-9_\\.]+$"))) {
        return false;
    } else if (::isdigit(name.at(0))) {
        return false;
    }

    // "Interface names must contain at least one '.' (period) character (and thus at least two elements)."
    if (name.find('.') == std::string::npos) {
        return false;
    }

    // "Interface names must not begin with a '.' (period) character."
    if (name.at(0) == '.') {
        return false;
    }

    // "Interface names must not exceed the maximum name length."
    if (name.size() > DBUS_MAXIMUM_NAME_LENGTH) {
        return false;
    }

    return true;
}

bool DBus::Validation::isValidBusName(const std::string& name)
{
    // https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-names-bus
    bool is_unique_connection_name = name.at(0) == ':';

    // "Each element must only contain the ASCII characters "[A-Z][a-z][0-9]_-".  "
    if (is_unique_connection_name) {
        if (!std::regex_match(name.substr(1), std::regex("^[A-Za-z0-9_\\.\\-]+$"))) {
            return false;
        }
    } else if (!std::regex_match(name, std::regex("^[A-Za-z0-9_\\.\\-]+$"))) {
        return false;
    }

    // "Only elements that are part of a unique connection name may begin with a digit,
    // elements in other bus names must not begin with a digit."
    // (Note: )the logic of is_unique_connection_name is moot, but included to demonstrate the point)
    if (::isdigit(name.at(0)) && !is_unique_connection_name) {
        return false;
    }

    // "Bus names must contain at least one '.' (period) character (and thus at least two elements)."
    if (name.find('.') == std::string::npos) {
        return false;
    }

    // "Bus names must not begin with a '.' (period) character."
    if (name.at(0) == '.') {
        return false;
    }

    // "Bus names must not exceed the maximum name length."
    if (name.size() > DBUS_MAXIMUM_NAME_LENGTH) {
        return false;
    }

    return true;
}

bool DBus::Validation::isValidMethodName(const std::string& name)
{
    // https://dbus.freedesktop.org/doc/dbus-specification.html#message-protocol-names-member

    // "Must be at least 1 byte in length."
    // "Must not exceed the maximum name length."
    if (name.size() == 0 || name.size() > DBUS_MAXIMUM_NAME_LENGTH) {
        return false;
    }

    // "Must only contain the ASCII characters "[A-Z][a-z][0-9]_" and may not begin with a digit."
    if (!std::regex_match(name, std::regex("^[A-Za-z0-9_]+$"))) {
        return false;
    }

    // "Must not contain the '.' (period) character."
    if (name.find('.') != std::string::npos) {
        return false;
    }

    return true;
}

bool DBus::Validation::isValidErrorName(const std::string& name)
{
    // https://dbus.freedesktop.org/doc/dbus-specification.html#mmessage-protocol-names-error
    // "Error names have the same restrictions as interface names."
    return isValidInterfaceName(name);
}
