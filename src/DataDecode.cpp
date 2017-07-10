#include <fstream>
#include <tuple>
#include <vector>
#include "HeaderDecode.h"
#include "DataDecode.h"
#include "ByteManipulation.h"
#include "ReadFile.h"

using namespace ByteManipulation;

/**
 * Loads bytes from buffer vector based on entry length and adjusts offset to account for database entries included header bytes.
 *
 * @param buf Vector containing binary data
 * @param bytes Number of bytes that need to be loaded
 * @param currEntry Entry we are using to decode
 * @param offset Offset based on whether we have headers
 * @return True if successful. False if outside vector bounds
 */
bool DataDecode::loadData(const std::vector<uint8_t>& buf, Bytes& bytes, const DataTypes::Entry& currEntry, const uint32_t& offset)
{
    uint32_t length = currEntry.length;
    if (length > 24)  // Four bytes
    {
        if (currEntry.byte - offset + 1 >= buf.size() || currEntry.byte - offset + 2 >= buf.size() || currEntry.byte - offset + 3 >= buf.size())  // Ensure we don't exceed vector boundaries
            return false;
        m_byte1 = buf.at(currEntry.byte - offset + 1);
        m_byte2 = buf.at(currEntry.byte - offset + 2);
        m_byte3 = buf.at(currEntry.byte - offset + 3);
        bytes = FOUR;
    }
    else if (length > 16)  // Three bytes
    {
        if (currEntry.byte - offset + 1 >= buf.size() || currEntry.byte - offset + 2 >= buf.size())
            return false;
        m_byte1 = buf.at(currEntry.byte - offset + 1);
        m_byte2 = buf.at(currEntry.byte - offset + 2);
        bytes = THREE;
    }
    else if (length > 8)  // Two bytes
    {
        if (currEntry.byte - offset + 1 >= buf.size())
            return false;
        m_byte1 = buf.at(currEntry.byte - offset + 1);
        bytes = TWO;
    }
    else
    {
        bytes = ONE;
    }
    return true;
}

/**
 * Special function to handle loading all eight bytes for a floating point entry.
 *
 * @param buf Vector containing binary data
 * @param currEntry Entry we are using to decode
 * @param offset Offset based on whether we have headers
 * @param initialByte First byte that was loaded previously
 * @return Final floating point value
 */
float DataDecode::getFloat(const std::vector<uint8_t>& buf, const DataTypes::Entry& currEntry, const uint32_t& offset, uint8_t initialByte)
{
    uint8_t b1, b2, b3, b4, b5, b6, b7;
    if (currEntry.length == 64)
    {
        b1 = buf.at(currEntry.byte - offset + 1);
        b2 = buf.at(currEntry.byte - offset + 2);
        b3 = buf.at(currEntry.byte - offset + 3);
        b4 = buf.at(currEntry.byte - offset + 4);
        b5 = buf.at(currEntry.byte - offset + 5);
        b6 = buf.at(currEntry.byte - offset + 6);
        b7 = buf.at(currEntry.byte - offset + 7);
        uint64_t result = mergeBytes64(initialByte, b1, b2, b3, b4, b5, b6, b7);
        return static_cast<float>(result);
    }
    else  // Length is 32
    {
        b1 = buf.at(currEntry.byte - offset + 1);
        b2 = buf.at(currEntry.byte - offset + 2);
        b3 = buf.at(currEntry.byte - offset + 3);
        uint32_t result = mergeBytes(initialByte,b1,b2,b3,4);
        return static_cast<float>(result);
    }
}

/**
 * Set packet data from secondary header.
 *
 * @param pack Packet to be set
 * @return N/A
 */
void DataDecode::getHeaderData(DataTypes::Packet& pack)
{
    pack.day = m_sHeader.day;
    pack.millis = m_sHeader.millis;
    pack.micros = m_sHeader.micros;
    pack.sequenceCount = m_pHeader.packetSequence;
}

/**
 * Get correct offset based on input data type (Offset refers to starting byte:bit values listed in database)
 *
 * @return Unsigned 32-bit integer offset
 */
uint8_t DataDecode::getOffset()
{
    if (m_pHeader.secondaryHeader)
        if (m_Instrument == "OMPS")
            return 20;
        else
            return 14;
    else
        return 6;
}

/**
 * Main decode function. Handles decoding all entries for current APID.
 *
 * @param infile File to read binary data from
 * @param index Index to begin from (used only in segmented packets)
 * @return Packet containing all data from binary stream
 */
