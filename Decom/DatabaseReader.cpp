#include "stdafx.h"
#include <iostream>
#include "DatabaseReader.h"
#include "CSVRow.h"


std::istream& operator >> (std::istream& str, CSVRow& data)
{
	data.readNextRow(str);
	return str;
}

void DatabaseReader::getByteBit(std::string& bytebit, int32_t& i_byte, int32_t& i_bitLower, int32_t& i_bitUpper)
{
	bytebit.erase(0, 1);
	std::string s_byte = bytebit.substr(0, 4);
	i_byte = std::stoi(s_byte);
	if (bytebit.length() > 4)
	{
		std::string s_bit = bytebit.substr(5);
		if (s_bit.length() > 1)
		{
			size_t found;
			if ((found = s_bit.find("-")) != std::string::npos)
			{
				i_bitLower = std::stoi(s_bit.substr(0, found));
				i_bitUpper = std::stoi(s_bit.substr(found + 1));
			}
		}
		else
		{
			i_bitLower = std::stoi(s_bit);
		}
	}

}

void DatabaseReader::readAPIDList()
{
	std::ifstream CXXParams("C:\\JPSS\\CXXParams.csv");
	if (!CXXParams)
	{
		std::cerr << "Could not find CXXParams" << std::endl;
	}

	CSVRow apidRow;
	while (CXXParams >> apidRow)
	{
		for (size_t cell = 0; cell < apidRow.size(); ++cell)
		{
			m_APIDs.push_back(std::stoi(apidRow[cell]));
		}
	}
}

void DatabaseReader::readDatabase(const std::string& filename)
{
	readAPIDList();
	std::ifstream database(filename);
	if (!database)
	{
		std::cerr << "Could not find Database" << std::endl;
	}

	CSVRow dataRow;
	while (database >> dataRow)
	{
		if (m_firstRun)
		{
			m_firstRun = false;
			continue;
		}
		std::string s_APID = dataRow[4];
		s_APID.erase(0, 4);
		uint32_t i_APID = atoi(s_APID.c_str());

		if (std::find(m_APIDs.begin(), m_APIDs.end(), i_APID) != m_APIDs.end())
		{
			DataTypes::Entry tmp = defaults;
			tmp.mnemonic = dataRow[0];
			tmp.SS = dataRow[2];
			tmp.type = dataRow[3];
			tmp.s_APID = dataRow[4];
			tmp.i_APID = i_APID;

			std::string bytebit = dataRow[5];
			int32_t i_byte = -1;
			int32_t i_bitLower = -1;
			int32_t i_bitUpper = -1;
			getByteBit(bytebit, i_byte, i_bitLower, i_bitUpper);

			tmp.byte = i_byte;
			tmp.bitLower = i_bitLower;
			tmp.bitUpper = i_bitUpper;
			m_entries.push_back(tmp);
		}
	}
}

void DatabaseReader::printDataBase() const
{
	for (const auto& entry : m_entries)
	{
		std::cout << entry.mnemonic << "," << entry.SS << "," << entry.type << "," << entry.s_APID << "," << entry.i_APID << "," << entry.byte << "," << entry.bitLower << "," << entry.bitUpper << "\n";
	}
}

std::vector<DataTypes::Entry> DatabaseReader::getEntries() const
{
	return m_entries;
}