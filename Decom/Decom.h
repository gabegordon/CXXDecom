#pragma once
#include <vector>
#include <fstream>
#include "Entry.h"

class Decom
{
	public:
		Decom(const std::string& instrument) :
			m_packets(),
			m_infile(),
			m_complete(false),
			m_sequenceFlag(),
			m_secondaryHeader(false),
			m_APID(),
			m_packetSequence(0),
			m_packetLength(0),
			m_segments(0),
			m_output(),
			m_entry(),
			m_instrument(instrument)
		{};
		virtual ~Decom() {};

		enum SequenceFlag
		{
			MIDDLE,
			FIRST,
			LAST,
			STANDALONE
		};

		bool loadPackets(const std::string& filename, const std::vector<entry>& entries);


	private:
		void readHeader();

		void readSecondHeader();

		void debugPrinter() const;

		void readData(const std::vector<entry>& entries);

		void writeData();

		std::vector<int> m_packets;

		std::ifstream m_infile;

		bool m_complete;

		uint32_t m_secondaryHeader;

		SequenceFlag m_sequenceFlag;

		uint32_t m_APID;

		uint32_t m_packetSequence;

		uint32_t m_packetLength;

		uint32_t m_segments;

		std::vector<std::string> m_output;

		entry m_entry;

		std::string m_instrument;

		template <class T>
		void read(T& buffer)
		{
			m_infile.read(reinterpret_cast<char *>(&buffer), sizeof(buffer));
		}
};

