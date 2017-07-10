#include <fstream>
#include <iostream>
#include <vector>
#include <string>
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
    uint32_t sos_sync;
    std::vector<uint32_t> chans;
};

struct out_pack
{
    std::string day;
    std::string millis;
    std::string micros;
    std::vector<uint32_t> chans = std::vector<uint32_t>(104);
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
    std::vector<out_pack> outpacks(22);  // Create vector of outpackets (1 for each ATMS channel)
    std::vector<float> scans(104);   // Create vector for scan angles (104 per complete scan)
    std::vector<std::ofstream> outfiles(22);  // Create of streams (1 for each ATMS channnel)

    while (i < bufSize)  // Loop until we reach end of buffer
    {
        writeProgress.Progressed(i);
        uint8_t packCounter = 0;

        for (auto& pack : outpacks)  // Add time info to outpacks
        {
            pack.day = buf.at(i).day;
            pack.millis = buf.at(i).millis;
            pack.micros = buf.at(i).micros;
            pack.chans.at(0) = (buf.at(i).chans.at(packCounter++));
        }
        scans.at(0) = buf.at(i).scanangle;
        i++;  // Get scan angle and chans of new scan flag location, and then increment i so that next loop does not stop immediately

        uint16_t scanCounter = 1;
        for (uint64_t k = i; k < bufSize; k++)
        {
            if (scanCounter > 103)
                break;
            if (buf.at(k).sos_sync == 0)
            {
                for (uint16_t l = 0; l < 22; l++)  // For each channel get angle and counts
                {
                    outpacks.at(l).chans.at(scanCounter) = (buf.at(k).chans.at(l));
                    scans.at(scanCounter) = (buf.at(k).scanangle);
                }
                scanCounter++;
            }
            else  // If we get new scan flag then stop
            {
                i = k;
                break;
            }
        }

        if (scanCounter < 103)  // If we didnt get a full scan
            continue;

        for (uint16_t channelNumber = 1; channelNumber < 23; channelNumber++)  // Write to files
        {
            std::ofstream& outfile = outfiles.at(channelNumber - 1);  // Get stream from vector

            if (!outfile.is_open())
            {
                std::string filename = "output/ATMS_CHAN" + std::to_string(channelNumber) + ".txt";
                outfile.open(filename, std::ios::app);
            }

            auto out = outpacks.at(channelNumber - 1);
            outfile << out.day << "," << out.millis << "," << out.micros << ",";
            for (const float& scan : scans)
                outfile << scan << ",";
            for (const uint32_t& chan : out.chans)
                outfile << chan << ",";
            outfile << "\n";
        }
    }
    writeProgress.Progressed(bufSize);
}

/**
 * Read ATMS science data in so that it can be properly formatted.
 *
 * @return N/A
 */
void formatATMS()
{
    CSVRow atms_row;
    std::ifstream m_infile;
    m_infile.open("output/ATMS_528.txt", std::ios::in | std::ios::ate);
    if (!m_infile || !m_infile.is_open())
    {
        return;
    }

    uint64_t fileSize = m_infile.tellg();
    m_infile.seekg(0, std::ios::beg);  // Seek to beginning because we opened at end
    ProgressBar readProgress(fileSize, "Read ATMS");

    bool firstRow = true;
    std::vector<atms_pack> buf;

    while (m_infile >> atms_row)  // Read rows into atms_pack structs
    {
        readProgress.Progressed(m_infile.tellg());
        if (firstRow)
        {
            firstRow = false;
            continue;
        }
        atms_pack pack;

        pack.day = atms_row[0];
        pack.millis = atms_row[1];
        pack.micros = atms_row[2];
        pack.scanangle = static_cast<float>(0.005493) * static_cast<float>(std::stoul(atms_row[4]));  // Scan angle conversion constant
        pack.sos_sync = std::stoul(atms_row[6]);
        for (uint8_t i = 14; i < 36; ++i)  // Channel counts are in columns 14 to 36
        {
            pack.chans.emplace_back(std::stoul(atms_row[i]));
        }
        buf.emplace_back(pack);
    }
    std::cout << std::endl;
    writeChans(buf);
}


void formatOMPS()
{
    CSVRow omps_row;
    std::ifstream m_infile;
}
}
