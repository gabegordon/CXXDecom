#pragma once

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>
#include "DataTypes.h"

class ThreadSafeListenerQueue
{
  public:
  ThreadSafeListenerQueue() :
    m_active(true),
    q(),
    queueLock(),
    c()
    {}

    ~ThreadSafeListenerQueue() {}

    void push(std::unique_ptr<DataTypes::Packet> element);

    std::unique_ptr<DataTypes::Packet> listen(uint32_t& retVal);

    void setInactive();

  private:
    bool m_active;
    std::queue<std::unique_ptr<DataTypes::Packet>> q;
    mutable std::mutex queueLock;
    std::condition_variable c;
};
