#pragma once

#include <queue>
#include <tuple>
#include <mutex>
#include <memory>
#include <condition_variable>
#include "ThreadSafeStreamMap.h"
#include "DataTypes.h"

class ThreadSafeListenerQueue
{
  public:
  ThreadSafeListenerQueue() :
    q(),
    queueLock(),
    c(),
    m_map()
    {}

    ~ThreadSafeListenerQueue() {}

    void push(std::unique_ptr<DataTypes::Packet> element);

    std::tuple<std::unique_ptr<DataTypes::Packet>, std::mutex*> listen(uint32_t& retVal);

  private:
    std::queue<std::unique_ptr<DataTypes::Packet>> q;
    mutable std::mutex queueLock;
    mutable std::mutex orderLock;
    std::condition_variable c;
    ThreadSafeStreamMap m_map;
};
