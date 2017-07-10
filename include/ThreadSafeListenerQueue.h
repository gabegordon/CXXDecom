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
    c(),
    m_map()
    {}

    ~ThreadSafeListenerQueue() {}

    /**
     * Pushes a Packet pointer into the queue. Notifies one waiting listener.
     *
     * @param element unique_ptr to be pushed
     * @return N/A
     */
    void push(std::unique_ptr<DataTypes::Packet> element)
    {
        std::lock_guard<std::mutex> lock(queueLock);  // Lock mutex to prevent simultaneous push and pops
        q.push(std::move(element));  // Move pointer into queue
        c.notify_one();  // Notify one waiting thread
    }

    /**
     * Function that blocks while queue is empty on condition_variable c. Once notified pops pointer from queue and acquires lock.
     *
     * @param retVal Integer passed by referenec to get return value. 1 on success. 0 on timeout.
     * @return tuple containing pointer to Packet and pointer to locked mutex
     */
    std::tuple<std::unique_ptr<DataTypes::Packet>, std::mutex*> listen(uint32_t& retVal)
    {
        std::unique_lock<std::mutex> lock(queueLock);  // Get mutex lock
        while (q.empty()) // While loop checking empty to account for spurious wakeups
        {
            if (c.wait_for(lock, std::chrono::seconds(2)) == std::cv_status::timeout)  // Wait on condition var until notified, or 2 second timeout is reached
            {
                retVal = 0;  // Reached timeout return 0 to let thread know to terminate
                return std::make_tuple(nullptr, nullptr);  // Return empty tuple
            }
        }

        auto frontPtr = std::move(q.front());  // Once woken up save front queue Packet
        q.pop();  // Then pop it

        std::mutex* mut = m_map.getLock(frontPtr->apid);  // Get mutex ptr from map
        mut->lock();  // Then lock the mutex ptr
        lock.unlock();  // Unlock queue lock so other threads can pop front (saves a bit of time)

        auto tmpTuple = std::make_tuple(std::move(frontPtr), mut);  // Create tuple to return
        retVal = 1;
        return tmpTuple;
    }

  private:
    std::queue<std::unique_ptr<DataTypes::Packet>> q;
    mutable std::mutex queueLock;
    std::condition_variable c;
    ThreadSafeStreamMap m_map;
};
