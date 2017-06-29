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
    m_mapEntries(),
    m_outfiles(),
    m_entries(entries),
    m_infile(),
    m_instrument(instrument),
    m_debug(debug),
    m_missingAPIDs(),
    m_firstRun(true)
    {};
    virtual ~Decom() {};

    void init(const std::string& infile);

  private:
    void getEntries(const uint32_t& APID);
    void writeData();
    uint64_t getLength();
    bool checkForMissingOutput();
    void formatInstruments();

    std::unordered_map<uint32_t, std::vector<DataTypes::Packet>> m_map;
    std::unordered_map<uint32_t, std::vector<DataTypes::Entry>> m_mapEntries;
    std::unordered_map<uint32_t, std::ofstream> m_outfiles;
    std::vector<DataTypes::Entry> m_entries;
    std::ifstream m_infile;
    std::string m_instrument;
    bool m_debug;
    bool m_firstRun;
    std::vector<uint32_t> m_missingAPIDs;
};
