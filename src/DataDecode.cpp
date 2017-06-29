#include <fstream>
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
    if (length > 24)
    {
        if (currEntry.byte - offset + 1 >= buf.size() || currEntry.byte - offset + 2 >= buf.size() || currEntry.byte - offset + 3 >= buf.size())
            return false;
        m_byte1 = buf.at(currEntry.byte - offset + 1);
        m_byte2 = buf.at(currEntry.byte - offset + 2);
        m_byte3 = buf.at(currEntry.byte - offset + 3);
        bytes = FOUR;
    }
    else if (length > 16)
    {
        if (currEntry.byte - offset + 1 >= buf.size() || currEntry.byte - offset + 2 >= buf.size())
            return false;
        m_byte1 = buf.at(currEntry.byte - offset + 1);
        m_byte2 = buf.at(currEntry.byte - offset + 2);
        bytes = THREE;
    }
    else if (length > 8)
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
    uint8_t b1,b2,b3,b4,b5,b6,b7;
    if(currEntry.length == 64)
    {
        b1 = buf.at(currEntry.byte - offset + 1);
        b2 = buf.at(currEntry.byte - offset + 2);
        b3 = buf.at(currEntry.byte - offset + 3);
        b4 = buf.at(currEntry.byte - offset + 4);
        b5 = buf.at(currEntry.byte - offset + 5);
        b6 = buf.at(currEntry.byte - offset + 6);
        b7 = buf.at(currEntry.byte - offset + 7);
        uint64_t result = mergeBytes64(initialByte,b1,b2,b3,b4,b5,b6,b7);
        return static_cast<float>(result);
    }
    else
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
 * Get correct offset based on input data type
 *
 * @return Unsigned 32-bit integer offset
 */
uint32_t DataDecode::getOffset()
{
    if(m_pHeader.secondaryHeader)
        if(m_Instrument == "OMPS")
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
    std::vector<uint8_t> buf(m_pHeader.packetLength); //reserve space for bytes
    infile.read(reinterpret_cast<char*>(buf.data()), buf.size()); //read bytes
    DataTypes::Packet pack;

    if(m_entries.size() < 1)
    {
        pack.ignored = true;
        return pack;
    }
    else
        pack.ignored = false;

    uint32_t offset = getOffset();
    uint32_t entryIndex;
    pack.data.reserve(m_entries.size() * sizeof(DataTypes::Numeric) * 2);
    for(entryIndex = index; entryIndex < m_entries.size(); entryIndex++)
    {
        DataTypes::Numeric num;
        Bytes numBytes;

        if (!loadData(buf, numBytes, m_entries.at(entryIndex), offset) || m_entries.at(entryIndex).byte - offset >= buf.size()) //Make sure we don't go past array bounds (entries not contained in packet)
        {
            continue;
        }

        uint8_t initialByte = buf.at(m_entries.at(entryIndex).byte - offset);

        DataTypes::DataType dtype = m_entries.at(entryIndex).type;

        if (dtype == DataTypes::FLOAT)
        {
            num.f64 = getFloat(buf,m_entries.at(entryIndex),offset,initialByte);
        }
        else
        {
            switch (numBytes)
            {
            case ONE:
            {
                if (m_entries.at(entryIndex).bitUpper == 0 && m_entries.at(entryIndex).bitLower == 0)
                {
                    if (dtype == DataTypes::SIGNED)
                        num.i32 = initialByte;
                    else
                        num.u32 = initialByte;
                }
                else
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
    segmentLastByte = entryIndex;
    getHeaderData(pack);
    return pack;
}

/**
 * Handles decoding segmented functions. Acts as a wrapper function for standard decodeData function. Builds one large packet until encountering LAST flag.
 *
 * @param infile File to read from
 * @return Single packet containing all segmented packets
 */
DataTypes::Packet DataDecode::decodeDataSegmented(std::ifstream& infile)
{
    DataTypes::Packet segPack;
    getHeaderData(segPack);
    do
    {
        auto pack = decodeData(infile,segmentLastByte);
        segPack.data.insert(std::end(segPack.data), std::begin(pack.data), std::end(pack.data));
        std::tuple<DataTypes::PrimaryHeader, DataTypes::SecondaryHeader, bool> headers = HeaderDecode::decodeHeaders(infile, m_debug);
        m_pHeader = std::get<0>(headers);
    } while (m_pHeader.sequenceFlag != DataTypes::LAST);
    return segPack;
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
    uint32_t versionNum;
    uint16_t contCount;
    uint16_t contFlag;
    ReadFile::read(versionNum, infile);
    ReadFile::read(contCount, infile);
    ReadFile::read(contFlag, infile);
    if(m_pHeader.sequenceFlag == DataTypes::STANDALONE)
    {
        segPack = decodeData(infile,0);
    }
    else
    {
        if(!contCount)
        {
            segPack = decodeDataSegmented(infile);
        }
        else
        {
            uint16_t segPacketCount = 0;
            while(segPacketCount != contCount)
            {
                DataTypes::Packet tmpPack = decodeDataSegmented(infile);
                segPack.data.insert(std::end(segPack.data), std::begin(tmpPack.data), std::end(tmpPack.data));
                segPacketCount++;
            }
        }
    }
    return segPack;
}
