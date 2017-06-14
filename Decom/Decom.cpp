#include "stdafx.h"
#include <iostream>
#include <bitset>
#include <iterator>
#include <stdlib.h>  
#include "Decom.h"
#include "ByteManipulation.h"

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define BITS(k,m,n) LAST((k)>>(m),((n)-(m)))

typedef uint8_t BYTE;
using namespace ByteManipulation;
using std::cout;
using std::endl;

bool Decom::loadPackets(const std::string& filename, const std::vector<entry>& entries)
{
	loadToMemory(filename);
	while (!m_complete)
	{
		readHeader();
		if (m_secondaryHeader)
		{
			readSecondHeader();
		}
		readData(entries);
		m_secondaryHeader = false;
		if (m_byteIndex >= m_allBytes.size())
		{
			writeData();
			m_complete = true;
		}
	}
	if (m_map.empty())
	{
		return false;
	}
	return true;
}

void Decom::loadToMemory(const std::string& filename)
{
	m_infile.open("C:/JPSS/tlm.pkt", std::ios::binary | std::ios::in);
	
	m_infile.seekg(0, std::ios::end);
	size_t filesize = m_infile.tellg();
	m_infile.seekg(0, std::ios::beg);

	m_allBytes.resize(filesize / sizeof(BYTE));

	m_infile.read((char *)m_allBytes.data(), filesize);

	std::cout << filesize << "," << m_allBytes.size() << endl;

}

void Decom::readHeader()
{
	uint8_t byte1 = getByte();
	uint8_t byte2 = getByte();
	uint16_t packetID = mergeBytes(byte1, byte2);
	m_secondaryHeader = BITS(packetID, 11, 12);												            //0 or 1 flag located at 5th bit (11th from right)
	m_APID = BITS(packetID, 0, 11);
	
	byte1 = getByte();
	byte2 = getByte();
	uint16_t packetSeqControl = mergeBytes(byte1, byte2);
	m_sequenceFlag = static_cast<SequenceFlag>(BITS(packetSeqControl, 14, 16));						    //00-Cont,01-First,10-Last,11-Stand flag for sequence
	m_packetSequence = BITS(packetSeqControl, 0, 14);												    //14 bits for sequence number

	byte1 = getByte();
	byte2 = getByte();
	m_packetLength = mergeBytes(byte1, byte2);												    //2 bytes for packet length
	
	if (m_debug);
		//debugPrinter(packetID, packetSeqControl);
}

void Decom::readSecondHeader()
{
	std::vector<uint8_t> m_timeCodeBytes = getXBytes(8);												//8 bytes for timecode

	if (m_packetSequence == FIRST)
	{
		uint16_t segmentAndSpare = mergeBytes(getByte(), getByte());
		m_segments = BITS(segmentAndSpare, 7, 16);													    //First 1 byte for segment count
	}
}

