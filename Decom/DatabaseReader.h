#pragma once
#include <string>
#include <vector>
#include "DataTypes.h"

class DatabaseReader
{
public:
	DatabaseReader(const std::string& filename) :
		m_APIDs(),
		m_entries(),
		m_firstRun()
	{
		readDatabase(filename);
	};

	virtual ~DatabaseReader() {};

	std::vector<DataTypes::Entry> getEntries() const;

private:
	std::vector<uint32_t> m_APIDs;
	std::vector<DataTypes::Entry> m_entries;
	bool m_firstRun;

	void readDatabase(const std::string& filename);
	void getByteBit(std::string& bytebit, int32_t& i_byte, int32_t& i_bitLower, int32_t& i_bitUpper);
	void readAPIDList();
	void printDataBase() const;

	const struct DataTypes::Entry defaults = {
		"","","","",0,0,0,0,
	};
};

