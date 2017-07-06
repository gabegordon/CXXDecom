#include <iostream>
#include <bitset>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <stdlib.h>
#include "Decom.h"
#include "ByteManipulation.h"
#include "HeaderDecode.h"
#include "DataDecode.h"
#include "ProgressBar.h"
#include "InstrumentFormat.h"
#include "ThreadPoolServer.h"

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
    ProgressBar readProgress(fileSize, "Parsing");

    ThreadPoolServer pool(m_instrument);
    pool.start();
    while (true)
    {
        m_progress = m_infile.tellg();
        readProgress.Progressed(m_progress);
        if (m_infile.eof() || m_progress >= fileSize)
            break;

        std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> headers = HeaderDecode::decodeHeaders(m_infile, m_debug);

        if(!std::get<2>(headers))
            break;

        getEntries(std::get<0>(headers).APID);
        DataTypes::Packet pack;
        DataDecode dc(std::get<0>(headers), std::get<1>(headers), m_mapEntries[std::get<0>(headers).APID], m_debug, m_instrument);

        if(m_instrument == "OMPS")
            pack = dc.decodeOMPS(m_infile);
        else if (std::get<0>(headers).sequenceFlag == DataTypes::FIRST)
            pack = dc.decodeDataSegmented(m_infile, false);
        else
            pack = dc.decodeData(m_infile,0);

        pack.apid = std::get<0>(headers).APID;
        pool.exec(pack);
    }
    m_infile.close();
    pool.join();
    cout << endl;
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
 * Handles any special formatting requirements for instrument science data. Checks to see if we had instrument APID and then calls corresponding formatter function.
 *
 * @return N/A
 */
void Decom::formatInstruments()
{
    InstrumentFormat::formatATMS();
}
