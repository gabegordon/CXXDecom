#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include "ProgressBar.h"
#include "InstrumentFormat.h"
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
 * Handles accumulating data from between scans and writing it to file.
 *
 * @param buf Buffer of atms_pack read in from output file
 * @return N/A
 */
void writeChans(const std::vector<atms_pack>& buf)
{
    uint64_t i = 0;
    uint64_t bufSize = buf.size();

    ProgressBar writeProgress(bufSize, "Write ATMS");

    std::ofstream outfile;
    out_pack blank = { "","","",{0}};
    std::vector<out_pack> outpacks(22);
    std::vector<float> scans(104);
    for (uint8_t init = 0; init < 22; init++)
    {
        outpacks.at(init) = blank;
        outpacks.at(init).chans.resize(104);
    }

    while(i < bufSize)
    {
        writeProgress.Progressed(i);
        uint8_t packCounter = 0;

        for (auto& pack : outpacks)
        {
            pack.day = buf.at(i).day;
            pack.millis = buf.at(i).millis;
            pack.micros = buf.at(i).micros;
            pack.chans.at(0) = (buf.at(i).chans.at(packCounter++));
        }
        scans.at(0) = buf.at(i).scanangle;
        i++;
        uint16_t scanCounter = 1;
        for(uint64_t k = i; k < bufSize; k++)
        {
            if (scanCounter > 103)
                break;
            if(buf.at(k).errflags == 0)
            {
                for (uint16_t l = 0; l < 22; l++)
                {
                    outpacks.at(l).chans.at(scanCounter) = (buf.at(k).chans.at(l));
                    scans.at(scanCounter) = (buf.at(k).scanangle);
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
            for (const float scan : scans)
                outfile << scan << ",";
            for (const uint32_t chan : out.chans)
                outfile << chan << ",";
            outfile << "\n";
            outfile.close();
        }
    }
    writeProgress.Progressed(bufSize);
}

/**
 * Read ATMS science data in so that it can be properly formatted.
 *
 * @return N/A
 */
void InstrumentFormat::formatATMS()
{
    CSVRow atms_row;
    std::ifstream m_infile;
    m_infile.open("output/ATMS_528.txt", std::ios::in | std::ios::ate);
    if (!m_infile || !m_infile.is_open())
    {
        std::cerr << "Failed to find ATMS output" << std::endl;
        system("pause");
        return;
    }

    uint64_t fileSize = m_infile.tellg();
    m_infile.seekg(0, std::ios::beg);
    ProgressBar readProgress(fileSize, "Read ATMS");

    bool firstRow = true;
    std::vector<atms_pack> buf;

    while(m_infile >> atms_row)
    {
        readProgress.Progressed(m_infile.tellg());
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
        for(uint8_t i = 6; i < 28; ++i)
        {
            pack.chans.emplace_back(std::stoul(atms_row[i]));
        }
        buf.emplace_back(pack);
    }
    std::cout << std::endl;
    writeChans(buf);
}

void InstrumentFormat::formatOMPS()
{
    CSVRow omps_row;
    std::ifstream m_infile;
}
}
