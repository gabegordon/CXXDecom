#pragma once
#include "DataTypes.h"

namespace DataDecode
{
	DataTypes::Packet decodeData(std::ifstream& infile, const std::vector<DataTypes::Entry>& entries, const uint32_t& packetLength);
}

