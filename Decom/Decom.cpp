#include "stdafx.h"
#include <iostream>
#include <bitset>
#include <stdlib.h>  
#include "Decom.h"
#include "ByteManipulation.h"

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define BITS(k,m,n) LAST((k)>>(m),((n)-(m)))

using namespace ByteManipulation;
using std::cout;
using std::endl;

bool Decom::loadPackets(const std::string& filename, const std::vector<entry>& entries)
{
	m_infile.open(filename, std::ios::binary | std::ios::in);
	while (!m_complete)
	{
		
		readHeader();
		if (m_secondaryHeader)
		{
			readSecondHeader();
		}
		readData(entries);
		debugPrinter();
		m_secondaryHeader = false;
		if (m_sequenceFlag == LAST || m_sequenceFlag == STANDALONE)
		{
			m_complete = true;
		}
	}
	if (m_output.empty())
	{
		return false;
	}
	return true;
}


void Decom::readHeader()
{
	uint16_t buffer;

	read(buffer);
	m_secondaryHeader = BITS(buffer, 11, 12);												//0 or 1 flag located at 5th bit (11th from right)
	m_APID = BITS(buffer, 0, 11);
	cout << "2 Bytes: " << std::bitset<16>(buffer) << endl;

	read(buffer);
	m_sequenceFlag = static_cast<SequenceFlag>(BITS(buffer, 14, 16));						//00-Cont,01-First,10-Last,11-Stand flag for sequence
	m_packetSequence = BITS(buffer, 0, 14);												    //14 bits for sequence number
	cout << "2 Bytes: " << std::bitset<16>(buffer) << endl;

	read(buffer);
	m_packetLength = buffer;																//2 bytes for packet length
	cout << "2 Bytes: " << std::bitset<16>(buffer) << endl;

}

void Decom::readSecondHeader()
{
	uint64_t m_timeCode;																	//8 bytes for timecode
	read(m_timeCode);
	if (m_packetSequence == FIRST)
	{
		uint16_t buffer;
		read(buffer);
		m_segments = BITS(buffer, 7, 16);													//First 1 byte for segment count
	}
}

void Decom::debugPrinter() const
{
	cout << "SecHeader: " << m_secondaryHeader << endl;
	cout << "APID: " << m_APID << endl;
	cout << "Sequence Flag: " << std::bitset<2>(m_sequenceFlag) << endl;
	cout << "Packet Sequence: " << m_packetSequence << endl;
	cout << "Packet Length: " << m_packetLength << endl;
}

void Decom::readData(const std::vector<entry>& entries)
{
	bool foundEntry = false;
	for (const auto& entry : entries)
	{
		if (entry.i_APID == m_APID)
		{
			m_entry = entry;
			foundEntry = true;
			break;
		}
	}
	if (!foundEntry)
	{
		std::cerr << "Couldn't find matching APID in database" << std::endl;
	}

	if (m_secondaryHeader)
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
	for (size_t octet = 0; octet < m_packetLength; ++octet)
	{
		uint8_t buffer;
		read(buffer);
		m_output.push_back(std::to_string(buffer));
	}
	writeData();
}

void Decom::writeData()
{
	std::ofstream outfile(m_entry.mnemonic+".txt");
	size_t vsize = m_output.size();
	for (size_t n = 0; n < vsize; ++n)
	{
		outfile << m_output.at(n) << '\t';
		outfile << ",";
	}
}