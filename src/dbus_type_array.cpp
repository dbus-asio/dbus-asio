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

#include <sstream>

#include <boost/thread/recursive_mutex.hpp>

#include "dbus_log.h"

#include "dbus_type.h"
#include "dbus_type_array.h"
#include "dbus_type_base.h"
#include "dbus_type_dictentry.h"
#include "dbus_type_struct.h"

#include "dbus_message.h"
#include "dbus_messageprotocol.h"
#include "dbus_messageostream.h"
#include "dbus_messageistream.h"

const std::string DBus::Type::Array::s_StaticTypeCode("a");

size_t DBus::Type::Array::size() const { return contents.size(); }

size_t DBus::Type::Array::add(const DBus::Type::Struct& s)
{
    contents.push_back(s);
    return size();
}

size_t DBus::Type::Array::add(const DBus::Type::DictEntry& s)
{
    contents.push_back(s);
    return size();
}

const std::vector<DBus::Type::Generic>& DBus::Type::Array::getContents() const { return contents; }

void DBus::Type::Array::marshall(MessageOStream& stream) const
{
    const size_t sizePos = stream.size();
    stream.writeUint32(0);

    // Size does not include any padding to the first element
    stream.pad(Type::getAlignment(Type::extractSignature(getSignature(), 1)));

    const size_t contentsStartPos = stream.size();
    marshallContents(stream);
    const uint32_t contentsSize = stream.size() - contentsStartPos;
    stream.data.replace(sizePos, 4, (char*)&contentsSize, sizeof(uint32_t));
}

void DBus::Type::Array::marshallContents(MessageOStream& stream) const
{
    for (size_t i = 0; i < contents.size(); ++i) {
        DBus::Type::marshallData(contents[i], stream);
    }
}

void DBus::Type::Array::unmarshall(MessageIStream& stream)
{
    uint32_t size = 0;
    stream.read<uint32_t>(&size);
    std::string signature = Type::extractSignature(getSignature(), 1);
    stream.align(Type::getAlignment(signature));

    MessageIStream arrayStream(stream, size);
    while(!arrayStream.empty()) {
        contents.push_back(DBus::Type::create(signature));
        DBus::Type::unmarshallData(contents.back(), arrayStream);
    };
}

std::string DBus::Type::Array::getSignature() const
{
    if (contents.size()) {
        return "a" + DBus::Type::getMarshallingSignature(contents[0]);
    }

    return m_Signature;
}

std::string DBus::Type::Array::toString(const std::string& prefix) const
{
    std::stringstream ss;

    ss << prefix << "Array (" << getSignature() << ") [\n";
    for (size_t i = 0; i < contents.size(); ++i) {
        ss << prefix << "   [" << i << "] = \n";
        ss << DBus::Type::toString(contents[i], prefix + "      ");
    }
    ss << prefix << "]\n";

    return ss.str();
}

std::string DBus::Type::Array::asString() const { return "[array]"; }
