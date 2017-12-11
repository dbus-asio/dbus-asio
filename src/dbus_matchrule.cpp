#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/tokenizer.hpp>

#include "dbus_log.h"
#include "dbus_type.h"

#include "dbus_matchrule.h"
#include "dbus_message.h"

// https://dbus.freedesktop.org/doc/dbus-specification.html#message-bus-routing-match-rules

DBus::MatchRule::MatchRule(const std::string& rule, const Message::CallbackFunctionSignal& handler)
{
    // Parse the rule string into something programmatic

    // 1. Split by the comma
    boost::char_separator<char> separator{ "," };
    boost::tokenizer<boost::char_separator<char> > tokenizer{ rule, separator };
    for (auto&& param : tokenizer) {
        // 2. Then split each at the equals
        std::vector<std::string> keyvalue;
        boost::split(keyvalue, param, boost::is_any_of("="));

        // Valid keys are: type, sender, interface, member, path, path_namespace, destination, arg*, eavesdrop
        // We're happy for [] to throw on malformed rules, since that's watch we'd do anyway in this case

        // 3. Strip the single quotes from the value
        keyvalue[1].pop_back();
        keyvalue[1].erase(0, 1);

        // We store it somewhere convenient to speed the isMateched checks
        if (keyvalue[0] == "type") {
            type = keyvalue[1];
        } else if (keyvalue[0] == "sender") {
            sender = keyvalue[1];
        } else if (keyvalue[0] == "interface") {
            interface = keyvalue[1];
        } else if (keyvalue[0] == "member") {
            member = keyvalue[1];
        } else if (keyvalue[0] == "path") {
            path = keyvalue[1];
        } else if (keyvalue[0] == "path_namespace") {
            path_namespace = keyvalue[1];
        } else if (keyvalue[0] == "destination") {
            destination = keyvalue[1];
        }

        // TODO: eavesdrop
        // TODO: arg
    }

    // Using both path and path_namespace in the same match rule is not allowed.
    if (path != "" && path_namespace != "") {
        throw std::runtime_error("Match rules with both 'path' and 'path_namespace' are not allowed.");
    }

    // Finally, remember the callback
    callback = handler;
}

bool DBus::MatchRule::isMatched(const DBus::Message::Signal& signal)
{
    // These checks are AND, so all set parameters must agree
    if (sender != "" && sender != signal.getHeaderSender()) {
        return false;
    }

    if (interface != "" && interface != signal.getHeaderInterface()) {
        return false;
    }

    if (member != "" && member != signal.getHeaderMember()) {
        return false;
    }

    if (destination != "" && destination != signal.getHeaderDestination()) {
        return false;
    }

    std::string signal_path(signal.getHeaderPath());
    if (path != "" && path != signal_path) {
        return false;
    }

    // For example, path_namespace='/com/example/foo' would match signals sent
    // by /com/example/foo or by /com/example/foo/bar, but not by /com/example/foobar.
    if (path_namespace != "") {
        if (signal_path.find(path_namespace) != 0) {
            return false;
        }

        // This handles the 3rd case, by checking the character _after_ the match is found:
        // if it's not the terminator or another path (i.e. a new /) the search fails.
        auto next_char = signal_path.at(path_namespace.size());
        if (next_char != '\0' && next_char != '/') {
            return false;
        }
    }

    // TODO: eavesdrop
    // TODO: arg

    return true;
}

void DBus::MatchRule::invoke(const DBus::Message::Signal& signal) { callback(signal); }
