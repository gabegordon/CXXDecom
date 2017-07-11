#pragma once

#include <thread>
#include <string>
#include <vector>
#include <unordered_map>
#include "ThreadSafeListenerQueue.h"
#include "DataTypes.h"

class ThreadPoolServer
{
  public:
  ThreadPoolServer(const std::string& instrument) :
    m_queues(),
    m_instrument(instrument)
    {}

    ~ThreadPoolServer() {}

    void exec(std::unique_ptr<DataTypes::Packet> pack);
    void join();
    void ThreadMain(ThreadSafeListenerQueue& queue, const std::string& instrument, const uint32_t apid);

  private:
    std::unordered_map<uint32_t, ThreadSafeListenerQueue> m_queues;
    std::vector<std::thread> m_threads;
    std::string m_instrument;
};
