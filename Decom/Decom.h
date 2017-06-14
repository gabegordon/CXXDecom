#pragma once
#include <vector>
#include <fstream>
#include <unordered_map>
#include "Entry.h"

typedef uint8_t BYTE;

class Decom
{
	public:
		Decom(const std::string& instrument, const bool& debug) :
			m_allBytes(),
			m_map(),
			m_infile(),
			m_complete(false),
			m_sequenceFlag(),
			m_secondaryHeader(false),
			m_APID(),
			m_packetSequence(0),
			m_packetLength(0),
			m_segments(0),
			m_byteIndex(0),
			m_entryIndex(0),
			m_pastAPID(),
			m_entries(),
			m_instrument(instrument),
			m_debug(debug)
		{};
		virtual ~Decom() {};

		enum SequenceFlag
		{
			MIDDLE,
			FIRST,
			LAST,
			STANDALONE
		};

		enum DataType
		{
			NILL,
			SIGNED,
			UNSIGNED,
			FLOAT
		};

		struct Numeric
		{
			enum { U32, I32, F64, NLINE } tag;
			union
			{
				uint32_t u32;
				int32_t i32;
				float f64;
			};
		};

		struct device
		{
			std::vector<Numeric> data;
		};

		struct APIDGroup
		{
			std::string name;
			std::vector<device> devices;
		};

		bool loadPackets(const std::string& filename, const std::vector<entry>& entries);

	private:
		void loadToMemory(const std::string& filename);

		void readHeader();

		void readSecondHeader();

		void debugPrinter(const uint16_t& packetID, const uint16_t& packetSeqControl) const;

		void readData(const std::vector<entry>& entries);

		void writeData();

		void getEntries(const std::vector<entry>& entries);
		
		DataType hashIt(std::string const& inString);

		entry nextEntry();

		BYTE Decom::getByte();

		std::vector<uint8_t> getXBytes(const uint32_t& x);

		void switchEnds(uint32_t& begin, uint32_t& end, uint32_t length) const;

		std::vector<BYTE> m_allBytes;

		std::unordered_map<uint32_t, std::vector<Numeric>> m_map;

		std::ifstream m_infile;

		bool m_complete;

		uint32_t m_secondaryHeader;

		SequenceFlag m_sequenceFlag;

		uint32_t m_APID;

		uint32_t m_packetSequence;

		uint32_t m_packetLength;

		uint32_t m_segments;
		
		uint32_t m_byteIndex;

		uint32_t m_entryIndex;

		uint32_t m_pastAPID;

		std::vector<entry> m_entries;

		std::string m_instrument;

		bool m_debug;

		template <typename T>
		void read(T& buffer)
		{
			m_infile.read(reinterpret_cast<char *>(&buffer), sizeof(buffer));
		}
};

