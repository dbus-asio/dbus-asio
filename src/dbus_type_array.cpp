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
#include "dbus_messagestream.h"

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

void DBus::Type::Array::marshall(MessageStream& stream) const
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

void DBus::Type::Array::marshallContents(MessageStream& stream) const
{
    for (size_t i = 0; i < contents.size(); ++i) {
        DBus::Type::marshallData(contents[i], stream);
    }
}

bool DBus::Type::Array::unmarshall(const UnmarshallingData& data)
{

    if (m_Unmarshalling.areWeSkippingPaddingForSize && !Utils::isAlignedTo(getAlignment(), data.offset)) {
        return false;
    }
    m_Unmarshalling.areWeSkippingPaddingForSize = false;

    // DOCS: The array type code must be followed by a single complete type.
    // (Although this might be a structure. Therefore the padding must consider both)

    // Unmarshalling structure:
    // A UINT32 giving the length of the array data in bytes, followed by alignment padding
    // to the alignment boundary of the array element type, followed by each array element.

    // We have the idea of 'for whom am I unmarhsalling'
    // a) myself. i.e. I skip padding and parse data that I use
    // b) others. i.e. I pass data onto the current generic type within me

    // First four bytes are the size of the array in bytes
    if (m_Unmarshalling.count++ < 4) {
        m_Unmarshalling.array_size <<= 8;
        m_Unmarshalling.array_size += data.c;
        if (m_Unmarshalling.count == 4) {
            m_Unmarshalling.createType = true;
            m_Unmarshalling.array_size = doSwap32(m_Unmarshalling.array_size);
            m_Unmarshalling.typeSignature = Type::extractSignature(getSignature(), 1);

            Log::write(Log::TRACE, "Unmarshall : Array : size = %ld\n", m_Unmarshalling.array_size);

            if (m_Unmarshalling.array_size > 67108864) {
                Log::write(Log::ERROR, "Array is too long (%d vs %d) and should not be received.", m_Unmarshalling.array_size, 67108864);
                throw std::out_of_range("Can not unmarshall arrays over 67108864 bytes");
            }
        }
        // This handles the edge case when we've read the size, but there's
        // no data to read.
        return m_Unmarshalling.count == 4 && m_Unmarshalling.array_size == 0;
    }

    if (m_Unmarshalling.createType) {
        m_Unmarshalling.createType = false;

        contents.push_back(DBus::Type::create(m_Unmarshalling.typeSignature, isLittleEndian()));
    }

    // Unmarshall the next character into the current data type
    DBus::Type::Generic& current_type = contents.back();
    m_Unmarshalling.createType = DBus::Type::unmarshallData(current_type, data);

    if (m_Unmarshalling.createType) {

        if (m_Unmarshalling.count - 4 >= m_Unmarshalling.array_size) {
            DBus::Log::write(DBus::Log::TRACE, "Array element complete.\n");
            return true;
        }
    }

    return false;
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
