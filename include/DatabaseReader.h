#pragma once
#include <string>
#include <vector>
#include "DataTypes.h"

class DatabaseReader
{
  public:
  DatabaseReader(const std::string& paramsfile, const bool allAPIDs) :
    m_APIDs(),
    m_entries(),
    m_firstRun(true),
    m_allAPIDs(allAPIDs),
    m_paramsFile(paramsfile)
    {
        init();
    };

    virtual ~DatabaseReader() {};

    std::vector<DataTypes::Entry> getEntries();

  private:
    std::vector<uint32_t> m_APIDs;
    std::vector<DataTypes::Entry> m_entries;
    std::vector<std::string> m_skip = {"PVNO", "PPTYPE", "PSHDF", "PID", "PSEGF", "PSCNT", "PLEN", "PTDAY", "PTMS", "PTUS", "ORCV", "OVER", "OTCNT", "OTFLG", "C_RDR_VER", "C_CONT_COUNT", "C_CONT_FLAG"};
    bool m_firstRun;
    bool m_allAPIDs;
    std::string m_paramsFile;

    void init();
    void readDatabase(const std::string& filename);
    void getByteBit(std::string& bytebit, uint32_t& i_byte, uint32_t& i_bitLower, uint32_t& i_bitUpper);
    void readAPIDList();
    void printDataBase() const;
    bool bannedAPID(std::string& mnem);
    const struct DataTypes::Entry defaults = {
        "",DataTypes::NILL,0,0,0,0,true
    };
};
