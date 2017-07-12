#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <fstream>
#include <tuple>
#include <string>
#include "DataTypes.h"

typedef uint8_t BYTE;

class Decom
{
  public:
  Decom(const std::string& instrument, const bool& debug, const std::vector<DataTypes::Entry>& entries) :
    m_mapEntries{},
    m_entries{entries},
    m_instrument{instrument},
    m_progress{0},
    m_debug{debug},
    m_headers{}
    {};
    virtual ~Decom() {}

    void init(const std::string& infile);

  private:
    void getEntries(const uint32_t& APID);
    void formatInstruments() const;
    void storeAPID(const uint32_t& APID);
    uint64_t getFileSize();
    DataTypes::Packet decodeData();
    bool getHeadersAndEntries();

    std::unordered_map<uint32_t, std::vector<DataTypes::Entry>> m_mapEntries;
    std::vector<DataTypes::Entry> m_entries;
    std::ifstream m_infile;
    std::string m_instrument;

    uint64_t m_progress;
    bool m_debug;
    std::vector<uint32_t> m_missingAPIDs;
    std::set<uint32_t> m_APIDs;
    std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> m_headers;
};
