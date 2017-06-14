#include "stdafx.h"
#include <fstream>
#include "DataDecode.h"
#include "ByteManipulation.h"


namespace DataDecode
{
	enum Bytes
	{
		ONE,
		TWO,
		THREE,
		FOUR
	};
	
	void loadData(uint8_t& extraByte1, uint8_t& extraByte2, uint8_t& extraByte3, const std::vector<uint8_t>& buf, Bytes& bytes, const DataTypes::Entry& currEntry)
	{
		uint32_t length = std::stoi(currEntry.type.substr(1, std::string::npos));
		if (length > 24)
		{
			extraByte1 = buf.at(currEntry.byte + 1);
			extraByte2 = buf.at(currEntry.byte + 2);
			extraByte3 = buf.at(currEntry.byte + 3);
			bytes = FOUR;
		}
		else if (length > 16)
		{
			extraByte1 = buf.at(currEntry.byte + 1);
			extraByte2 = buf.at(currEntry.byte + 2);
			bytes = THREE;
		}
		else if (length > 8)
		{
			extraByte1 = buf.at(currEntry.byte + 1);
			bytes = TWO;
		}
		else
		{
			bytes = ONE;
		}
	}
	
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
			Bytes numBytes;
			
			uint8_t initialByte = buf.at(currEntry.byte);
			uint8_t extraByte1, extraByte2, extraByte3;
			loadData(extraByte1, extraByte2, extraByte3, buf, numBytes, currEntry);
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
					case THREE:
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
					case FOUR:
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
				}

				if (dtype == DataTypes::SIGNED)
					num.tag = DataTypes::Numeric::I32;
				else
					num.tag = DataTypes::Numeric::U32;
				
				num.mnem = currEntry.mnemonic;
				
				pack.data.push_back(num);
			}
		}
		return pack;
	}
}
