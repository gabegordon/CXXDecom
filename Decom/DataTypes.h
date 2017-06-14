#pragma once
#include <string>
#include <vector>

namespace DataTypes {
	struct Entry {
		std::string mnemonic;
		std::string SS;
		std::string type;
		std::string s_APID;
		uint32_t i_APID;
		uint32_t byte;
		uint32_t bitLower;
		uint32_t bitUpper;
	};

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

	struct Numeric
	{
		enum { U32, I32, F64, NLINE } tag;
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
		uint64_t timeCode;
		uint32_t segments;
	};

	static inline DataType hashIt(std::string const& inString) {
		if (inString == "D") return UNSIGNED;
		if (inString == "S") return SIGNED;
		if (inString == "U") return UNSIGNED;
		if (inString == "F") return FLOAT;
		return NILL;
	}
}