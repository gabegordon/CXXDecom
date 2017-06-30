#pragma once

#include <thread>
#include <vector>
#include <string>
#include "ThreadSafeListenerQueue.h"
#include "DataTypes.h"

class ThreadPoolServer
{

public:
	ThreadPoolServer(const uint32_t n, const std::string& instrument) :
		m_queue(),
		m_threads(),
		m_num_threads(n),
		m_instrument(instrument)
	{}
	~ThreadPoolServer() {}
	void start();
	void exec(const DataTypes::Packet pack);
	void ThreadPoolServer::ThreadMain(ThreadSafeListenerQueue<DataTypes::Packet>& queue, const std::string instrument);
private:
	ThreadSafeListenerQueue<DataTypes::Packet> m_queue;
	std::vector<std::thread> m_threads;
	uint32_t m_num_threads;
	std::string m_instrument;
};

