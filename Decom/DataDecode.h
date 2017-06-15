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

	DataTypes::Packet decodeData(std::ifstream& infile, std::vector<DataTypes::Entry>& entries, const uint32_t& packetLength);
	uint32_t mergeBytes(uint8_t& initialByte, uint8_t& extraByte1, uint8_t& extraByte2, uint8_t& extraByte3, const uint32_t& num);
	void loadData(uint8_t& extraByte1, uint8_t& extraByte2, uint8_t& extraByte3, const std::vector<uint8_t>& buf, Bytes& bytes, const DataTypes::Entry& currEntry);
}

