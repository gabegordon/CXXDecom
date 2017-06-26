#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include "InstrumentFormat.h"
#include "ProgressBar.h"
#include "CSVRow.h"



struct atms_pack
{
    std::string day;
    std::string millis;
    std::string micros;
    float scanangle;
    uint32_t errflags;
    std::vector<uint32_t> chans;
};

struct out_pack
{
    std::string day;
    std::string millis;
    std::string micros;
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
    std::ofstream outfile;
    bool firstRun = true;
    auto bufSize = buf.size();
    out_pack blank = { "","","",{0}};
    std::vector<out_pack> outpacks(22);
    std::vector<float> scans(104);
    for (uint8_t init = 0; init < 22; init++)
    {
        outpacks[init] = blank;
        outpacks[init].chans.resize(104);
    }
    while(i < bufSize)
    {
        /*       if(firstRun)
                 {
                 firstRun = false;
                 for(uint16_t first = i; first < buf.size(); first++)
                 {
                 if(buf.at(first).errflags == 0)
                 continue;
                 else
                 {
                 i = first;
                 break;
                 }
                 }
                 }*/

        uint8_t packCounter = 0;
        for (auto& pack : outpacks)
        {
            pack.day = buf.at(i).day;
            pack.millis = buf.at(i).millis;
            pack.micros = buf.at(i).micros;
            pack.chans[0] = (buf.at(i).chans.at(packCounter++));
        }
        scans[0] = buf.at(i).scanangle;
        i++;
        uint16_t scanCounter = 1;
        for(uint32_t k = i; k < bufSize; k++)
        {
            if(buf.at(k).errflags == 0)
            {
                for (uint16_t l = 0; l < 22; l++)
                {
                    outpacks.at(l).chans[scanCounter] = (buf.at(k).chans.at(l));
                    scans[scanCounter] = (buf.at(k).scanangle);
                }
                scanCounter++;
            }
            else
            {
                i = k;
                break;
            }
        }

        if (scanCounter < 104)
            continue;

        for (uint16_t channelNumber = 1; channelNumber < 23; channelNumber++)
        {
            std::string filename = "output/ATMS_CHAN" + std::to_string(channelNumber) + ".txt";
            outfile.open(filename, std::ios::app);
            auto out = outpacks.at(channelNumber - 1);
            outfile << out.day << "," << out.millis << "," << out.micros << ",";
            for (const float& scan : scans)
                outfile << scan << ",";
            for (const uint32_t& chan : out.chans)
                outfile << chan << ",";
            outfile << "\n";
            outfile.close();
        }
    }
}

void InstrumentFormat::formatATMS()
{
    CSVRow atms_row;
    std::ifstream m_infile;
    m_infile.open("C:/Users/ggordon5/MSVC/Decom/output/SC_528.txt", std::ios::in | std::ios::ate);
    if (!m_infile || !m_infile.is_open())
    {
        std::cerr << "Failed to find ATMS output" << std::endl;
        return;
    }
    uint64_t fileSize = m_infile.tellg();
    m_infile.seekg(0, std::ios::beg);
    std::unique_ptr<ProgressBar> progbar(new ProgressBar(fileSize, "Reading ATMS"));
    progbar->SetFrequencyUpdate(10000);

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
        pack.scanangle = static_cast<float>(0.005493) * static_cast<float>(std::stoul(atms_row[4]));
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
