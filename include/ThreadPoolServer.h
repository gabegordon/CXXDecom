#pragma once

#include <thread>
#include <string>
#include "ThreadSafeListenerQueue.h"
#include "DataTypes.h"
#include "ThreadSafeStreamMap.h"

class ThreadPoolServer
{

public:
	ThreadPoolServer(const std::string& instrument) :
		m_queue(),
		m_instrument(instrument)
	{}
	~ThreadPoolServer() {}
	void start();
	void exec(const DataTypes::Packet pack);
  void join();
  void ThreadMain(ThreadSafeListenerQueue<DataTypes::Packet>& queue, const std::string instrument, ThreadSafeStreamMap& outfiles);
private:
	ThreadSafeListenerQueue<DataTypes::Packet> m_queue;
	std::thread m_thread;
	std::string m_instrument;
  ThreadSafeStreamMap m_outfiles;
};

