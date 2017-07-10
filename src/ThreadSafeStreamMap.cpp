#include "ThreadSafeStreamMap.h"

/**
 * Function to give threads access to output streams.
 *
 * @param instrument Instrument name for output file naming purposes
 * @param apid APID used in map lookup
 * @return ofstream reference for corresponding output file
 */
std::ofstream& ThreadSafeStreamMap::getStream(const std::string& instrument, const uint32_t& apid)
{
    std::lock_guard<std::mutex> lock(m_ofstreamLock);
    auto& stream = m_map[apid];
    if (!stream.is_open())
    {
        stream.open("output/" + instrument + "_" + std::to_string(apid) + ".txt", std::ios::ate);
    }
    return stream;
}

/**
 * Function to give thread mutex corresponding to a file stream.
 *
 * @param apid APID used in map lookup
 * @return pointer to corresponding mutex
 */
std::mutex* ThreadSafeStreamMap::getLock(const uint32_t& apid)
{
    std::lock_guard<std::mutex> lock(m_mutexLock);
    return &m_mutices[apid];
}
