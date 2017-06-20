#pragma once
#include <string>
#include <vector>

namespace DataTypes {
    enum SequenceFlag
    {
        MIDDLE,
        FIRST,
        LAST,
        STANDALONE
    };

    enum DataType
    {
        NILL,
        SIGNED,
        UNSIGNED,
        FLOAT
    };

    struct Entry {
        std::string mnemonic;
        std::string SS;
        DataTypes::DataType type;
        std::string s_APID;
        uint32_t i_APID;
        uint32_t byte;
        uint32_t bitLower;
        uint32_t bitUpper;
        uint32_t length;
    };

    struct Numeric
    {
        enum { U32, I32, F64, NILL } tag;
        std::string mnem;
        union
        {
            uint32_t u32;
            int32_t i32;
            float f64;
        };
    };

    struct Packet
    {
        std::vector<Numeric> data;
        uint16_t day;
        uint32_t millis;
        uint16_t micros;
        uint16_t sequenceCount;
    };

    struct PrimaryHeader
    {
        SequenceFlag sequenceFlag;
        uint32_t APID;
        uint32_t packetSequence;
        uint32_t packetLength;
        uint32_t secondaryHeader;
        uint32_t CCSDS;
    };

    struct SecondaryHeader
    {
        uint16_t day;
        uint32_t millis;
        uint16_t micros;
        uint32_t segments;
    };

    static inline DataType hashIt(const std::string&  inString) {
        if (inString == "D") return UNSIGNED;
        if (inString == "S") return SIGNED;
        if (inString == "U") return UNSIGNED;
        if (inString == "F") return FLOAT;
        return NILL;
    }
}
