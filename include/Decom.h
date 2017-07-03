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
    m_mapEntries(),
    m_entries(entries),
    m_infile(),
    m_instrument(instrument),
    m_debug(debug),
    m_missingAPIDs(),
    m_firstRun(true),
    m_progress(0),
    m_pack()
    {};
    virtual ~Decom() {};

    void init(const std::string& infile);

  private:
    void getEntries(const uint32_t& APID);
    void formatInstruments();

    std::unordered_map<uint32_t, std::vector<DataTypes::Entry>> m_mapEntries;
    std::vector<DataTypes::Entry> m_entries;
    std::ifstream m_infile;
    std::string m_instrument;

    uint64_t m_progress;
    bool m_debug;
    bool m_firstRun;
    std::vector<uint32_t> m_missingAPIDs;
    DataTypes::Packet m_pack;

};