void Decom::readData(const std::vector<entry>& entries)
{
	if (m_APID != m_pastAPID)
	{
		m_entries.clear();
		m_pastAPID = m_APID;
		getEntries(entries);
	}

	if (m_secondaryHeader) //Account for bytes in secondary header
	{
		if (m_sequenceFlag == FIRST)
		{
			m_packetLength -= 10;
		}
		else
		{
			m_packetLength -= 8;
		}
	}
	uint32_t offset = m_byteIndex;
	std::vector<Numeric> vec;
	for(auto& currEntry: m_entries)
	{
		if (currEntry.byte > m_packetLength + 1 || currEntry.byte+offset >= m_allBytes.size()) {
			continue;
		}
		Numeric num;
		uint32_t length = std::stoi(currEntry.type.substr(1, std::string::npos));
		uint32_t byte = currEntry.byte;
		uint32_t bitlower = currEntry.bitLower;
		uint32_t bitupper = currEntry.bitUpper + 1;

		if (bitupper > 16)
			switchEnds(bitlower, bitupper, 32);
		else if (bitupper > 8)
			switchEnds(bitlower, bitupper, 16);
		else if (bitupper == 0 && bitlower == 0);
		else
			switchEnds(bitlower, bitupper, 8);
		switch (hashIt(currEntry.type.substr(0,1))) {
			case UNSIGNED:
			{
				num.tag = Decom::Numeric::U32;
				if (length > 24)
				{
					uint8_t byte1 = m_allBytes.at(byte + offset);
					uint8_t byte2 = m_allBytes.at(byte + offset);
					uint8_t byte3 = m_allBytes.at(byte + offset);
					uint8_t byte4 = m_allBytes.at(byte + offset);

					uint16_t onetwo = mergeBytes(byte1, byte2);
					uint16_t threefour = mergeBytes(byte3, byte4);
					uint32_t all = mergeShorts(onetwo, threefour);
					if (bitupper == 0 && bitlower == 0)
						num.u32 = all;
					else
						num.u32 = BITS(all, bitlower, bitupper);
				}
				else if (length > 16)
				{
					uint8_t byte1 = m_allBytes.at(byte + offset);
					uint8_t byte2 = m_allBytes.at(byte + offset);
					uint8_t byte3 = m_allBytes.at(byte + offset);

					uint16_t onetwo = mergeBytes(byte1, byte2);
					uint32_t threefour = mergeShorts(onetwo, (uint16_t)byte3);
					if (bitupper == 0 && bitlower == 0)
						num.u32 = threefour;
					else
						num.u32 = BITS(threefour, bitlower, bitupper);
				}
				else if (length > 8)
				{
					uint8_t byte1 = m_allBytes.at(byte + offset);
					uint8_t byte2 = m_allBytes.at(byte + offset);

					uint16_t onetwo = mergeBytes(byte1, byte2);
					if (bitupper == 0 && bitlower == 0)
						num.u32 = onetwo;
					else
						num.u32 = BITS(onetwo, bitlower, bitupper);
				}
				else
				{
					uint8_t byte1 = m_allBytes.at(byte + offset);

					if (bitupper == 0 && bitlower == 0)
						num.u32 = byte1;
					else
						num.u32 = BITS(byte1, bitlower, bitupper);
				}
				break;
			}
			case SIGNED:
			{
				num.tag = Decom::Numeric::I32;
				if (length > 24)
				{
					int8_t byte1 = m_allBytes.at(byte + offset);
					int8_t byte2 = m_allBytes.at(byte + offset);
					int8_t byte3 = m_allBytes.at(byte + offset);
					int8_t byte4 = m_allBytes.at(byte + offset);

					int16_t onetwo = mergeBytesSigned(byte1, byte2);
					int16_t threefour = mergeBytesSigned(byte3, byte4);
					int32_t all = mergeShortsSigned(onetwo, threefour);
					if (bitupper == 0 && bitlower == 0)
						num.u32 = all;
					else
						num.u32 = BITS(all, bitlower, bitupper);
				}
				else if (length > 16)
				{
					int8_t byte1 = m_allBytes.at(byte + offset);
					int8_t byte2 = m_allBytes.at(byte + offset);
					int8_t byte3 = m_allBytes.at(byte + offset);

					int16_t onetwo = mergeBytesSigned(byte1, byte2);
					int32_t threefour = mergeShortsSigned(onetwo, (int16_t)byte3);
					if (bitupper == 0 && bitlower == 0)
						num.i32 = threefour;
					else
						num.i32 = BITS(threefour, bitlower, bitupper);
				}
				else if (length > 8)
				{
					int8_t byte1 = m_allBytes.at(byte + offset);
					int8_t byte2 = m_allBytes.at(byte + offset);

					int16_t onetwo = mergeBytesSigned(byte1, byte2);
					if (bitupper == 0 && bitlower == 0)
						num.i32 = onetwo;
					else
						num.i32 = BITS(onetwo, bitlower, bitupper);
				}
				else
				{
					int8_t byte1 = m_allBytes.at(byte + offset);

					if (bitupper == 0 && bitlower == 0)
						num.i32 = byte1;
					else
						num.i32 = BITS(byte1, bitlower, bitupper);
				}
				break;
			}
			case FLOAT:
			{
				int8_t byte1 = m_allBytes.at(byte + offset);
				int8_t byte2 = m_allBytes.at(byte + offset);
				int8_t byte3 = m_allBytes.at(byte + offset);
				int8_t byte4 = m_allBytes.at(byte + offset);
				int8_t byte5 = m_allBytes.at(byte + offset);
				int8_t byte6 = m_allBytes.at(byte + offset);
				int8_t byte7 = m_allBytes.at(byte + offset);
				int8_t byte8 = m_allBytes.at(byte + offset);

				int16_t onetwo = mergeBytesSigned(byte1, byte2);
				int16_t threefour = mergeBytesSigned(byte3, byte4);
				int16_t fivesix = mergeBytesSigned(byte5, byte6);
				int16_t seveneight = mergeBytesSigned(byte7, byte8);

				int32_t firsthalf = mergeShortsSigned(onetwo, threefour);
				int32_t secondhalf = mergeShortsSigned(fivesix, seveneight);
				num.tag = Decom::Numeric::F64;
				num.f64 = mergeInts(firsthalf, secondhalf);
			}
			default:
			{
				std::cerr << "Unsupported Type" << endl;
			}
		}

		vec.push_back(num);
	}
	Numeric nline;
	nline.tag = Decom::Numeric::NLINE;
	vec.push_back(nline);
	m_map[m_APID].insert(std::end(m_map[m_APID]), std::begin(vec), std::end(vec));
	m_byteIndex += m_packetLength + 1;
}

