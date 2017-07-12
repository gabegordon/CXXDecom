#include <iostream>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <cstdlib>
#include <string>
#include <memory>
#include "Decom.h"
#include "ProgressBar.h"
#include "ByteManipulation.h"
#include "HeaderDecode.h"
#include "DataDecode.h"
#include "InstrumentFormat.h"
#include "ThreadPoolServer.h"
namespace Local
{
#include "ReadFile.h"
}

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
    m_infile.open(infile, std::ios::binary | std::ios::in);  //Open file as binary for reading
    Local::ReadFile::checkFile(m_infile, ".pkt");

    uint64_t fileSize = getFileSize();
    ProgressBar readProgress(fileSize, "Parsing");  // Create progress bar

    ThreadPoolServer pool{m_instrument};  // Create thread pool that we will be passing our packets to

    while (true)  // Loop until error or we reach end of file
    {
        m_progress = m_infile.tellg();  // Get current progress
        readProgress.Progressed(m_progress);
        if (m_infile.eof() || m_progress >= fileSize)  // If reached end of file
            break;

        if (!getHeadersAndEntries())
            break;

        DataTypes::Packet pack = decodeData();

        storeAPID(pack.apid);
        pool.exec(std::move(std::make_unique<DataTypes::Packet>(pack)));  // Push packet into our writer thread's queue
    }

    m_infile.close(); // Close input file
    pool.join();  // Wait for writer threads to join
    formatInstruments(); // Check if we need to format instrument data
}

/**
 * Finds database entries that match our current APID.
 *
 * @param APID APID to search for
 * @return N/A
 */
void Decom::getEntries(const uint32_t& APID)
{
    if (std::find(std::begin(m_missingAPIDs), std::end(m_missingAPIDs), APID) != std::end(m_missingAPIDs)){  // If we already determined apid was missing, then skip
        return;
    }

    if (m_mapEntries[APID].empty())  // If we haven't already loaded entries for this apid
    {
        bool foundEntry = false;
        for (const auto& entry : m_entries)  // Loop through all entries
        {
            if (entry.i_APID == APID)
            {
                if (!entry.ignored)
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
 * Handles any special formatting requirements for instrument science data.
 *
 * @return N/A
 */
void Decom::formatInstruments() const
{
    if(m_APIDs.count(528))
        InstrumentFormat::formatATMS();
}

/**
 * Stores set of all APIDs that we have processed.
 *
 * @param APID APID to store
 * @return N/A
 */
void Decom::storeAPID(const uint32_t& APID)
{
    m_APIDs.insert(APID);
}

/**
 * Finds filesize by seeking to end of file and getting file pointer position.
 *
 * @return fileSize
 */
uint64_t Decom::getFileSize()
{
    m_infile.seekg(0, std::ios::end);  // Seek to end to get filesize
    uint64_t fileSize = m_infile.tellg();
    m_infile.seekg(0, std::ios::beg);
    return fileSize;
}

/**
 * Create DataDecode object and call correct function for decoding data.
 *
 * @return Packet struct containing the data
 */
DataTypes::Packet Decom::decodeData()
{
    DataDecode dc{std::get<0>(m_headers), std::get<1>(m_headers), m_mapEntries[std::get<0>(m_headers).APID], m_debug, m_instrument};  // Create new dataDecode object and pass headers/instrument info

    if (m_instrument == "OMPS")  // If omps then use special function
    {
        return dc.decodeOMPS(m_infile);
    }
    else if (std::get<0>(m_headers).sequenceFlag == DataTypes::FIRST)  // If segmented packet
    {
        return dc.decodeDataSegmented(m_infile, false);
    }
    else  // Otherwise standalone packet
    {
        return dc.decodeData(m_infile, 0);
    }
}

/**
 * Calls decodeHeaders function and stores result in member variable.
 * Once complete finds matching entries.
 *
 * @return False on invalid header
 */
bool Decom::getHeadersAndEntries()
{
    m_headers = HeaderDecode::decodeHeaders(m_infile, m_debug);  // Decode headers

    if (!std::get<2>(m_headers))  // If header is invalid
        return false;

    getEntries(std::get<0>(m_headers).APID);  // Get matching database entries from header APID
    return true;
}