DataTypes::Packet DataDecode::decodeData(std::ifstream& infile, const uint32_t& index)
{
    DataTypes::Packet pack;

    if (m_entries.size() < 1)  // If no entries for this APID, then it is ignored
    {
        if (m_pHeader.packetLength != 0)
        {
            pack.ignored = true;
        }
        else  // Unless it is an APID that only contains header info (data length of 0)
        {
            getHeaderData(pack);
            return pack;
        }
    }
    else
        pack.ignored = false;

    std::vector<uint8_t> buf(m_pHeader.packetLength);  // reserve space for bytes
    infile.read(reinterpret_cast<char*>(buf.data()), buf.size());  // read bytes
    pack.data.reserve(m_entries.size() * sizeof(DataTypes::Numeric) * 2);

    uint8_t offset = getOffset();
    uint32_t entryIndex;
    uint64_t size = m_entries.size();

    for (entryIndex = index; entryIndex < size; entryIndex++)  // Loop through all database entries until we reach end or are about to go out of bounds
    {
        DataTypes::Numeric num;
        Bytes numBytes;

        if (!loadData(buf, numBytes, m_entries.at(entryIndex), offset) || m_entries.at(entryIndex).byte - offset >= buf.size())  // Make sure we don't go past array bounds (entries not contained in packet)
        {
            continue;
        }

        uint8_t initialByte = buf.at(m_entries.at(entryIndex).byte - offset);

        DataTypes::DataType dtype = m_entries.at(entryIndex).type;

        if (dtype == DataTypes::FLOAT)
        {
            num.f64 = getFloat(buf, m_entries.at(entryIndex), offset, initialByte);
        }
        else
        {
            switch (numBytes)
            {
            case ONE:  // One byte
            {
                if (m_entries.at(entryIndex).bitUpper == 0 && m_entries.at(entryIndex).bitLower == 0)  // If no bit range, then use whole byte
                {
                    if (dtype == DataTypes::SIGNED)
                        num.i32 = initialByte;
                    else
                        num.u32 = initialByte;
                }
                else  // Otherwise extract specific bit(s)
                {
                    if (dtype == DataTypes::SIGNED)
                        num.i32 = ByteManipulation::extract8Signed(initialByte, m_entries.at(entryIndex).bitLower, (m_entries.at(entryIndex).bitUpper + 1));
                    else
                        num.u32 = ByteManipulation::extract8(initialByte, m_entries.at(entryIndex).bitLower, (m_entries.at(entryIndex).bitUpper - m_entries.at(entryIndex).bitLower) + 1);
                }
                break;
            }
            case TWO:
            {
                uint16_t result = mergeBytes16(initialByte, m_byte1);
                if (m_entries.at(entryIndex).bitUpper == 0 && m_entries.at(entryIndex).bitLower == 0)
                {
                    if (dtype == DataTypes::SIGNED)
                        num.i32 = static_cast<int16_t>(result);
                    else
                        num.u32 = result;
                }
                else
                {
                    if (dtype == DataTypes::SIGNED)
                        num.i32 = static_cast<int16_t>(ByteManipulation::extract16(result, m_entries.at(entryIndex).bitLower, (m_entries.at(entryIndex).bitUpper - m_entries.at(entryIndex).bitLower) + 1));
                    else
                        num.u32 = ByteManipulation::extract16(result, m_entries.at(entryIndex).bitLower, (m_entries.at(entryIndex).bitUpper - m_entries.at(entryIndex).bitLower) + 1);
                }
                break;
            }
            case THREE:
            {
                uint32_t result = mergeBytes(initialByte, m_byte1, m_byte2, m_byte3, 3);
                if (m_entries.at(entryIndex).bitUpper == 0 && m_entries.at(entryIndex).bitLower == 0)
                {
                    if (dtype == DataTypes::SIGNED)
                        num.i32 = static_cast<int32_t>(result);
                    else
                        num.u32 = result;
                }
                else
                {
                    if (dtype == DataTypes::SIGNED)
                        num.i32 = static_cast<int32_t>(ByteManipulation::extract32(result, m_entries.at(entryIndex).bitLower, (m_entries.at(entryIndex).bitUpper - m_entries.at(entryIndex).bitLower) + 1));
                    else
                        num.u32 = ByteManipulation::extract32(result, m_entries.at(entryIndex).bitLower, (m_entries.at(entryIndex).bitUpper - m_entries.at(entryIndex).bitLower) + 1);
                }
                break;
            }
            case FOUR:
            {
                uint32_t result = mergeBytes(initialByte, m_byte1, m_byte2, m_byte3, 4);
                if (m_entries.at(entryIndex).bitUpper == 0 && m_entries.at(entryIndex).bitLower == 0)
                {
                    if (dtype == DataTypes::SIGNED)
                        num.i32 = static_cast<int32_t>(result);
                    else
                        num.u32 = result;
                }
                else
                {
                    if (dtype == DataTypes::SIGNED)
                        num.i32 = static_cast<int32_t>(ByteManipulation::extract32(result, m_entries.at(entryIndex).bitLower, (m_entries.at(entryIndex).bitUpper - m_entries.at(entryIndex).bitLower) + 1));
                    else
                        num.u32 = ByteManipulation::extract32(result, m_entries.at(entryIndex).bitLower, (m_entries.at(entryIndex).bitUpper - m_entries.at(entryIndex).bitLower) + 1);
                }
                break;
            }
            }

            if (dtype == DataTypes::SIGNED)
                num.tag = DataTypes::Numeric::I32;
            else
                num.tag = DataTypes::Numeric::U32;

            num.mnem = m_entries.at(entryIndex).mnemonic;
            pack.data.emplace_back(num);
        }
    }
    segmentLastByte = entryIndex;  // Save our last entry position in case this is a segmented packet
    getHeaderData(pack);  // Load time data
    return pack;
}

