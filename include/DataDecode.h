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

  DataDecode(const DataTypes::PrimaryHeader& ph, const DataTypes::SecondaryHeader& sh, std::vector<DataTypes::Entry>& entries, const bool debug, const std::string& instrument) :
    m_byte1(),
    m_byte2(),
    m_byte3(),
    segmentLastByte(0),
    m_Instrument(instrument),
    m_entries(entries),
    m_pHeader(ph),
    m_sHeader(sh),
    m_debug(debug),
    m_segmented(false)
    {};

    virtual ~DataDecode() {};

    DataTypes::Packet decodeData(std::ifstream& infile, const uint32_t& index);
    DataTypes::Packet decodeDataSegmented(std::ifstream& infile, const bool omps);
    DataTypes::Packet decodeOMPS(std::ifstream& infile);
  private:
    uint8_t m_byte1;
    uint8_t m_byte2;
    uint8_t m_byte3;
    uint32_t segmentLastByte;
    std::string m_Instrument;
    std::vector<DataTypes::Entry>& m_entries;
    DataTypes::PrimaryHeader m_pHeader;
    DataTypes::SecondaryHeader m_sHeader;
    bool m_debug;
    bool m_segmented;
    bool loadData(const std::vector<uint8_t>& buf, Bytes& bytes, const DataTypes::Entry& currEntry, const uint32_t& offset);
    void getHeaderData(DataTypes::Packet& pack);
    float getFloat(const std::vector<uint8_t>& buf, const DataTypes::Entry& currEntry, const uint32_t& offset, uint8_t initialByte);
    uint8_t getOffset();
};
