#pragma once
#include <unordered_map>
#include <iostream>

class ThreadSafeStreamMap
{
public:
	ThreadSafeStreamMap() :
		m_map()
	{}
	~ThreadSafeStreamMap() {}

	std::ofstream& getStream(const std::string& instrument, const uint32_t& apid)
	{
      auto& stream = m_map[apid];
      if (stream.is_open())
      {
          return stream;
      }
      else
      {
          stream.open("output/" + instrument + "_" + std::to_string(apid) + ".txt", std::ios::ate);
          return stream;
      }
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
};
