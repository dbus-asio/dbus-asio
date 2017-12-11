#ifndef DBUS_MATCHRULE
#define DBUS_MATCHRULE

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
