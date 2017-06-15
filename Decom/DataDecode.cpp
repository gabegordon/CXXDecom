#include "stdafx.h"
#include <fstream>
#include "DataDecode.h"
#include "ByteManipulation.h"

using namespace ByteManipulation;

namespace DataDecode
{	
	bool loadData(uint8_t& extraByte1, uint8_t& extraByte2, uint8_t& extraByte3, const std::vector<uint8_t>& buf, Bytes& bytes, const DataTypes::Entry& currEntry)
	{
		uint32_t length = std::stoi(currEntry.type.substr(1, std::string::npos));
		if (length > 24)
		{
			if (currEntry.byte + 1 >= buf.size() || currEntry.byte + 2 >= buf.size() || currEntry.byte + 3 >= buf.size())
				return false;
			extraByte1 = buf.at(currEntry.byte + 1);
			extraByte2 = buf.at(currEntry.byte + 2);
			extraByte3 = buf.at(currEntry.byte + 3);
			bytes = FOUR;
		}
		else if (length > 16)
		{
			if (currEntry.byte + 1 >= buf.size() || currEntry.byte + 2 >= buf.size())
				return false;
			extraByte1 = buf.at(currEntry.byte + 1);
			extraByte2 = buf.at(currEntry.byte + 2);
			bytes = THREE;
		}
		else if (length > 8)
		{
			if (currEntry.byte + 1 >= buf.size())
				return false;
			extraByte1 = buf.at(currEntry.byte + 1);
			bytes = TWO;
		}
		else
		{
			bytes = ONE;
		}
		return true;
	}

	void getTimestamp(DataTypes::Packet& pack, const DataTypes::SecondaryHeader& sheader)
	{
		pack.day = sheader.day;
		pack.millis = sheader.millis;
		pack.micros = sheader.micros;
	}

	DataTypes::Packet decodeData(std::ifstream& infile, std::vector<DataTypes::Entry>& entries, const uint32_t& packetLength, const DataTypes::SecondaryHeader& sheader)
	{
		DataTypes::Packet pack;
		std::vector<uint8_t> buf(packetLength); //reserve space for bytes
		infile.read(reinterpret_cast<char*>(buf.data()), buf.size()); //read bytes

		for (DataTypes::Entry& currEntry : entries)
		{
			DataTypes::Numeric num;
			Bytes numBytes;
			uint8_t extraByte1, extraByte2, extraByte3;
			
			if (!loadData(extraByte1, extraByte2, extraByte3, buf, numBytes, currEntry) || currEntry.byte >= buf.size()) //Make sure we don't go past array bounds (entries not contained in packet)
				break;

			uint8_t initialByte = buf.at(currEntry.byte);
			
			DataTypes::DataType dtype = DataTypes::hashIt(currEntry.type.substr(0, 1));
			
			if (dtype == DataTypes::FLOAT) 
			{
			}
			else
			{
				switch (numBytes)
				{
					case ONE :
					{
						if (currEntry.bitUpper == 0 && currEntry.bitLower == 0)
						{
							if (dtype == DataTypes::SIGNED)
								num.i32 = initialByte;
							else
								num.u32 = initialByte;
						}
						else
						{
							if (dtype == DataTypes::SIGNED)
								num.i32 = ByteManipulation::extract8Signed(initialByte, currEntry.bitLower, (currEntry.bitUpper - currEntry.bitLower + 1));
							else
								num.u32 = ByteManipulation::extract8(initialByte, currEntry.bitLower, (currEntry.bitUpper - currEntry.bitLower + 1));
						}
						break;
					}
					case TWO:
					{
						uint16_t result = mergeBytes16(initialByte, extraByte1);
						if (currEntry.bitUpper == 0 && currEntry.bitLower == 0)
						{
							if (dtype == DataTypes::SIGNED)
								num.i32 = static_cast<int16_t>(result);
							else
								num.u32 = result;
						}
						else
						{
							if (dtype == DataTypes::SIGNED)
								num.i32 = static_cast<int16_t>(ByteManipulation::extract16(result, currEntry.bitLower, (currEntry.bitUpper - currEntry.bitLower + 1)));
							else
								num.u32 = ByteManipulation::extract16(result, currEntry.bitLower, (currEntry.bitUpper - currEntry.bitLower + 1));
						}
						break;
					}
					case THREE:
					{
						uint32_t result = mergeBytes(initialByte, extraByte1, extraByte2, extraByte3, 3);
						if (currEntry.bitUpper == 0 && currEntry.bitLower == 0)
						{
							if (dtype == DataTypes::SIGNED)
								num.i32 = static_cast<int32_t>(result);
							else
								num.u32 = result;
						}
						else
						{
							if (dtype == DataTypes::SIGNED)
								num.i32 = static_cast<int32_t>(ByteManipulation::extract32(result, currEntry.bitLower, (currEntry.bitUpper - currEntry.bitLower + 1)));
							else
								num.u32 = ByteManipulation::extract32(result, currEntry.bitLower, (currEntry.bitUpper - currEntry.bitLower + 1));
						}
						break;
					}
					case FOUR:
					{
						uint32_t result = mergeBytes(initialByte, extraByte1, extraByte2, extraByte3, 4);
						if (currEntry.bitUpper == 0 && currEntry.bitLower == 0)
						{
							if (dtype == DataTypes::SIGNED)
								num.i32 = static_cast<int32_t>(result);
							else
								num.u32 = result;
						}
						else
						{
							if (dtype == DataTypes::SIGNED)
								num.i32 = static_cast<int32_t>(ByteManipulation::extract32(result, currEntry.bitLower, (currEntry.bitUpper - currEntry.bitLower + 1)));
							else
								num.u32 = ByteManipulation::extract32(result, currEntry.bitLower, (currEntry.bitUpper - currEntry.bitLower + 1));
						}
						break;
					}
				}

				if (dtype == DataTypes::SIGNED)
					num.tag = DataTypes::Numeric::I32;
				else
					num.tag = DataTypes::Numeric::U32;
				
				num.mnem = currEntry.mnemonic;
				pack.data.push_back(num);
			}
		}
		getTimestamp(pack, sheader);
		return pack;
	}
}
