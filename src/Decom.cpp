#include <iostream>
#include <bitset>
#include <iterator>
#include <tuple>
#include <iomanip>
#include <stdlib.h>
#include "Decom.h"
#include "ByteManipulation.h"
#include "HeaderDecode.h"
#include "DataDecode.h"
#include "ProgressBar.h"
#include "InstrumentFormat.h"

using std::cout;
using std::endl;

/**
 * Decom engine initializer. Main event loop for calling all Decom helper functions. Stops reading upon reaching end of input file. Writes data once finished reading.
 * Handles instrument formatting after writing (if necessary).
 *
 * @param infile File to read from
 * @return N/A
 */
void Decom::init(const std::string& infile)
{
    m_infile.open(infile, std::ios::binary | std::ios::in);
    if (!m_infile || !m_infile.is_open())
    {
        std::cerr << "Failed to open .pkt file" << std::endl;
        system("pause");
        exit(0);
    }
    m_infile.seekg(0, std::ios::end);
    uint64_t fileSize = m_infile.tellg();
    m_infile.seekg(0, std::ios::beg);
    ProgressBar readProgress(fileSize, "Read Packet");
    uint64_t progress = 0;
    uint64_t writeCounter = 0;
    while (true)
    {
        progress = m_infile.tellg();
        readProgress.Progressed(progress);
        if (m_infile.eof() || progress >= fileSize)
            break;

        std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> headers = HeaderDecode::decodeHeaders(m_infile, m_debug);

        if(!std::get<2>(headers))
            break;

        getEntries(std::get<0>(headers).APID);

        DataTypes::Packet pack;
        DataDecode dc(std::get<0>(headers), std::get<1>(headers), m_mapEntries[std::get<0>(headers).APID], m_debug, m_instrument);

        if(m_instrument == "OMPS")
            pack = std::move(dc.decodeOMPS(m_infile));
        else if (std::get<0>(headers).sequenceFlag == DataTypes::FIRST)
            pack = std::move(dc.decodeDataSegmented(m_infile));
        else
            pack = std::move(dc.decodeData(m_infile,0));

        m_map[std::get<0>(headers).APID].emplace_back(pack);
        if(writeCounter % 400000 == 0)
        {
			//return;
            writeData();
            while (m_map.size())
                m_map.erase(m_map.begin());
        }
        writeCounter++;
    }
    writeData();
    m_infile.close();
    formatInstruments();
}

/**
 * Finds database entries that match our current APID.
 *
 * @param APID APID to search for
 * @return N/A
 */
void Decom::getEntries(const uint32_t& APID)
{
    if(std::find(std::begin(m_missingAPIDs), std::end(m_missingAPIDs), APID) != std::end(m_missingAPIDs)){
        return;
    }
    if (m_mapEntries[APID].empty())
    {
        bool foundEntry = false;
        for (const auto& entry : m_entries)
        {
            if (entry.i_APID == APID)
            {
                if(!entry.ignored)
                {
                    m_mapEntries[APID].emplace_back(entry);
                }
                foundEntry = true;
            }
        }
        if (!foundEntry)
        {
            m_missingAPIDs.emplace_back(APID);
            std::cerr << "Couldn't find matching APID in database: " << APID << "\n";
        }
    }
}

/**
 * Writes data to output files for each APID.
 *
 * @return N/A
 */
void Decom::writeData()
{
    if(checkForMissingOutput())
    {
        std::cerr << endl << "No APIDs matching selected APIDs" << endl;
        system("pause");
        return;
    }
    uint64_t i = 0;
    for (const auto& apid : m_map)
    {
        if(apid.second.at(0).ignored)
            continue;
        std::ofstream outfile;
        if(m_firstRun)
        {
            outfile.open("output/" + m_instrument + "_" + std::to_string(apid.first) + ".txt", std::ios::ate);
            outfile << std::setw(15) << "Day" << "," << std::setw(15) <<  "Millis" << "," << std::setw(15) << "Micros" << "," << std::setw(15) << "SeqCount" << ",";
        }
        else
            outfile = std::move(m_outfiles.at(apid.first));

        for (const DataTypes::Numeric& num : apid.second.at(0).data)
        {
            outfile << std::setw(15) << num.mnem << ",";
        }

        outfile << "\n";
        for (const DataTypes::Packet& pack: apid.second)
        {
            outfile << std::setw(15) << pack.day << "," << std::setw(15) << pack.millis << "," << std::setw(15) << pack.micros << "," << std::setw(15) << pack.sequenceCount << ",";
            for (const DataTypes::Numeric& num : pack.data)
            {
                switch (num.tag)
                {
                case DataTypes::Numeric::I32: outfile << std::setw(15) << std::right << num.i32; break;
                case DataTypes::Numeric::U32: outfile << std::setw(15) << std::right << num.u32; break;
                case DataTypes::Numeric::F64: outfile << std::setw(15) << std::right << num.f64; break;
                }
                outfile << ",";
            }
            outfile << "\n";
        }
        m_outfiles[apid.first] = std::move(outfile);
        m_firstRun = true;
    }
}

/**
 * Helper function to get output length. For progress bar purposes.
 *
 * @return N/A
 */
uint64_t Decom::getLength()
{
    uint64_t len = 1;
    for (const auto& apid : m_map)
    {
        for (const auto& packet : apid.second)
        {
            for (const auto& num : packet.data)
            {
                len++;
            }
        }
    }
    return len;
}

/**
 * Check to see if we are missing database entries for the current APID. IF we are missing them, then caller function will throw error.
 *
 * @return True if missing entries for APID. Otherwise false.
 */
bool Decom::checkForMissingOutput()
{
    if(m_map.empty())
        return true;
    else
        return false;
}

/**
 * Handles any special formatting requirements for instrument science data. Checks to see if we had instrument APID and then calls corresponding formatter function.
 *
 * @return N/A
 */
void Decom::formatInstruments()
{
    if (m_map.count(528) > 0)
    {
        m_map.clear();
        InstrumentFormat::formatATMS();
    }
    else if (m_map.count(536) > 0)
    {
        m_map.clear();
        InstrumentFormat::formatATMS();
    }
}
