#include <iostream>
#include <algorithm>
#include "DatabaseReader.h"
#include "DataTypes.h"
#include "CSVRow.h"


/**
 * Overide stream operator for reading from our CSVRow class.
 *
 * @param str Stream to read from
 * @param data Our CSVRow object
 * @return Stream containing row
 */
std::istream& operator >> (std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}

/**
 * Helper function to read all necessary databases.
 *
 *
 * @return N/A
 */
void DatabaseReader::init()
{
	readAPIDList();
    readDatabase("databases/scdatabase.csv");
    readDatabase("databases/atmsdatabase.csv");
}

/**
 * Extracts byte and bit integers out of byte:bit string from database column.
 *
 * @param bytebit "byte:bit" string read from database
 * @param i_byte Unsigned 32-bit integer to be set by function
 * @param i_bitLower Unsigned 32-bit integer to be set by function
 * @param i_bitUpper Unsigned 32-bit integer to be set by function
 * @return None. Parameters serve as the return value
 */
void DatabaseReader::getByteBit(std::string& bytebit, uint32_t& i_byte, uint32_t& i_bitLower, uint32_t& i_bitUpper)
{
    bytebit.erase(0, 1);
    std::string s_byte = bytebit.substr(0, 4);
    try
    {
        i_byte = std::stoi(s_byte);
    }
    catch(...)
    {
        std::cerr << "stoi failed for i_byte: " << s_byte << std::endl;
    }
    if (bytebit.length() > 4)
    {
        std::string s_bit = bytebit.substr(5);
        if (s_bit.length() > 1)
        {
            size_t found;
            if ((found = s_bit.find("-")) != std::string::npos)
            {
                try
                {
                    i_bitLower = std::stoi(s_bit.substr(0, found));
                    i_bitUpper = std::stoi(s_bit.substr(found + 1));
                }
                catch(...)
                {
                    std::cerr << "stoi failed for i_bitLower or i_bitUpper: " << s_bit.substr(0,found) << "," << s_bit.substr(found + 1) << std::endl;
                }
            }
        }
        else
        {
            try
            {
                i_bitLower = std::stoi(s_bit);
            }
            catch (...)
            {
                std::cerr << "i_bitLower stoi failed with: " << s_bit << std::endl;
            }
            i_bitUpper = i_bitLower;
        }
    }
}

/**
 * Open CXXParams.csv generated by the Python script. Contains the selected the APIDS.
 * Load APIDs into a vector.
 *
 * @return NA
 */
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
            m_APIDs.emplace_back(std::stoi(apidRow[cell]));
        }
    }
}

/**
 * Read a database file. Generate a DataTypes::Entry struct for each row.
 *
 * @param filename Database file to open
 * @return N/A
 */
void DatabaseReader::readDatabase(const std::string& filename)
{
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
        catch (...)
        {
            std::cout << "i_APID stoul failed for: " << s_APID << std::endl;
        }

        std::string mnem = dataRow[0];
        DataTypes::Entry tmp = defaults;

        if (m_allAPIDs)
        {
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
        }
        else
        {
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
        }
        if(bannedAPID(mnem)) //Skip entries containing header info, as we already decode it.
            tmp.ignored = true;
        m_entries.emplace_back(tmp);
    }
    m_firstRun = true;
}

/**
 * Debug function to print what contents were read from the database.
 *
 * @return N/A
 */
void DatabaseReader::printDataBase() const
{
    for (const auto& entry : m_entries)
    {
        std::cout << entry.mnemonic << "," << entry.type << "," << entry.s_APID << "," << entry.i_APID << "," << entry.byte << "," << entry.bitLower << "," << entry.bitUpper << "\n";
    }
}

/**
 * Helper function to allow sorting of entries. Compares by APID and then by byte:bit.
 *
 * @param a Entry
 * @param b Entry
 * @return True if Entry a is "smaller"
 */
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

/**
 * Getter function for member vector containing all entries.
 * Output is sorted before being returned.
 *
 * @return
 */
std::vector<DataTypes::Entry> DatabaseReader::getEntries()
{
    std::sort(m_entries.begin(), m_entries.end(), compareByAPID);
    return m_entries;
}


/**
 * Database contains entries for decoding Primary and Secondary header information. As this information is already hardcoded, these entries can be ignored.
 * This function skips entries containing header information.
 * @param mnem Mnemonic to check
 * @return True if we should skip this mnemonic
 */
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
