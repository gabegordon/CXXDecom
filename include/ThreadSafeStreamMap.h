#pragma once
#include <unordered_map>
#include <mutex>
#include <iostream>

class ThreadSafeStreamMap
{
public:
	ThreadSafeStreamMap() :
		m(),
		m_map()
	{}
	~ThreadSafeStreamMap() {}

	std::ofstream& lockStream(const std::string& instrument, const uint32_t& apid)
	{
		auto& mut = getMutex(apid);
		mut.lock();
		auto& stream = getStream(apid);
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

	void unlockStream(const uint32_t& apid)
	{
		auto& mut = getMutex(apid);
		mut.unlock();
	}

	std::mutex& getMutex(const uint32_t& apid)
	{
		return m_locks[apid]; 
	}

	std::ofstream& getStream(const uint32_t& apid)
	{
		return m_map[apid];
	}

private:
	mutable std::mutex m;
	std::unordered_map<uint32_t, std::ofstream> m_map;
	std::unordered_map<uint32_t, std::mutex> m_locks;
};