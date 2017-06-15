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
		m_firstRun(true),
		m_allAPIDs(true)
	{
		readDatabase(filename);
	};

	virtual ~DatabaseReader() {};

	std::vector<DataTypes::Entry> getEntries() const;

private:
	std::vector<uint32_t> m_APIDs;
	std::vector<DataTypes::Entry> m_entries;
	bool m_firstRun;
	bool m_allAPIDs;

	void readDatabase(const std::string& filename);
	void getByteBit(std::string& bytebit, uint32_t& i_byte, uint32_t& i_bitLower, uint32_t& i_bitUpper);
	void readAPIDList();
	void printDataBase() const;

	const struct DataTypes::Entry defaults = {
		"","","","",0,0,0,0,
	};
};

