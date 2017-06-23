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
    std::string day;
    std::string millis;
    std::string micros;
    std::string scanangle;
    uint32_t errflags;
    std::vector<std::string> chans;
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
    std::unique_ptr<ProgressBar> progbar(new ProgressBar(buf.size(), "Writing ATMS"));
    progbar->SetFrequencyUpdate(1000);
    while(i < buf.size())
    {
        progbar->Progressed(i);
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
        std::vector<std::string> rawcounts = buf.at(i).chans;
        std::vector<std::string> scanangles = {buf.at(i).scanangle};
        std::string day = buf.at(i).day;
        std::string millis = buf.at(i).millis;
        std::string micros = buf.at(i).micros;

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
            for(const std::string& scan: scanangles)
                outfile << scan << ",";
            std::cout << "HERE" << rawcounts.size();
            for(size_t channels = 0; channels < rawcounts.size(); channels++)
            {
                for(int scanlength = 0; scanlength < 104*22; scanlength+=22)
                    outfile << rawcounts.at(scanlength+channels) << ",";
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
        pack.day = atms_row[0];
        pack.millis = atms_row[1];
        pack.micros = atms_row[2];
        pack.scanangle = atms_row[4];
        pack.errflags = std::stoul(atms_row[5]);

        for(int i = 6; i < 28; ++i)
        {
            pack.chans.push_back(atms_row[i]);
        }
        buf.push_back(pack);
    }
    writeChans(buf);
}


}