/**
 * Handles decoding segmented functions. Acts as a wrapper function for standard decodeData function. Builds one large packet until encountering LAST flag.
 *
 * @param infile File to read from
 * @return Single packet containing all segmented packets
 */
DataTypes::Packet DataDecode::decodeDataSegmented(std::ifstream& infile, const bool omps)
{
    DataTypes::Packet segPack;

    if (!omps)  // If omps, then segmentLastByte is handled elsewhere (because packets may be super-segmented)
        segmentLastByte = 0;

    getHeaderData(segPack);
    auto pack = decodeData(infile, segmentLastByte);  // Read the data from the first packet segment
    segPack.data.insert(std::end(segPack.data), std::begin(pack.data), std::end(pack.data));   // Append to back of our data vector

    do  // Continue getting headers and data, and appending it until we reach LAST packet segment
    {
        std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> headers = HeaderDecode::decodeHeaders(infile, m_debug);
        m_pHeader = std::get<0>(headers);
        auto pack = decodeData(infile, segmentLastByte);
        segPack.data.insert(std::end(segPack.data), std::begin(pack.data), std::end(pack.data));
    } while (m_pHeader.sequenceFlag != DataTypes::LAST);

    return segPack;  // Return one packet containing all the data from segmented packets
}

/**
 * Special OMPS decode function. Handles OMPS extra header. Uses standard dataDecode as underlying decode method.
 *
 * @param infile File to read from
 * @return Single packet containing all [segmented] packets
 */
DataTypes::Packet DataDecode::decodeOMPS(std::ifstream& infile)
{
    DataTypes::Packet segPack;
    uint16_t versionNum;
    uint8_t contCount;
    uint8_t contFlag;
    ReadFile::read(versionNum, infile);
    ReadFile::read(contCount, infile);
    ReadFile::read(contFlag, infile);
    versionNum = ByteManipulation::swapEndian16(versionNum);

    m_pHeader.packetLength -= 4;  // Subtract four from length to account for versionNum, contCount, and contFlag

    if (m_pHeader.sequenceFlag == DataTypes::STANDALONE)  // If standalone, then do standard decode
    {
        segPack = decodeData(infile, 0);
    }
    else
    {
        if (!contCount)  // If contCount is not set, then do standard segmented decode
        {
            segPack = decodeDataSegmented(infile, false);
        }
        else  // Otherwise, handle super-segmented packet
        {
            uint16_t segPacketCount = 0;
            segmentLastByte = 0;
            while (segPacketCount != contCount)
            {
                if (segPacketCount != 0)  //  Skip parsing headers for first packet, as we have already done so
                {
                    std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> headers = HeaderDecode::decodeHeaders(infile, m_debug);
                    m_pHeader = std::get<0>(headers);
                    m_pHeader.packetLength -= 8;
                    uint64_t ompsHeader;
                    ReadFile::read(ompsHeader, infile);
                }
                DataTypes::Packet tmpPack = decodeDataSegmented(infile, true);
                segPack.data.insert(std::end(segPack.data), std::begin(tmpPack.data), std::end(tmpPack.data));
                segPacketCount++;
            }
        }
    }
    return segPack;
}
