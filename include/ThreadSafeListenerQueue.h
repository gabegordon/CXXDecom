#pragma once

#include <queue>
#include <tuple>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <iostream>
#include <chrono>
#include "ThreadSafeStreamMap.h"
#include "DataTypes.h"

class ThreadSafeListenerQueue
{
  public:
  ThreadSafeListenerQueue() :
    q(),
    queueLock(),
    mapLock(),
    c(),
    m_map()
    {}

    ~ThreadSafeListenerQueue() {}

    uint32_t push(std::unique_ptr<DataTypes::Packet> element)
    {
        std::lock_guard<std::mutex> lock(queueLock);
        q.push(std::move(element));
        c.notify_one();
        return 0;
    }

    std::tuple<std::unique_ptr<DataTypes::Packet>, std::mutex*> listen(uint32_t& retVal)
    {
        std::unique_lock<std::mutex> lock(queueLock);
        while (q.empty())
        {
            if (c.wait_for(lock, std::chrono::seconds(2)) == std::cv_status::timeout)
            {
                retVal = 0;
                return std::make_tuple(nullptr, nullptr);
            }
        }
        auto frontPtr = std::move(q.front());
        q.pop();
        std::lock_guard<std::mutex> orderingLock(mapLock);
        lock.unlock();
        std::mutex* mut = m_map.getLock(frontPtr->apid);
        mut->lock();
        auto tmpTuple = std::make_tuple(std::move(frontPtr), mut);
        retVal = 1;
        return tmpTuple;
    }


  private:
    std::queue<std::unique_ptr<DataTypes::Packet>> q;
    mutable std::mutex queueLock;
    mutable std::mutex mapLock;
    std::condition_variable c;
    ThreadSafeStreamMap m_map;
};
