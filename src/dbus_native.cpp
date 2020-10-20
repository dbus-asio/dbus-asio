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

#include "dbus_native.h"
#include "dbus_auth.h"
#include "dbus_log.h"
#include "dbus_matchrule.h"
#include "dbus_message.h"
#include "dbus_messageprotocol.h"

std::string DBus::Native::DBusDaemon("org.freedesktop.DBus");

DBus::Native::Native(const std::string& busname)
    : m_MessageProtocol(new DBus::MessageProtocol())
    , m_Transport(new DBus::Transport(busname))
    , m_AuthenticationProtocol(new DBus::AuthenticationProtocol(m_Transport))
{
    m_Transport->setDataHandler(
        std::bind(&Native::onReceiveAuthData, this, std::placeholders::_1));
    m_MessageProtocol->setMethodCallHandler(
        std::bind(&Native::onReceiveMethodCall, this, std::placeholders::_1));
    m_MessageProtocol->setMethodReturnHandler(
        std::bind(&Native::onReceiveMethodReturn, this, std::placeholders::_1));
    m_MessageProtocol->setErrorHandler(
        std::bind(&Native::onReceiveError, this, std::placeholders::_1));
    m_MessageProtocol->setSignalHandler(
        std::bind(&Native::onReceiveSignal, this, std::placeholders::_1));
}

DBus::Native::~Native()
{
    m_Transport->setDataHandler([](DBus::OctetBuffer&) {});
    m_MessageProtocol->setMethodCallHandler(
        [](const DBus::Message::MethodCall& method) {});
    m_MessageProtocol->setMethodReturnHandler(
        [](const DBus::Message::MethodReturn& method) {});
    m_MessageProtocol->setErrorHandler([](const DBus::Message::Error& method) {});
    m_MessageProtocol->setSignalHandler(
        [](const DBus::Message::Signal& method) {});
}

void DBus::Native::BeginAuth(AuthenticationProtocol::AuthRequired type)
{
    m_AuthenticationProtocol->sendAuth(type);
}

void DBus::Native::onReceiveAuthData(OctetBuffer& buffer)
{
    try {
        const size_t bufferSize = buffer.size();
        if (m_AuthenticationProtocol->onReceiveData(buffer)) {
            m_Stats.bytes_auth = bufferSize - buffer.size();
            onReceiveMessageData(buffer);
            m_Transport->setDataHandler(std::bind(&Native::onReceiveMessageData, this,
                std::placeholders::_1));
        }
    } catch (const std::exception& e) {
        DBus::Log::write(
            DBus::Log::ERROR,
            "DBus :: Native : onReceiveAuthData has thrown an exception : %s\n",
            e.what());
        m_AuthenticationProtocol->reset();
    }
}

void DBus::Native::onReceiveMessageData(OctetBuffer& buffer)
{
    try {
        // AFAIK, we can ever leave the message protocol, once auth'd, so the return
        // value can be safely ignored. (To exit the communication we just close the
        // socket.)
        m_Stats.bytes_message += buffer.size();
        m_MessageProtocol->onReceiveData(buffer);
    } catch (const std::exception& e) {
        DBus::Log::write(
            DBus::Log::ERROR,
            "DBus :: Native : onReceiveMessageData has thrown an exception : %s\n",
            e.what());
        m_MessageProtocol->reset();
    }
}

void DBus::Native::registerMethodCallHandler(
    const std::string& name,
    const DBus::Message::CallbackFunctionMethodCall& handler)
{
    boost::recursive_mutex::scoped_lock guard(m_MethodCallMapMutex);

    m_MethodCallMap.emplace(name, handler);
}

void DBus::Native::registerSignalHandler(
    const std::string& name,
    const DBus::Message::CallbackFunctionSignal& handler)
{
    boost::recursive_mutex::scoped_lock guard(m_SignalMapMutex);

    m_SignalMap.emplace(name, handler);
}

//
// Send messages out
//
void DBus::Native::sendMethodCall(
    const std::string& destination, const DBus::Message::MethodCall& method,
    const DBus::Message::CallbackFunctionMethodReturn& success,
    const DBus::Message::CallbackFunctionError& failure)
{
    ++m_Stats.count_sent_methodcalls;
    if (method.isReplyExpected()) {
        boost::recursive_mutex::scoped_lock guard(m_MessageQueueMutex);

        m_MessageQueue.emplace(method.getSerial(),
            Native::CallbackPair(success, failure));
    }
    m_Transport->sendString(method.marshall(destination));
}