void Decom::writeData()
{
	for (const auto& apid : m_map)
	{
		std::ofstream outfile(m_instrument + "_" + std::to_string(apid.first) +".txt");
		for (const auto& data : apid.second)
		{
			switch (data.tag)
			{
			case Decom::Numeric::I32: outfile << data.i32 << '\t'; break;
			case Decom::Numeric::U32: outfile << data.u32 << '\t'; break;
			case Decom::Numeric::F64: outfile << data.f64 << '\t'; break;
			case Decom::Numeric::NLINE: outfile << '\n'; break;
			}
			outfile << ",";
		}
	}
}

BYTE Decom::getByte()
{
	return m_allBytes.at(m_byteIndex++);
}

void Decom::debugPrinter(const uint16_t& packetID, const uint16_t& packetSeqControl) const
{
	cout << "2 Bytes: " << std::bitset<16>(packetID) << endl;
	cout << "2 Bytes: " << std::bitset<16>(packetSeqControl) << endl;
	cout << "2 Bytes: " << std::bitset<16>(m_packetLength) << endl;
	cout << "SecHeader: " << m_secondaryHeader << endl;
	cout << "APID: " << m_APID << endl;
	cout << "Sequence Flag: " << std::bitset<2>(m_sequenceFlag) << endl;
	cout << "Packet Sequence: " << m_packetSequence << endl;
	cout << "Packet Length: " << m_packetLength << endl;
}

std::vector<uint8_t> Decom::getXBytes(const uint32_t& x)
{
	std::vector<uint8_t> tmp;
	for (size_t i = 0; i < x; ++i)
	{
		tmp.push_back(getByte());
	}
	return tmp;
}

void Decom::getEntries(const std::vector<entry>& entries)
{
	bool foundEntry = false;
	for (const auto& entry : entries)
	{
		if (entry.i_APID == m_APID)
		{
			m_entries.push_back(entry);
			foundEntry = true;
		}
	}
	if (!foundEntry)
	{
		std::cerr << "Couldn't find matching APID in database" << std::endl;
	}
}

entry Decom::nextEntry()
{
	return m_entries.at(m_entryIndex++);
}

Decom::DataType Decom::hashIt(std::string const& inString) {
	if (inString == "D") return UNSIGNED;
	if (inString == "S") return SIGNED;
	if (inString == "U") return UNSIGNED;
	if (inString == "F") return FLOAT;
	return NILL;
}

void Decom::switchEnds(uint32_t& begin, uint32_t& end, uint32_t length) const
{
	uint32_t tmp = begin;
	begin = length - end;
	end = length - tmp;
}