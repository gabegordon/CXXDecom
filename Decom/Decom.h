#pragma once
#include <vector>
#include <fstream>
#include <unordered_map>
#include "DataTypes.h"

typedef uint8_t BYTE;

class Decom
{
	public:
		Decom(const std::string& instrument, const bool& debug, const std::vector<DataTypes::Entry>& entries) :
			m_map(),
			m_entries(entries),
			m_matchingEntries(),
			m_infile(),
			m_instrument(instrument),
			m_debug(debug)
		{};
		virtual ~Decom() {};

		void init(const std::string& infile);

	private:
		void debugPrinter(DataTypes::PrimaryHeader) const;
		void Decom::getEntries(const DataTypes::PrimaryHeader& pheader);

		std::unordered_map<uint32_t, std::vector<DataTypes::Packet>> m_map;
		std::vector<DataTypes::Entry> m_entries;
		std::vector<DataTypes::Entry> m_matchingEntries;
		std::ifstream m_infile;
		std::string m_instrument;
		bool m_debug;
};

