#include <chrono>
#include <iostream>
#include "ThreadSafeListenerQueue.h"

/**
 * Pushes a Packet pointer into the queue. Notifies one waiting listener.
 *
 * @param element unique_ptr to be pushed
 * @return N/A
 */
void ThreadSafeListenerQueue::push(std::unique_ptr<DataTypes::Packet> element)
{
    std::lock_guard<std::mutex> lock(queueLock);  // Lock mutex to prevent simultaneous push and pops
    q.push(std::move(element));  // Move pointer into queue
    c.notify_one();  // Notify one waiting thread
}

/**
 * Function that blocks while queue is empty on condition_variable c. Once notified pops pointer from queue and acquires lock.
 *
 * @param retVal Integer passed by referenec to get return value. 1 on success. 0 on timeout.
 * @return pointer to Packet
 */
std::unique_ptr<DataTypes::Packet> ThreadSafeListenerQueue::listen(uint32_t& retVal)
{
    std::unique_lock<std::mutex> lock(queueLock);  // Get mutex lock
    while (q.empty()) // While loop checking empty to account for spurious wakeups
    {
        if (c.wait_for(lock, std::chrono::seconds(1)) == std::cv_status::timeout)  // Wait on condition var until notified, or 1 second timeout is reached
        {
            if(!m_active && q.empty())
            {
                retVal = 0;  // Reached timeout and inactive, return 0 to let thread know to terminate
                return nullptr;
            }
        }
    }

    auto frontPtr = std::move(q.front());  // Once notified save front queue Packet
    q.pop();  // Then pop it
    retVal = 1;
    return frontPtr;
}

/**
 * Sets member variable m_active to false. This causes threads to terminate once queue becomes empty.
 *
 * @return N/A
 */
void ThreadSafeListenerQueue::setInactive()
{
    std::lock_guard<std::mutex> lock(queueLock);
    m_active = false;
}
