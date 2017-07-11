#pragma once
#include <unordered_map>
#include <fstream>
#include <string>
#include <mutex>

class ThreadSafeStreamMap
{
  public:
  ThreadSafeStreamMap() :
    m_map(),
    m_mutices(),
    m_tempLocks(),
    m_ofstreamLock(),
    m_mutexLock()
    {}
    ~ThreadSafeStreamMap() {}


    std::ofstream& getStream(const std::string& instrument, const uint32_t& apid);

    std::mutex* getLock(const uint32_t& apid);

    std::unordered_map<uint32_t, std::ofstream>::iterator begin()
    {
        return m_map.begin();
    }

    std::unordered_map<uint32_t, std::ofstream>::iterator end()
    {
        return m_map.end();
    }

  private:
    std::unordered_map<uint32_t, std::ofstream> m_map;
    std::unordered_map<uint32_t, std::mutex> m_mutices;
    std::unordered_map<uint32_t, std::mutex> m_tempLocks;
    mutable std::mutex m_ofstreamLock;
    mutable std::mutex m_mutexLock;
};
