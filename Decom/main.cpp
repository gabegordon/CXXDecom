#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <algorithm>
#include "CSVRow.h"
#include "Decom.h"

using std::cout;
using std::endl;

struct entry {
	std::string mnemonic;
	std::string description;
	std::string SS;
	std::string type;
	std::string s_APID;
	int32_t i_APID;
	int32_t byte;
	int32_t bitLower;
	int32_t bitUpper;
};

std::vector<uint32_t> APIDs;
std::vector<entry> entries;
std::string instrument;
bool firstRun = true;



std::istream& operator >> (std::istream& str, CSVRow& data)
{
	data.readNextRow(str);
	return str;
}

void getByteBit(std::string& bytebit, int32_t& i_byte, int32_t& i_bitLower, int32_t& i_bitUpper) 
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
				i_bitUpper = std::stoi(s_bit.substr(found+1));
			}
		}
		else
		{
			i_bitLower = std::stoi(s_bit);
		}
	}

}

bool readAPIDList()
{
	std::ifstream CXXParams("C:\\JPSS\\CXXParams.csv");
	if (!CXXParams)
	{
		return false;
	}

	CSVRow apidRow;
	while (CXXParams >> apidRow)
	{
		for (size_t cell = 0; cell < apidRow.size(); ++cell)
		{
			APIDs.push_back(std::stoi(apidRow[cell]));
		}
	}

	return true;
}

bool readDatabase(const std::string filename)
{
	std::ifstream database(filename);
	if (!database)
	{
		return false;
	}

	CSVRow dataRow;
	while (database >> dataRow)
	{
		if (firstRun)
		{
			firstRun = false;
			continue;
		}
		std::string s_APID = dataRow[4];
		s_APID.erase(0, 4);
		uint32_t i_APID = atoi(s_APID.c_str());

		if (std::find(APIDs.begin(), APIDs.end(), i_APID) != APIDs.end())
		{
			entry tmp;
			tmp.mnemonic = dataRow[0];
			tmp.description = dataRow[1];
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
			entries.push_back(tmp);
		}
	}

	return true;
}

void printDataBase()
{
	for(auto& entry : entries)
	{
		cout << entry.mnemonic << "," << entry.description << "," << entry.SS << "," << entry.type << "," << entry.s_APID << "," << entry.i_APID << "," << entry.byte << "," << entry.bitLower << "," << entry.bitUpper << "\n";
	}
}


int main(int argc, char* argv[])
{	
	Decom decomEngine;
	if (argc < 4)
	{
		cout << "Specify: database and instrument\n";
		return 0;
	}
	else
	{
		if (!readAPIDList())
		{
			std::cerr << "Could not find CXXParams" << endl;
			return 0;
		}
		std::string filename = argv[1];
		cout << filename << endl;
		instrument = argv[2];
		//if (!readDatabase(filename))
		//{
		//	std::cerr << "Could not find Database" << endl;
		//	return 0;
		//}
		if (!decomEngine.loadPackets(argv[3]))
		{
			std::cerr << "Error in decom engine" << endl;
		}
	}

	//printDataBase();
    return 0;
}

