#include "stdafx.h"
#include <fstream>
#include "DataDecode.h"

namespace DataDecode
{
	DataTypes::Packet decodeData(std::ifstream& infile, const std::vector<DataTypes::Entry>& entries, const uint32_t& packetLength)
	{
		DataTypes::Packet pack;
		std::vector<uint8_t> buf(packetLength); //reserve space for bytes
		infile.read(reinterpret_cast<char*>(buf.data()), buf.size()); //read bytes

		for (const DataTypes::Entry& currEntry : entries)
		{
			if (currEntry.byte > packetLength + 1 || currEntry.byte > buf.size()) //If entry byte isn't included in packet, then skip
				break;
			
			DataTypes::Numeric num;

		}
		return pack;
	}
}
