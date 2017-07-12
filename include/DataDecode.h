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
    m_initialByte{},
    m_byte1{},
    m_byte2{},
    m_byte3{},
    segmentLastByte{0},
    m_Instrument{instrument},
    m_entries{entries},
    m_pHeader{ph},
    m_sHeader{sh},
    m_debug{debug},
    m_offset{}
    {};

    virtual ~DataDecode() {};

    DataTypes::Packet decodeData(std::ifstream& infile, const uint32_t& index);
    DataTypes::Packet decodeDataSegmented(std::ifstream& infile, const bool omps);
    DataTypes::Packet decodeOMPS(std::ifstream& infile);

  private:
    uint8_t m_initialByte;
    uint8_t m_byte1;
    uint8_t m_byte2;
    uint8_t m_byte3;
    uint32_t segmentLastByte;
    std::string m_Instrument;
    std::vector<DataTypes::Entry>& m_entries;
    DataTypes::PrimaryHeader m_pHeader;
    DataTypes::SecondaryHeader m_sHeader;
    bool m_debug;
    uint8_t m_offset;
    Bytes m_numBytes;

    bool loadData(const std::vector<uint8_t>& bufs, const DataTypes::Entry& currEntry);
    void getHeaderData(DataTypes::Packet& pack);
    float getFloat(const std::vector<uint8_t>& buf, const DataTypes::Entry& currEntry);
    uint8_t getOffset();
    DataTypes::Numeric getNum(const DataTypes::DataType& dtype, const std::vector<uint8_t>& buf, const uint32_t& entryIndex);
    bool checkPackEntries(DataTypes::Packet& pack);
    void decodeOne(const DataTypes::DataType& dtype, const uint32_t& entryIndex, DataTypes::Numeric& num);
    void decodeTwo(const DataTypes::DataType& dtype, const uint32_t& entryIndex, DataTypes::Numeric& num);
    void decodeThree(const DataTypes::DataType& dtype, const uint32_t& entryIndex, DataTypes::Numeric& num);
    void decodeFour(const DataTypes::DataType& dtype, const uint32_t& entryIndex, DataTypes::Numeric& num);
};
