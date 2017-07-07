#pragma once

#include <thread>
#include <string>
#include <vector>
#include "ThreadSafeListenerQueue.h"
#include "DataTypes.h"
#include "ThreadSafeStreamMap.h"

class ThreadPoolServer
{
  public:
  ThreadPoolServer(const std::string& instrument) :
    m_queue(),
    m_instrument(instrument),
    m_num_threads(4)
    {}
    ~ThreadPoolServer() {}
    void start();
    void exec(DataTypes::Packet& pack);
    void join();
    void ThreadMain(ThreadSafeListenerQueue& queue, const std::string instrument, ThreadSafeStreamMap& outfiles);
  private:
    ThreadSafeListenerQueue m_queue;
    std::vector<std::thread> m_threads;
    std::string m_instrument;
    ThreadSafeStreamMap m_outfiles;
    uint32_t m_num_threads;
};
