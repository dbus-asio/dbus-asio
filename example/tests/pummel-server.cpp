#include "dbus.h"
#include <iostream>

bool gInterrupted = false;

void s_signal_handler(int signal_value)
{
    gInterrupted = true;
}

void s_catch_signals(void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
}

void testServer()
{
    DBus::Log::setLevel(DBus::Log::WARNING);

    DBus::Native native(DBus::Platform::getSessionBus());
    std::this_thread::sleep_for(std::chrono::seconds(1));

    native.BeginAuth(DBus::AuthenticationProtocol::AUTH_BASIC);

#if 1
    DBus::Introspectable::Introspection introspection;
    DBus::Introspectable::Interface iface("biz.brightsign.test");
    iface.addMethod(DBus::Introspectable::Method("concat", "ss", "s"));

    introspection.addInterface(iface);

    std::string xml_introspection(introspection.serialize());
#endif

    native.callHello([](const DBus::Message::MethodReturn& msg) { std::cout << "Ready. Server on " << DBus::Type::asString(msg.getParameter(0)) << std::endl; },
        [](const DBus::Message::Error& msg) {
            std::cout << "ERROR : Server did not say 'hello' correct " << msg.getMessage() << std::endl;
        });

    native.registerMethodCallHandler("org.freedesktop.DBus.Introspectable.Introspect", [&](const DBus::Message::MethodCall& method) {
        DBus::Message::MethodReturn result(method.getSerial());
        result.addParameter(DBus::Type::String(xml_introspection));
        native.sendMethodReturn(method.getHeaderSender(), result);
    });

    // It is recommend you implement this method - even if you only return an empty string.
    // Some apps (like d-feet) will issue a GetAll before the standard method call you've requested, and
    // then wait for a reply, making them appear slow.
    native.registerMethodCallHandler("org.freedesktop.DBus.Properties.GetAll", [&](const DBus::Message::MethodCall& method) {
        //DBus::Type::Generic body = DBus::Type::String("");
        DBus::Message::MethodReturn result2(method.getSerial());
        result2.addParameter(DBus::Type::String(""));
        native.sendMethodReturn(method.getHeaderSender(), result2);
    });

    native.registerMethodCallHandler("biz.brightsign.test.concat", [&](const DBus::Message::MethodCall& method) {
        if (!method.isReplyExpected()) {
            // NOP
        } else if (method.getParameterCount() != 2) {
            DBus::Message::Error err(method.getSerial(), "biz.brightsign.Error.InvalidParameters", "This needs 2 params.");
            native.sendError(method.getHeaderSender(), err);

        } else {
            std::string input1(DBus::Type::asString(method.getParameter(0)));
            std::string input2(DBus::Type::asString(method.getParameter(1)));
            DBus::Type::Generic body = DBus::Type::String(input1 + " " + input2);
            DBus::Message::MethodReturn result(method.getSerial());
            result.addParameter(body);
            native.sendMethodReturn(method.getHeaderSender(), result);
        }
    });

    native.callRequestName("biz.brightsign", 0, [](const DBus::Message::MethodReturn& msg) {
        uint32_t result = DBus::Type::asUint32(msg.getParameter(0));

        std::cout << "Now running as biz.brightsign (" << result << ")" << std::endl;

        if (result != DBus::Native::DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
            std::cout << "Not Primary Owner." << std::endl;
        }

    },
        [](const DBus::Message::Error& msg) {
            std::cout << "ERROR FROM callRequestName : " << msg.getMessage() << std::endl;
        });

    std::this_thread::sleep_for(std::chrono::seconds(1));
    bool bRunLoop = true;

    // Now do a mini-self test
    DBus::Message::MethodCallParametersIn params;
    params.add(std::string("one"));
    params.add(std::string("two"));
    DBus::Message::MethodCall concat(DBus::Message::MethodCallIdentifier("/", "biz.brightsign.test", "concat"), params);
    native.sendMethodCall("biz.brightsign", concat,
        [&bRunLoop](const DBus::Message::MethodReturn& msg) {
            std::string result = DBus::Type::asString(msg.getParameter(0));
            if (result != "one two") {
                std::cout << "Terminating because concat is returning incorrect base data." << std::endl;
                bRunLoop = false;
            }
        },
        [&bRunLoop](const DBus::Message::Error& msg) {
            std::cout << "Terminating because concat has returned an error." << std::endl;
            bRunLoop = false;
        });

    // Spin!
    while (bRunLoop) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (gInterrupted) {
            break;
        }
    }
    std::cout << native.getStats();
}

int main(int argc, const char* argv[])
{
    s_catch_signals();
    testServer();

    return 0;
}
