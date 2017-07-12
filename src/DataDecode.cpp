#include <fstream>
#include <tuple>
#include <vector>
#include "HeaderDecode.h"
#include "DataDecode.h"
#include "ByteManipulation.h"
#include "ReadFile.h"

using namespace ByteManipulation;

/**
 * Loads bytes from buffer vector based on entry length and use offset to account for database entries included header bytes.
 *
 * @param buf Vector containing binary data
 * @param currEntry Entry we are using to decode
 * @return True if successful. False if outside vector bounds
 */
bool DataDecode::loadData(const std::vector<uint8_t>& buf, const DataTypes::Entry& currEntry)
{
    m_initialByte = buf.at(currEntry.byte - m_offset);
    uint32_t length = currEntry.length;
    if (length > 24)  // Four bytes
    {
        if (currEntry.byte - m_offset + 1 >= buf.size() || currEntry.byte - m_offset + 2 >= buf.size() || currEntry.byte - m_offset + 3 >= buf.size())  // Ensure we don't exceed vector boundaries
            return false;
        m_byte1 = buf.at(currEntry.byte - m_offset + 1);
        m_byte2 = buf.at(currEntry.byte - m_offset + 2);
        m_byte3 = buf.at(currEntry.byte - m_offset + 3);
        m_numBytes = FOUR;
    }
    else if (length > 16)  // Three bytes
    {
        if (currEntry.byte - m_offset + 1 >= buf.size() || currEntry.byte - m_offset + 2 >= buf.size())
            return false;
        m_byte1 = buf.at(currEntry.byte - m_offset + 1);
        m_byte2 = buf.at(currEntry.byte - m_offset + 2);
        m_numBytes = THREE;
    }
    else if (length > 8)  // Two bytes
    {
        if (currEntry.byte - m_offset + 1 >= buf.size())
            return false;
        m_byte1 = buf.at(currEntry.byte - m_offset + 1);
        m_numBytes = TWO;
    }
    else
    {
        m_numBytes = ONE;
    }
    return true;
}

/**
 * Special function to handle loading all bytes for a floating point entry.
 *
 * @param buf Vector containing binary data
 * @param currEntry Entry we are using to decode
 * @return Final floating point value
 */
float DataDecode::getFloat(const std::vector<uint8_t>& buf, const DataTypes::Entry& currEntry)
{

    m_initialByte = buf.at(currEntry.byte - m_offset);
    uint8_t b1, b2, b3, b4, b5, b6, b7;
    if (currEntry.length == 64)
    {
        b1 = buf.at(currEntry.byte - m_offset + 1);
        b2 = buf.at(currEntry.byte - m_offset + 2);
        b3 = buf.at(currEntry.byte - m_offset + 3);
        b4 = buf.at(currEntry.byte - m_offset + 4);
        b5 = buf.at(currEntry.byte - m_offset + 5);
        b6 = buf.at(currEntry.byte - m_offset + 6);
        b7 = buf.at(currEntry.byte - m_offset + 7);
        uint64_t result = mergeBytes64(m_initialByte, b1, b2, b3, b4, b5, b6, b7);
        return static_cast<float>(result);
    }
    else  // Length is 32
    {
        b1 = buf.at(currEntry.byte - m_offset + 1);
        b2 = buf.at(currEntry.byte - m_offset + 2);
        b3 = buf.at(currEntry.byte - m_offset + 3);
        uint32_t result = mergeBytes(m_initialByte,b1,b2,b3,4);
        return static_cast<float>(result);
    }
}

/**
 * Set packet data from headers.
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
    pack.apid = m_pHeader.APID;
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
    if (!checkPackEntries(pack))
        return pack;

    std::vector<uint8_t> buf(m_pHeader.packetLength);  // reserve space for bytes
    infile.read(reinterpret_cast<char*>(buf.data()), buf.size());  // read bytes
    pack.data.reserve(m_entries.size() * sizeof(DataTypes::Numeric) * 2);

    uint32_t entryIndex;
    uint64_t size = m_entries.size();
    m_offset = getOffset();

    for (entryIndex = index; entryIndex < size; ++entryIndex)  // Loop through all database entries until we reach end or are about to go out of bounds
    {
        if (!loadData(buf, m_entries.at(entryIndex)) || m_entries.at(entryIndex).byte - m_offset >= buf.size())  // Make sure we don't go past array bounds (entries not contained in packet)
        {
            continue;
        }
        DataTypes::DataType dtype = m_entries.at(entryIndex).type;
        pack.data.emplace_back(getNum(dtype, buf, entryIndex));
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
                    m_pHeader.packetLength -= 4;
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

/**
 * Given an entry get corresponding numerical value.
 *
 * @param dtype Data type of current entry
 * @param buf Byte buffer to read from
 * @param entryIndex Index of current entry in vector
 * @return Numerical value for entry
 */
