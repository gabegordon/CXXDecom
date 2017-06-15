#include "stdafx.h"
#include <iostream>
#include <bitset>
#include <iterator>
#include <tuple>
#include <stdlib.h>  
#include "Decom.h"
#include "ByteManipulation.h"
#include "HeaderDecode.h"
#include "DataDecode.h"

using std::cout;
using std::endl;

void Decom::init(const std::string& infile)
{
	m_infile.open(infile, std::ios::binary | std::ios::in);
	if (!m_infile || !m_infile.is_open())
	{
		std::cerr << "Failed to open .pkt file" << std::endl;
	}
	while (true)
	{	
		//Check if we are at EOF
		int64_t currPos = m_infile.tellg();
		if (currPos == -1 || m_infile.eof())
			break;

		std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> headers = HeaderDecode::decodeHeaders(m_infile);
		
		if (!std::get<2>(headers)) //If invalid header
			break;
		
		if (m_debug)
			debugPrinter(std::get<0>(headers));

		getEntries(std::get<0>(headers).APID);
		DataTypes::Packet pack = DataDecode::decodeData(m_infile, m_matchingEntries, std::get<0>(headers).packetLength);
		m_map[std::get<0>(headers).APID].push_back(pack);
	}
	writeData();
	m_infile.close();
}


void Decom::debugPrinter(DataTypes::PrimaryHeader ph) const
{
	std::cout << ph.secondaryHeader << "," << ph.APID << "," << std::bitset<2>(ph.sequenceFlag) << "," << ph.packetSequence << "," << ph.packetLength << "\n";
}


void Decom::getEntries(const uint32_t& APID)
{
	m_matchingEntries.clear();
	bool foundEntry = false;
	for (const auto& entry : m_entries)
	{
		if (entry.i_APID == APID)
		{
			m_matchingEntries.push_back(entry);
			foundEntry = true;
		}
	}
	if (!foundEntry)
	{
			std::cerr << "Couldn't find matching APID in database" << std::endl;
	}
}

void Decom::writeData()
{
	for (const auto& apid : m_map)
	{
		std::ofstream outfile(m_instrument + "_" + std::to_string(apid.first) + ".txt");
		
		for (const DataTypes::Numeric& num : apid.second.at(0).data)
		{
			outfile << num.mnem << "," << "\t";
		}

		outfile << "\n";

		for (const DataTypes::Packet& pack: apid.second)
		{
			for (const DataTypes::Numeric& num : pack.data)
			{
				switch (num.tag)
				{
				case DataTypes::Numeric::I32: outfile << num.i32 << "," << '\t'; break;
				case DataTypes::Numeric::U32: outfile << num.u32 << "," << '\t'; break;
				case DataTypes::Numeric::F64: outfile << num.f64 << "," << '\t'; break;
				}
			}
			outfile << "\n";
		}
		outfile.close();
	}
}
