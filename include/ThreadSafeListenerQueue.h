#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <chrono>

template <class T>
class ThreadSafeListenerQueue
{
public:
	ThreadSafeListenerQueue() :
		q(),
		m(),
		c()
	{}

	~ThreadSafeListenerQueue() {}

	uint32_t push(const T element)
	{
		std::lock_guard<std::mutex> lock(m);
		q.push(element);
		c.notify_one();
		return 0;
	}

	uint32_t listen(T& element)
	{
		std::unique_lock<std::mutex> lock(m);
		while (q.empty())
		{
			if(c.wait_for(lock,std::chrono::seconds(2)) == std::cv_status::timeout)
			{
				return 0;
			}
		}
		element = q.front();
		q.pop();
		return 1;
	}


private:
	std::queue<T> q;
	mutable std::mutex m;
	std::condition_variable c;
};
