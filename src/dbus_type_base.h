#ifndef DBUS_TYPE_BASE_H
#define DBUS_TYPE_BASE_H

namespace DBus {

namespace Type {

    // variant didn't work because the DBus variant type allows structs within structs,
    // meaning we need a forward reference.
    // Any suggestsions?
    //typedef boost::variant<Byte, Uint32, String, Struct, Variant, Array> Generic;

    // I dropped any when attempting to retrieve the specific type at compile time. (Since I needed it
    // to bind the unmarshall method) Even grabbing a ptr to the base class seemed impossible.
    // I returned to it when a custom class seemed too much effort.
    // https://theboostcpplibraries.com/boost.any
    typedef boost::any Generic;
    // TODO: I would like to have Generic derive from boost:any so the asByte methods can be
    // added to it. But there's a lot of plumbing since operator= et al need templating. It'd be
    // as quick to copy+paste boost::any as Generic and add the methods there. (I don't like that, tho.)

    // This describes multiple complete types. e.g. ss, i(ii)
    // (There doesn't appear to be an official name other than "multiple complete type" and I couldn't think of better)
    struct CompositeBlock {
        std::vector<Generic> m_TypeList;
    };

    // This base class exists primarily to act as marshalling helpers.
    // The use of boost::any prohibits our ability to grab typed data, and use
    // virtual methods.
    class Base {
    public:
        Base()
            : m_isLittleEndian(true)
        {
        }

        uint16_t doSwap16(uint16_t v) const;
        uint32_t doSwap32(uint32_t v) const;
        uint64_t doSwap64(uint64_t v) const;
        void setLittleEndian(bool isLittle) { m_isLittleEndian = isLittle; }
        size_t getAlignment() const { return 1; }

        void setSignature(const std::string& type);
        std::string getSignature() const;

        std::string toString(const std::string& prefix) const; // each non-container type should end with \n as as convenience to the calling function
        std::string asString() const;

    protected:
        bool m_isLittleEndian;
        std::string m_Signature;

        bool isSwapRequired() const;
    };
}
}

#endif
