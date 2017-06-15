#pragma once
#include "DataTypes.h"

namespace DataDecode
{
	enum Bytes
	{
		ONE,
		TWO,
		THREE,
		FOUR
	};

	DataTypes::Packet decodeData(std::ifstream& infile, std::vector<DataTypes::Entry>& entries, const uint32_t& packetLength, const DataTypes::SecondaryHeader& sheader);
	bool loadData(uint8_t& extraByte1, uint8_t& extraByte2, uint8_t& extraByte3, const std::vector<uint8_t>& buf, Bytes& bytes, const DataTypes::Entry& currEntry);
	void getTimestamp(DataTypes::Packet& pack, const DataTypes::SecondaryHeader& sheader);
}

