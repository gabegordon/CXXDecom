#include "stdafx.h"
#include <iostream>
#include <stdlib.h>  
#include "HeaderDecode.h"
#include "ReadFile.h"
#include "ByteManipulation.h"

#define LAST(k,n) ((k) & ((1<<(n))-1))
#define BITS(k,m,n) LAST((k)>>(m),((n)-(m)))

using namespace DataTypes;
using namespace ByteManipulation;
using namespace ReadFile;

namespace HeaderDecode
{
	uint32_t sh_flag;
	DataTypes::SequenceFlag seq_flag;

	std::pair<PrimaryHeader, SecondaryHeader> decodeHeaders(std::ifstream& infile)
	{
		return std::make_pair<PrimaryHeader, SecondaryHeader>(decodePrimary(infile), decodeSecondary(infile));
	}


	PrimaryHeader decodePrimary(std::ifstream& infile)
	{
		PrimaryHeader ph = p_defaults;
		uint32_t firstFourBytes;
		uint16_t fifthSixByte;
		read(firstFourBytes, infile);
		read(fifthSixByte, infile);
		firstFourBytes = swapEndian32(firstFourBytes);
		fifthSixByte = swapEndian16(fifthSixByte);

		//Set secondaryHeader from bit 4
		ph.secondaryHeader = extract32(firstFourBytes, 4, 1);
		sh_flag = ph.secondaryHeader;
		
		//Set APID from bits 4-15
		ph.APID = extract32(firstFourBytes, 5, 11);
		
		//Set sequenceFlag from bits 16-17
		ph.sequenceFlag = static_cast<SequenceFlag>(extract32(firstFourBytes, 16, 2));
		
		//Set packetSequence from bits 18-31
		ph.packetSequence = extract32(firstFourBytes, 18, 14);
		
		//Set packetLength from entire byte
		ph.packetLength = fifthSixByte;

		std::cout << ph.secondaryHeader << "," << ph.APID << "," << std::bitset<2>(ph.sequenceFlag) << "," << ph.packetSequence << "," << ph.packetLength;
		return ph;
	}

	SecondaryHeader decodeSecondary(std::ifstream& infile)
	{
		SecondaryHeader sh = s_defaults;
		if (sh_flag)
		{
			if (seq_flag == DataTypes::FIRST)
			{

			}
		}
		return sh;
	}
}