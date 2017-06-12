#pragma once
#include <vector>
#include <fstream>

class Decom
{
	public:
		Decom() :
			m_packets(),
			m_infile(),
			m_complete(false),
			m_sequenceFlag(),
			m_secondaryHeader(false),
			m_APID(),
			m_packetSequence(0),
			m_packetLength(0)
		{};
		virtual ~Decom() {};

		enum SequenceFlag
		{
			MIDDLE,
			FIRST,
			LAST,
			STANDALONE
		};

		bool loadPackets(const std::string& filename);


	private:
		void readHeader();

		void readSecondHeader();

		void Decom::debugPrinter();

		std::vector<int> m_packets;

		std::ifstream m_infile;

		bool m_complete;

		bool m_secondaryHeader;

		SequenceFlag m_sequenceFlag;

		uint32_t m_APID;

		uint32_t m_packetSequence;

		uint32_t m_packetLength;

		template <class T>
		void read(T& buffer)
		{
			m_infile.read(reinterpret_cast<char *>(&buffer), sizeof(buffer));
		}
};

