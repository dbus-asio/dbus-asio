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

// Type::extractSignature(getSignature(), 1);
/*
void DBus::Type::Array::etSignature(const std::string &type) {

}
*/
const std::vector<DBus::Type::Generic>& DBus::Type::Array::getContents() const { return contents; }

void DBus::Type::Array::marshall(MessageStream& stream) const
{
    stream.pad4(); // Needed for size in bytes (although in practise writeInt32 will add its own padding)

    // First, compute the stream data for the array, since we need to know its size
    // in bytes for the first part.
    MessageStream tmp_stream;
    marshallContents(tmp_stream);

    if (tmp_stream.size() > 67108864) {
        Log::write(Log::ERROR, "Array is too long (%d vs %d) to be sent.", tmp_stream.size(), 67108864);
        throw std::out_of_range("Can not marshall arrays over 67108864 bytes");
    }

    // A UINT32 giving the length of the array data in bytes
    stream.writeUint32(tmp_stream.size());

    // followed by alignment padding to the alignment boundary of the array element type
    stream.pad(Type::getAlignment(getSignature()));

    // followed by each array element.
    stream.write(tmp_stream);
}

void DBus::Type::Array::marshallContents(MessageStream& stream) const
{

    MessageStream tmp_stream;
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

        contents.push_back(DBus::Type::create(m_Unmarshalling.typeSignature));
    }

    // Unmarshall the next character into the current data type
    DBus::Type::Generic& current_type = contents.back();
    m_Unmarshalling.createType = DBus::Type::unmarshallData(current_type, data);

    if (m_Unmarshalling.createType) {

        m_Unmarshalling.areWeSkippingPaddingForElement = true; // we reset this because we might need to pad the next element in the array

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
