#include <iostream>
#include <algorithm>
#include "DatabaseReader.h"
#include "DataTypes.h"
#include "CSVRow.h"


std::istream& operator >> (std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}

void DatabaseReader::init()
{
    readDatabase("databases/scdatabase.csv");
    readDatabase("databases/atmsdatabase.csv");
}
void DatabaseReader::getByteBit(std::string& bytebit, uint32_t& i_byte, uint32_t& i_bitLower, uint32_t& i_bitUpper)
{
    bytebit.erase(0, 1);
    std::string s_byte = bytebit.substr(0, 4);
    i_byte = std::stoi(s_byte);
    if (bytebit.length() > 4)
    {
        std::string s_bit = bytebit.substr(5);
        if (s_bit.length() > 1)
        {
            size_t found;
            if ((found = s_bit.find("-")) != std::string::npos)
            {
                i_bitLower = std::stoi(s_bit.substr(0, found));
                i_bitUpper = std::stoi(s_bit.substr(found + 1));
            }
        }
        else
        {
            i_bitLower = std::stoi(s_bit);
            i_bitUpper = i_bitLower;
        }
    }
}

void DatabaseReader::readAPIDList()
{
    std::ifstream CXXParams(m_paramsFile);
    if (!CXXParams || !CXXParams.is_open())
    {
        std::cerr << "Could not find CXXParams" << std::endl;
        system("pause");
        exit(0);
    }

    CSVRow apidRow;
    while (CXXParams >> apidRow)
    {
        for (size_t cell = 0; cell < apidRow.size(); ++cell)
        {
            m_APIDs.push_back(std::stoi(apidRow[cell]));
        }
    }
}

void DatabaseReader::readDatabase(const std::string& filename)
{
    readAPIDList();
    std::ifstream database(filename);
    if (!database || !database.is_open())
    {
        std::cerr << "Could not find Database" << std::endl;
        system("pause");
        exit(0);
    }

    CSVRow dataRow;
    while (database >> dataRow)
    {
        if (m_firstRun)
        {
            m_firstRun = false;
            continue;
        }

        std::string s_APID = dataRow[2];
        s_APID.erase(0, 4);
        uint32_t i_APID = 0;

        try
        {
            i_APID = std::stoul(s_APID);
        }
        catch (std::invalid_argument e)
        {
            std::cout << "Invalid argument " << s_APID << std::endl;
        }

        std::string mnem = dataRow[0];
        if(bannedAPID(mnem)) //Skip entries containing header info, as we already decode it.
            continue;

        if (m_allAPIDs)
        {
            DataTypes::Entry tmp = defaults;
            tmp.mnemonic = mnem;
            tmp.type = DataTypes::hashIt(dataRow[1].substr(0, 1));
            tmp.s_APID = s_APID;
            tmp.i_APID = i_APID;

            std::string bytebit = dataRow[3];
            uint32_t i_byte = 0;
            uint32_t i_bitLower = 0;
            uint32_t i_bitUpper = 0;
            getByteBit(bytebit, i_byte, i_bitLower, i_bitUpper);

            tmp.byte = i_byte;
            tmp.bitLower = i_bitLower;
            tmp.bitUpper = i_bitUpper;
            tmp.length = std::stoi(dataRow[1].substr(1, std::string::npos));
            tmp.ignored = false;
            m_entries.push_back(tmp);
        }
        else
        {
            DataTypes::Entry tmp = defaults;
            tmp.ignored = true;
            if (std::find(m_APIDs.begin(), m_APIDs.end(), i_APID) != m_APIDs.end())
            {
                tmp.ignored = false;
            }
            tmp.mnemonic = mnem;
            tmp.type = DataTypes::hashIt(dataRow[1].substr(0, 1));
            tmp.s_APID = s_APID;
            tmp.i_APID = i_APID;

            std::string bytebit = dataRow[3];
            uint32_t i_byte = 0;
            uint32_t i_bitLower = 0;
            uint32_t i_bitUpper = 0;
            getByteBit(bytebit, i_byte, i_bitLower, i_bitUpper);

            tmp.byte = i_byte;
            tmp.bitLower = i_bitLower;
            tmp.bitUpper = i_bitUpper;
            tmp.length = std::stoi(dataRow[1].substr(1, std::string::npos));
            m_entries.push_back(tmp);
        }
    }
    m_firstRun = true;
}

void DatabaseReader::printDataBase() const
{
    for (const auto& entry : m_entries)
    {
        std::cout << entry.mnemonic << "," << entry.type << "," << entry.s_APID << "," << entry.i_APID << "," << entry.byte << "," << entry.bitLower << "," << entry.bitUpper << "\n";
    }
}

bool compareByAPID(const DataTypes::Entry& a, const DataTypes::Entry& b)
{
    if(a.i_APID != b.i_APID)
        return a.i_APID < b.i_APID;
    else
    {
        if(a.byte != b.byte)
            return a.byte < b.byte;
        else
            return a.bitLower < b.bitLower;
    }
}

std::vector<DataTypes::Entry> DatabaseReader::getEntries()
{
    std::sort(m_entries.begin(), m_entries.end(), compareByAPID);
    return m_entries;
}

bool DatabaseReader::bannedAPID(std::string& mnem)
{
    std::string strippedmnem;
    if (mnem.length() > 6)
        strippedmnem = mnem.substr(0, 1) + mnem.substr(5);
    else
        return false;
    if (std::find(m_skip.begin(), m_skip.end(), strippedmnem) != m_skip.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}