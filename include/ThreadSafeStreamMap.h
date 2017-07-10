#pragma once
#include <unordered_map>
#include <iostream>
#include <string>

class ThreadSafeStreamMap
{
  public:
  ThreadSafeStreamMap() :
    m_map(),
    m_mutices(),
    m_ofstreamLock(),
    m_mutexLock()
    {}
    ~ThreadSafeStreamMap() {}

    /**
     * Function to give threads access to output streams.
     *
     * @param instrument Instrument name for output file naming purposes
     * @param apid APID used in map lookup
     * @return ofstream reference for corresponding output file
     */
    std::ofstream& getStream(const std::string& instrument, const uint32_t& apid)
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
    std::mutex* getLock(const uint32_t& apid)
    {
        std::lock_guard<std::mutex> lock(m_mutexLock);
        return &m_mutices[apid];
    }

    typename std::unordered_map<uint32_t, std::ofstream>::iterator begin()
    {
        return m_map.begin();
    }

    typename std::unordered_map<uint32_t, std::ofstream>::iterator end()
    {
        return m_map.end();
    }

  private:
    std::unordered_map<uint32_t, std::ofstream> m_map;
    std::unordered_map<uint32_t, std::mutex> m_mutices;
    mutable std::mutex m_ofstreamLock;
    mutable std::mutex m_mutexLock;
};
