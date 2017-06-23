#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include "InstrumentFormat.h"
#include "ProgressBar.h"
#include "CSVRow.h"



struct atms_pack
{
    uint32_t day;
    uint32_t millis;
    uint32_t micros;
    uint32_t scanangle;
    uint32_t errflags;
    std::vector<uint32_t> chans;
};

namespace InstrumentFormat
{

std::istream& operator >> (std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}

void writeChans(std::vector<atms_pack>& buf)
{
    uint32_t i = 0;
    bool firstRun = true;
    while(i < buf.size())
    {
        if(firstRun)
        {
            for(uint16_t k = i; k < buf.size(); k++)
            {
                if(buf.at(k).errflags == 0)
                    continue;
                else
                {
                    i = k;
                    break;
                }

            }
            firstRun = false;
        }
        std::vector<uint32_t> rawcounts = buf.at(i).chans;
        std::vector<uint32_t> scanangles = {buf.at(i).scanangle};
        uint32_t day = buf.at(i).day;
        uint32_t millis = buf.at(i).millis;
        uint32_t micros = buf.at(i).micros;

        for(uint16_t k = i; k < buf.size(); k++)
        {
            if(buf.at(k).errflags == 0)
            {
                scanangles.push_back(buf.at(k).scanangle);
                rawcounts.insert(std::end(rawcounts), std::begin(buf.at(k).chans), std::end(buf.at(k).chans));
            }
            else
            {
                i = k;
                break;
            }
        }
        for(uint16_t channelNumber = 1; channelNumber < 23; channelNumber++)
        {
            std::ofstream outfile;
            std::string filename = "output/ATMS_CHAN" + std::to_string(channelNumber) + ".txt";
            outfile.open(filename, std::ios_base::app);
            outfile << day << "," << millis << "," << micros << ",";
            for(const uint32_t& scan: scanangles)
                outfile << scan << ",";
            for(size_t channels = 0; channels < rawcounts.size(); channels++)
            {
                if(channels == channelNumber - 1)
                    outfile << rawcounts.at(channels) << ",";
            }
            outfile << "\n";
        }
    }
}

void InstrumentFormat::formatATMS()
{
    CSVRow atms_row;
    std::ifstream m_infile;
    m_infile.open("output/ATMS_528.txt", std::ios::in | std::ios::ate);
    if (!m_infile || !m_infile.is_open())
    {
        std::cerr << "Failed to find ATMS output" << std::endl;
        return;
    }
    uint64_t fileSize = m_infile.tellg();
    m_infile.seekg(0, std::ios::beg);
    std::unique_ptr<ProgressBar> progbar(new ProgressBar(fileSize, "Reading ATMS"));
    progbar->SetFrequencyUpdate(1000);

    bool firstRow = true;
    std::vector<atms_pack> buf;
    while(m_infile >> atms_row)
    {
        progbar->Progressed(m_infile.tellg());
        if(firstRow)
        {
            firstRow = false;
            continue;
        }
        atms_pack pack;
        pack.day = std::stoul(atms_row[0]);
        pack.millis = std::stoul(atms_row[1]);
        pack.micros = std::stoul(atms_row[2]);
        pack.scanangle = std::stoul(atms_row[4]);
        pack.errflags = std::stoul(atms_row[5]);

        for(int i = 6; i < 28; ++i)
        {
            pack.chans.push_back(std::stoul(atms_row[i]));
        }
        buf.push_back(pack);
    }
    writeChans(buf);
}


}
