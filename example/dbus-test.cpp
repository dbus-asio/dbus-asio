
/*
DBus-
      :::::::::      ::::::::::     :::::::::       ::::::::   :::::::::::
     :+:    :+:     :+:            :+:    :+:     :+:    :+:      :+:
    +:+    +:+     +:+            +:+    +:+     +:+    +:+      +:+
   +#+    +:+     +#++:++#       +#++:++#+      +#+    +:+      +#+
  +#+    +#+     +#+            +#+            +#+    +#+      +#+
 #+#    #+#     #+#            #+#            #+#    #+#      #+#
#########      ##########     ###             ########       ###

*/

/*
https://dbus.freedesktop.org/doc/dbus-specification.html


ASIO learnings from:
https://www.gamedev.net/blogs/entry/2249317-a-guide-to-getting-started-with-boostasio/

SASL (w/dbus)
https://github.com/bus1/dbus-broker/blob/master/src/dbus/test-sasl.c

About unix_fd
https://lists.freedesktop.org/archives/dbus/2012-November/015341.html
*/

#include "dbus.h"

void test1()
{
    DBus::Log::setLevel(DBus::Log::WARNING);

    DBus::Log::write(DBus::Log::INFO, "System bus: %s\n",
        DBus::Platform::getSystemBus().c_str());
    DBus::Log::write(DBus::Log::INFO, "Session bus: %s\n",
        DBus::Platform::getSessionBus().c_str());
    // DBus::Native native(DBus::Platform::getSystemBus());
    DBus::Native native(DBus::Platform::getSessionBus());
    sleep(1);

    native.BeginAuth(
        DBus::AuthenticationProtocol::AUTH_BASIC); // AUTH_BASIC or
    // AUTH_NEGOTIATE_UNIX_FD

    // org.bluez.obex  - use / and objectmanager

#if 1
    DBus::Introspectable::Introspection introspection;
    DBus::Introspectable::Interface iface("biz.brightsign.TestInterface");
    iface.addMethod(DBus::Introspectable::Method("Ping", "", "ss"));
    iface.addMethod(DBus::Introspectable::Method("Echo2", "ss", "s"));
    iface.addProperty(DBus::Introspectable::Property("p1", "s"));
    iface.addProperty(DBus::Introspectable::Property("p2", "s"));
    iface.addSignal(DBus::Introspectable::Signal("BroadcastStuff", "s"));

    introspection.addInterface(iface);

    std::string xml_introspection(introspection.serialize());
#endif

    // Also, support methods etc
    // native.SendAuthListMethods();

    // Hello() is necessary before any communications take place
    printf("Send hello message...\n");

    DBus::Log::setLevel(DBus::Log::TRACE);
    native.registerSignalHandler(
        "org.freedesktop.DBus.NameAcquired",
        [&](const DBus::Message::Signal& signal) {
            // Knowing the type is 's' we can be safe in assuming asString will work
            std::string signalName = DBus::Type::asString(signal.getParameter(0));
            printf("RCV signalName : NameAcquired : %s\n", signalName.c_str());
        });

    native.registerSignalHandler(
        "org.freedesktop.DBus.NameOwnerChanged",
        [&](const DBus::Message::Signal& signal) {
            std::string signalName = DBus::Type::asString(signal.getParameter(0));
            printf("RCV signalName : NameOwnerChanged : %s\n", signalName.c_str());
        });

    native.callHello(
        [](const DBus::Message::MethodReturn& msg) {
            std::string signalName = DBus::Type::asString(msg.getParameter(0));
            printf("REPLY FROM HELLO : This is our unique name : %s\n",
                signalName.c_str());
        },
        [](const DBus::Message::Error& msg) {
            printf("ERROR FROM HELLO : %s\n", msg.getMessage().c_str());
        });
    //	DBus::Log::setLevel(DBus::Log::WARNING);
    // TODO: This should get removed, but _something_ i not buffering the method
    // calls below
    // TODO: Am I deadlocking?
    // sleep(3);

#if 0
	DBus::Message::MethodCallIdentifier test_ping("/org/example/TestObject", "org.example.TestInterface", "Ping");
	native.sendMethodCall(test_ping,
	 	[] (const DBus::Message::MethodReturn &msg) {
			printf("REPLY FROM ping : %s\n", DBus::Type::asString(msg.m_Body).c_str());
	 });
#endif

    native.registerMethodCallHandler(
        "org.freedesktop.DBus.Properties.Get",
        [&](const DBus::Message::MethodCall& method) {
            DBus::Message::MethodReturn result(method.getSerial());
            // TODO: return Serial should be in ctor for error and return - and
            // FIRST PARAMETER
            std::string interface = DBus::Type::asString(method.getParameter(0));
            std::string propertyName = DBus::Type::asString(method.getParameter(1));

            // result.m_Parameters.add(DBus::Type::String("The result of " +
            // interface + " of " + propertyName + " is 42. Always 42!"));
            result.addParameter(DBus::Type::String("The result of " + interface + " of " + propertyName + " is 42. Always 42!"));

            if (propertyName == "p1") {
                native.sendMethodReturn(method.getHeaderSender(), result);
            } else {
                DBus::Message::Error err(method.getSerial(),
                    "biz.brightsign.Error.InvalidParameters",
                    "Parameter is not p1");
                native.sendError(method.getHeaderSender(), err);
            }
        });

#if 1
    native.registerMethodCallHandler(
        "org.freedesktop.DBus.Introspectable.Introspect",
        [&](const DBus::Message::MethodCall& method) {
            DBus::Message::MethodReturn result(method.getSerial());
            result.addParameter(DBus::Type::String(xml_introspection));
            native.sendMethodReturn(method.getHeaderSender(), result);
        });
#endif

// TODO: finish org.freedesktop.DBus.Properties.GetAll to return the correct
// data as name:value pair looks wrong, but it doesn't crash. Whereas the a(sv)
// is right, but causes the sender to terminate the connection
#if 1
    // It is recommend you implement this method - even if you only return an
    // empty string. Some apps (like d-feet) will wait for a reply, making them
    // appear slow.
    native.registerMethodCallHandler(
        "org.freedesktop.DBus.Properties.GetAll",
        [&](const DBus::Message::MethodCall& method) {
            printf("GetAll\nCALLBACK METHOD : serial %.4x \n", method.getSerial());
            printf("CALLBACK METHOD : sender %s \n",
                method.getHeaderSender().c_str());
            printf("CALLBACK METHOD : destination %s \n",
                method.getHeaderDestination().c_str());
            printf("CALLBACK METHOD : serial %d \n", method.getSerial());

            DBus::Type::Array propertyList;

            DBus::Type::Struct s1;
            s1.add(DBus::Type::String("p1"));
            s1.add(DBus::Type::Variant(DBus::Type::String("s")));

            propertyList.add(s1);
            // native.sendMethodReturn(method.getSerial(), header.destination,
            // header.sender, propertyList);

            // std::string result("p1:123,p2:44");
            std::string result("");
            DBus::Type::Generic body = DBus::Type::String(result);

            // REM: The destination for the incoming message is us, the sender, in
            // this context.

            // TODO
            DBus::Message::MethodReturn result2(method.getSerial());
            result2.addParameter(body);
            native.sendMethodReturn(method.getHeaderSender(), result2);
            //		native.sendMethodReturn(method.getSerial(),
            //header.destination, header.sender, body);
        });
#endif

#if 1
    native.registerMethodCallHandler(
        "biz.brightsign.TestInterface.Echo2",
        [&](const DBus::Message::MethodCall& method) {
            if (!method.isReplyExpected()) {
                // NOP
            } else if (method.getParameterCount() != 2) {
                DBus::Message::Error err(method.getSerial(),
                    "biz.brightsign.Error.InvalidParameters",
                    "This needs 2 params.");
                native.sendError(method.getHeaderSender(), err);

            } else {
                std::string input1(DBus::Type::asString(method.getParameter(0)));
                std::string input2(DBus::Type::asString(method.getParameter(1)));
                DBus::Type::Generic body = DBus::Type::String("Echo of : " + input1 + " and " + input2);
                // TODO
                DBus::Message::MethodReturn result(method.getSerial());
                result.addParameter(body);
                native.sendMethodReturn(method.getHeaderSender(), result);
            }
        });

    native.registerMethodCallHandler(
        "biz.brightsign.TestInterface.Ping",
        [&](const DBus::Message::MethodCall& method) {
            DBus::Type::Array propertyList;

            propertyList.add(DBus::Type::DictEntry(DBus::Type::String("p1"),
                DBus::Type::String("two")));

            DBus::Message::MethodReturn result(method.getSerial());
            result.addParameter(DBus::Type::String("pong!!"));
            native.sendMethodReturn(method.getHeaderSender(), result);
        });

#endif

    // Run a "server"
    native.callRequestName(
        "test.steev", 0,
        [](const DBus::Message::MethodReturn& msg) {
            uint32_t result = DBus::Type::asUint32(msg.getParameter(0));

            if (result != DBus::Native::DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
                printf("Not Primary Owner\n");
            }
        },
        [](const DBus::Message::Error& msg) {
            printf("ERROR FROM callRequestName : %s\n", msg.getMessage().c_str());
        });

    printf("Try and call ourselves...\n");
    DBus::Message::MethodCallParametersIn params;
    params.add(std::string("one"));
    params.add(std::string("two"));
    DBus::Message::MethodCall concat(
        DBus::Message::MethodCallIdentifier("/", "biz.brightsign.TestInterface",
            "Echo2"),
        params);
    native.sendMethodCall(
        "test.steev", concat,
        [](const DBus::Message::MethodReturn& msg) {
            std::string result = DBus::Type::asString(msg.getParameter(0));
            printf("REPLY FROM echo2 : This is our id : %s\n", result.c_str());
        },
        [](const DBus::Message::Error& msg) {
            printf("ERROR FROM echo2 : %s\n", msg.getMessage().c_str());
        });

    sleep(200);
}

int main(int argc, const char* argv[])
{
    test1();

    return 0;
}
