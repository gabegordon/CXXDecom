#pragma once
#include <fstream>
#include "DataTypes.h"

class DataDecode
{
public:
	enum Bytes
	{
		ONE,
		TWO,
		THREE,
		FOUR
	};

	DataDecode(const DataTypes::PrimaryHeader& ph, const DataTypes::SecondaryHeader& sh, std::vector<DataTypes::Entry>& entries) :
		m_byte0(),
		m_byte1(),
		m_byte2(),
		m_byte3(),
		m_entries(entries),
		m_pHeader(ph),
		m_sHeader(sh)
	{};

	virtual ~DataDecode() {};

	DataTypes::Packet decodeData(std::ifstream& infile);
	DataTypes::Packet decodeDataSegmented(std::ifstream& infile);

private:
	uint8_t m_byte0;
	uint8_t m_byte1;
	uint8_t m_byte2;
	uint8_t m_byte3;
	std::vector<DataTypes::Entry>& m_entries;
	DataTypes::PrimaryHeader m_pHeader;
	DataTypes::SecondaryHeader m_sHeader;

	bool loadData(const std::vector<uint8_t>& buf, Bytes& bytes, const DataTypes::Entry& currEntry);
	void getHeaderData(DataTypes::Packet& pack);
};

