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

bool Decom::loadPackets(const std::string& filename)
{
	m_infile.open(filename, std::ios::binary | std::ios::in);
	while (!m_complete)
	{
		
		readHeader();
		if (m_secondaryHeader)
		{
			readSecondHeader();
		}
		m_complete = true;
	}

	debugPrinter();
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
	m_packetLength = buffer;																 //2 bytes for packet length
	cout << "2 Bytes: " << std::bitset<16>(buffer) << endl;

}

void Decom::readSecondHeader()
{
	uint64_t m_timeCode;
	read(m_timeCode);
	if (m_packetSequence == FIRST)
	{
		uint16_t buffer;
		read(buffer);
	}
}

void Decom::debugPrinter()
{
	cout << "SecHeader: " << m_secondaryHeader << endl;
	cout << "APID: " << m_APID << endl;
	cout << "Sequence Flag: " << std::bitset<2>(m_sequenceFlag) << endl;
	cout << "Packet Sequence: " << m_packetSequence << endl;
	cout << "Packet Length: " << m_packetLength << endl;
}

