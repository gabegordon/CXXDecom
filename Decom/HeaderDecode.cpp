#include "stdafx.h"
#include <iostream>
#include "HeaderDecode.h"
#include "ReadFile.h"
#include "ByteManipulation.h"

namespace HeaderDecode
{
	uint32_t sh_flag;
	DataTypes::SequenceFlag seq_flag;
	bool isValid = false;

	std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> decodeHeaders(std::ifstream& infile)
	{
		auto ph = decodePrimary(infile);
		auto sh = decodeSecondary(infile);
		return std::make_tuple(ph, sh, isValid);
	}


	DataTypes::PrimaryHeader decodePrimary(std::ifstream& infile)
	{
		DataTypes::PrimaryHeader ph = p_defaults;
		uint32_t firstFourBytes;
		uint16_t fifthSixByte;
		ReadFile::read(firstFourBytes, infile);
		ReadFile::read(fifthSixByte, infile);
		firstFourBytes = ByteManipulation::swapEndian32(firstFourBytes);
		fifthSixByte = ByteManipulation::swapEndian16(fifthSixByte);
		//Set CCSDS from bits 0-3
		ph.CCSDS = ByteManipulation::extract32(firstFourBytes, 0, 3);

		//Set secondaryHeader from bit 4
		ph.secondaryHeader = ByteManipulation::extract32(firstFourBytes, 4, 1);
		sh_flag = ph.secondaryHeader;
		
		//Set APID from bits 4-15
		ph.APID = ByteManipulation::extract32(firstFourBytes, 5, 11);
		
		//Set sequenceFlag from bits 16-17
		ph.sequenceFlag = static_cast<DataTypes::SequenceFlag>(ByteManipulation::extract32(firstFourBytes, 16, 2));
		seq_flag = ph.sequenceFlag;

		//Set packetSequence from bits 18-31
		ph.packetSequence = ByteManipulation::extract32(firstFourBytes, 18, 14);
		
		//Set packetLength from entire byte
		ph.packetLength = fifthSixByte + 1;

		//Account for secondary header length
		if (sh_flag)
		{
			if (seq_flag == DataTypes::FIRST)
				ph.packetLength -= 10;
			else
				ph.packetLength -= 8;
		}

		checkValidHeader(ph);
		return ph;
	}

	DataTypes::SecondaryHeader decodeSecondary(std::ifstream& infile)
	{
		DataTypes::SecondaryHeader sh = s_defaults;
		if (sh_flag)
		{
			uint16_t day;
			uint32_t millis;
			uint16_t micros;

			ReadFile::read(day, infile);
			ReadFile::read(millis, infile);
			ReadFile::read(micros, infile);

			sh.day = ByteManipulation::swapEndian16(day);
			sh.millis = ByteManipulation::swapEndian32(millis);
			sh.micros = ByteManipulation::swapEndian16(micros);
			if (seq_flag == DataTypes::FIRST)
			{
				//If first segmented packet, then bits 0-8 are segment count
				uint16_t packetSegments;
				ReadFile::read(packetSegments, infile);
				packetSegments = ByteManipulation::swapEndian16(packetSegments);
				sh.segments = ByteManipulation::extract16(packetSegments, 0, 8);
			}
		}
		return sh;
	}

	void checkValidHeader(const DataTypes::PrimaryHeader& pheader)
	{
		if (pheader.CCSDS != 0)
			isValid = false;
		else if (pheader.APID > 1449)
			isValid = false;
		else if (pheader.packetSequence > 16383)
			isValid = false;
		else if (pheader.packetLength > 65535)
			isValid = false;
		else if (pheader.sequenceFlag == DataTypes::FIRST && pheader.secondaryHeader == 0)
			isValid = false;
		else if (pheader.sequenceFlag == DataTypes::STANDALONE && pheader.secondaryHeader == 0)
			isValid = false;
		else if (pheader.sequenceFlag == DataTypes::MIDDLE && pheader.secondaryHeader == 1)
			isValid = false;
		else if (pheader.sequenceFlag == DataTypes::LAST && pheader.secondaryHeader == 1)
			isValid = false;
		else
			isValid = true;
	}
}