DataTypes::Numeric DataDecode::getNum(const DataTypes::DataType& dtype, const std::vector<uint8_t>& buf, const uint32_t& entryIndex)
{
    DataTypes::Numeric num;
    if (dtype == DataTypes::FLOAT)
    {
        num.f64 = getFloat(buf, m_entries.at(entryIndex));
        return num;
    }
    else
    {
        switch (m_numBytes)
        {
        case ONE:  // One byte
        {
            decodeOne(dtype, entryIndex, num);
            break;
        }
        case TWO:
        {
            decodeTwo(dtype, entryIndex, num);
            break;
        }
        case THREE:
        {
            decodeThree(dtype, entryIndex, num);
            break;
        }
        case FOUR:
        {
            decodeThree(dtype, entryIndex, num);
            break;
        }
        }

        if (dtype == DataTypes::SIGNED)
            num.tag = DataTypes::Numeric::I32;
        else
            num.tag = DataTypes::Numeric::U32;

        num.mnem = m_entries.at(entryIndex).mnemonic;
        return num;
    }
}

/**
 * Decode an entry for a 1 byte value.
 *
 * @param dtype Data type of current entry
 * @param entryIndex Index of current entry in vector
 * @param num Numerical value to update
 * @return N/A
 */
void DataDecode::decodeOne(const DataTypes::DataType& dtype, const uint32_t& entryIndex, DataTypes::Numeric& num)
{
    if (m_entries.at(entryIndex).bitUpper == 0 && m_entries.at(entryIndex).bitLower == 0)  // If no bit range, then use whole byte
    {
        if (dtype == DataTypes::SIGNED)
            num.i32 = m_initialByte;
        else
            num.u32 = m_initialByte;
    }
    else  // Otherwise extract specific bit(s)
    {
        if (dtype == DataTypes::SIGNED)
            num.i32 = ByteManipulation::extract8Signed(m_initialByte, m_entries.at(entryIndex).bitLower, (m_entries.at(entryIndex).bitUpper + 1));
        else
            num.u32 = ByteManipulation::extract8(m_initialByte, m_entries.at(entryIndex).bitLower, (m_entries.at(entryIndex).bitUpper - m_entries.at(entryIndex).bitLower) + 1);
    }
}

/**
 * Decode an entry for a 2 byte value.
 *
 * @param dtype Data type of current entry
 * @param entryIndex Index of current entry in vector
 * @param num Numerical value to update
 * @return N/A
 */
void DataDecode::decodeTwo(const DataTypes::DataType& dtype, const uint32_t& entryIndex, DataTypes::Numeric& num)
{
    uint16_t result = mergeBytes16(m_initialByte, m_byte1);
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
}

/**
 * Decode an entry for a 3 byte value.
 *
 * @param dtype Data type of current entry
 * @param entryIndex Index of current entry in vector
 * @param num Numerical value to update
 * @return N/A
 */
void DataDecode::decodeThree(const DataTypes::DataType& dtype, const uint32_t& entryIndex, DataTypes::Numeric& num)
{
    uint32_t result = mergeBytes(m_initialByte, m_byte1, m_byte2, m_byte3, 3);
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
}

/**
 * Decode an entry for a 4 byte value.
 *
 * @param dtype Data type of current entry
 * @param entryIndex Index of current entry in vector
 * @param num Numerical value to update
 * @return N/A
 */
void DataDecode::decodeFour(const DataTypes::DataType& dtype, const uint32_t& entryIndex, DataTypes::Numeric& num)
{
    uint32_t result = mergeBytes(m_initialByte, m_byte1, m_byte2, m_byte3, 4);
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
}

/**
 * Checks if a packet is missing entries (ignored) or is a header only packet.
 *
 * @param pack Packet to check
 * @return True if we should continue parsing packet
 */
bool DataDecode::checkPackEntries(DataTypes::Packet& pack)
{
    if (m_entries.size() < 1)  // If no entries for this APID, then it is ignored
    {
        if (m_pHeader.packetLength != 0)
        {
            pack.ignored = true;
            return false;
        }
        else  // Unless it is an APID that only contains header info (data length of 0)
        {
            getHeaderData(pack);
            return false;
        }
    }
    pack.ignored = false;
    return true;
}
