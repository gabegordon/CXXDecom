#include "stdafx.h"
#include <fstream>
#include "HeaderDecode.h"
#include "DataDecode.h"
#include "ByteManipulation.h"

using namespace ByteManipulation;

bool DataDecode::loadData(const std::vector<uint8_t>& buf, Bytes& bytes, const DataTypes::Entry& currEntry)
{
	uint32_t length = currEntry.length;
	if (length > 24)
	{
		if (currEntry.byte + 1 >= buf.size() || currEntry.byte + 2 >= buf.size() || currEntry.byte + 3 >= buf.size())
			return false;
		m_byte1 = buf.at(currEntry.byte + 1);
		m_byte2 = buf.at(currEntry.byte + 2);
		m_byte3 = buf.at(currEntry.byte + 3);
		bytes = FOUR;
	}
	else if (length > 16)
	{
		if (currEntry.byte + 1 >= buf.size() || currEntry.byte + 2 >= buf.size())
			return false;
		m_byte1 = buf.at(currEntry.byte + 1);
		m_byte2 = buf.at(currEntry.byte + 2);
		bytes = THREE;
	}
	else if (length > 8)
	{
		if (currEntry.byte + 1 >= buf.size())
			return false;
		m_byte1 = buf.at(currEntry.byte + 1);
		bytes = TWO;
	}
	else
	{
		bytes = ONE;
	}
	return true;
}

void DataDecode::getHeaderData(DataTypes::Packet& pack)
{
	pack.day = m_sHeader.day;
	pack.millis = m_sHeader.millis;
	pack.micros = m_sHeader.micros;
	pack.sequenceCount = m_pHeader.packetSequence;
}

DataTypes::Packet DataDecode::decodeData(std::ifstream& infile)
{
	DataTypes::Packet pack;
	std::vector<uint8_t> buf(m_pHeader.packetLength); //reserve space for bytes
	infile.read(reinterpret_cast<char*>(buf.data()), buf.size()); //read bytes

	for (DataTypes::Entry& currEntry : m_entries)
	{
		DataTypes::Numeric num;
		Bytes numBytes;
		uint8_t m_byte1, m_byte2, m_byte3;

		if (!loadData(buf, numBytes, currEntry) || currEntry.byte >= buf.size()) //Make sure we don't go past array bounds (entries not contained in packet)
			break;

		uint8_t initialByte = buf.at(currEntry.byte);

		DataTypes::DataType dtype = currEntry.type;

		if (dtype == DataTypes::FLOAT)
		{
		}
		else
		{
			switch (numBytes)
			{
			case ONE:
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
				uint16_t result = mergeBytes16(initialByte, m_byte1);
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
				uint32_t result = mergeBytes(initialByte, m_byte1, m_byte2, m_byte3, 3);
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
				uint32_t result = mergeBytes(initialByte, m_byte1, m_byte2, m_byte3, 4);
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
	getHeaderData(pack);
	return pack;
}

DataTypes::Packet DataDecode::decodeDataSegmented(std::ifstream& infile)
{
	DataTypes::Packet segPack;
	getHeaderData(segPack);
	while (m_pHeader.sequenceFlag != DataTypes::LAST)
	{
		DataTypes::Packet pack;
		std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> headers = HeaderDecode::decodeHeaders(infile);
		m_pHeader = std::get<0>(headers);
		pack = decodeData(infile);
		segPack.data.insert(std::end(segPack.data), std::begin(pack.data), std::end(pack.data));
	}
	return segPack;
}
