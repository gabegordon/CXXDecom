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

  DataDecode(const DataTypes::PrimaryHeader& ph, const DataTypes::SecondaryHeader& sh, std::vector<DataTypes::Entry>& entries, const bool debug) :
    m_byte1(),
    m_byte2(),
    m_byte3(),
    m_entries(entries),
    m_pHeader(ph),
    m_sHeader(sh),
    m_debug(debug)
    {};

    virtual ~DataDecode() {};

    DataTypes::Packet decodeData(std::ifstream& infile);
    DataTypes::Packet decodeDataSegmented(std::ifstream& infile);
    DataTypes::Packet decodeOMPS(std::ifstream& infile);
  private:
    uint8_t m_byte1;
    uint8_t m_byte2;
    uint8_t m_byte3;
    std::vector<DataTypes::Entry>& m_entries;
    DataTypes::PrimaryHeader m_pHeader;
    DataTypes::SecondaryHeader m_sHeader;
    bool m_debug;
    bool loadData(const std::vector<uint8_t>& buf, Bytes& bytes, const DataTypes::Entry& currEntry, const uint32_t& offset);
    void getHeaderData(DataTypes::Packet& pack);
    float getFloat(const std::vector<uint8_t>& buf, const DataTypes::Entry& currEntry, const uint32_t& offset, uint8_t initialByte);

};
