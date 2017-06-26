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
        DataDecode dc(std::get<0>(headers), std::get<1>(headers), m_mapEntries[std::get<0>(headers).APID], m_debug);

        if(m_instrument == "OMPS")
            pack = dc.decodeOMPS(m_infile);
        else if (std::get<0>(headers).sequenceFlag == DataTypes::FIRST)
            pack = dc.decodeDataSegmented(m_infile);
        else
            pack = dc.decodeData(m_infile,0);

        m_map[std::get<0>(headers).APID].push_back(pack);
    }
    m_infile.close();
    writeData();
    formatInstruments();
}
void Decom::getEntries(const uint32_t& APID)
{
    if (m_mapEntries[APID].empty())
    {
        bool foundEntry = false;
        for (const auto& entry : m_entries)
        {
            if (entry.i_APID == APID)
            {
                if(!entry.ignored)
                {
                    m_mapEntries[APID].push_back(entry);
                }
                foundEntry = true;
            }
        }
        if (!foundEntry)
        {
            std::cerr << "Couldn't find matching APID in database: " << APID << "\n";
        }
    }
}

void Decom::writeData()
{
    std::cout << std::endl;
    uint64_t len = getLength();
    ProgressBar writeProgress(len, "Write CSVs");
    if(!checkForMissingOutput())
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

        std::ofstream outfile("output/" + m_instrument + "_" + std::to_string(apid.first) + ".txt");

        outfile << std::setw(15) << "Day" << "," << std::setw(15) <<  "Millis" << "," << std::setw(15) << "Micros" << "," << std::setw(15) << "SeqCount" << ",";

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
                writeProgress.Progressed(i++);
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
        outfile.close();
    }
    std::cout << std::endl;
}

uint64_t Decom::getLength()
{
    uint64_t len = 0;
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

bool Decom::checkForMissingOutput()
{
    uint32_t packets = 0;
    uint32_t ignored_packets = 0;

    for(const auto& apid: m_map)
    {
        for(const auto& pack: apid.second)
        {
            packets++;
            if(pack.ignored)
                ignored_packets++;
        }
    }
    if(ignored_packets == packets)
        return false;
    else
        return true;
}

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
