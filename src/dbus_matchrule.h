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

#ifndef DBUS_MATCHRULE
#define DBUS_MATCHRULE

#include "dbus_message.h"
#include <string>

namespace DBus {

class MatchRule {
public:
    MatchRule(const std::string& rule, const Message::CallbackFunctionSignal& handler);

    bool isMatched(const DBus::Message::Signal& signal);
    void invoke(const DBus::Message::Signal& signal);

protected:
    std::string type; // NOTE: Only signals are currentl used/supported
    std::string sender;
    std::string interface;
    std::string member;
    std::string path;
    std::string path_namespace;
    std::string destination;

    Message::CallbackFunctionSignal callback;
};
}

#endif //  DBUS_MATCHRULE