void DBus::Native::sendMethodReturn(const std::string& destination,
    const DBus::Message::MethodReturn& result)
{
    ++m_Stats.count_sent_methodreturns;
    m_Transport->sendString(result.marshall(destination));
}

void DBus::Native::sendError(const std::string& destination,
    const DBus::Message::Error& err)
{
    ++m_Stats.count_sent_errors;
    m_Transport->sendString(err.marshall(destination));
}

void DBus::Native::sendSignal(const std::string& destination,
    const DBus::Message::Signal& signal)
{
    ++m_Stats.count_sent_signals;
    m_Transport->sendString(signal.marshall(destination));
}

//
// Handle incoming messages
//
void DBus::Native::onReceiveMethodCall(
    const DBus::Message::MethodCall& method)
{
    DBus::Log::write(DBus::Log::INFO, "DBus :: onReceiveMethodCall : %s\n",
        method.getFullName().c_str());
    ++m_Stats.count_receive_methodcalls;

    boost::recursive_mutex::scoped_lock guard(m_MethodCallMapMutex);
    auto it = m_MethodCallMap.find(method.getFullName());

    if (it != m_MethodCallMap.end()) {
        it->second(method);
    }
}

void DBus::Native::onReceiveMethodReturn(
    const DBus::Message::MethodReturn& result)
{
    boost::recursive_mutex::scoped_lock guard(m_MessageQueueMutex);
    auto it = m_MessageQueue.find(result.m_SerialReplyingTo);
    ++m_Stats.count_receive_methodreturns;

    if (it != m_MessageQueue.end()) {
        it->second.success(result);
        m_MessageQueue.erase(it);
    }
}

void DBus::Native::onReceiveError(const DBus::Message::Error& error)
{
    boost::recursive_mutex::scoped_lock guard(m_MessageQueueMutex);
    auto it = m_MessageQueue.find(error.getSerialOfReply());

    if (it != m_MessageQueue.end()) {
        it->second.failure(error);
        m_MessageQueue.erase(it);
    }
    DBus::Log::write(DBus::Log::WARNING, "DBus :: Error : %s\n",
        error.getMessage().c_str());
}

void DBus::Native::onReceiveSignal(const DBus::Message::Signal& signal)
{
    boost::recursive_mutex::scoped_lock guard(m_SignalMapMutex);
    ++m_Stats.count_receive_signals;

    std::string fullname = signal.getHeaderInterface() + "." + signal.getHeaderMember();
    auto it = m_SignalMap.find(fullname);

    if (it != m_SignalMap.end()) {
        it->second(signal);
    }

    {
        boost::recursive_mutex::scoped_lock guard(m_RulesMapMutex);
        for (auto it : m_RulesMap) {
            if (it.second.isMatched(signal)) {
                it.second.invoke(signal);
            }
        }
    }
}

std::string DBus::Native::getStats() const
{
    std::stringstream ss;

    ss << "Native stats:" << std::endl;
    ss << " count_sent_methodcalls: " << m_Stats.count_sent_methodcalls
       << std::endl;
    ss << " count_sent_methodreturns: " << m_Stats.count_sent_methodreturns
       << std::endl;
    ss << " count_sent_errors: " << m_Stats.count_sent_errors << std::endl;
    ss << " count_sent_signals: " << m_Stats.count_sent_signals << std::endl;

    ss << " count_receive_methodcalls: " << m_Stats.count_receive_methodcalls
       << std::endl;
    ss << " count_receive_methodreturns: " << m_Stats.count_receive_methodreturns
       << std::endl;
    ss << " count_receive_errors: " << m_Stats.count_receive_errors << std::endl;
    ss << " count_receive_signals: " << m_Stats.count_receive_signals
       << std::endl;

    ss << " bytes_auth: " << m_Stats.bytes_auth << std::endl;
    ss << " bytes_message: " << m_Stats.bytes_message << std::endl;

    ss << m_Transport->getStats();
    return ss.str();
}
