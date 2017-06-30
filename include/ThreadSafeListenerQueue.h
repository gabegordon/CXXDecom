#pragma once

#include <list>
#include <mutex>
#include <condition_variable>

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

	int push(const T element)
	{
		std::lock_guard<std::mutex> lock(m);
		q.push_back(element);
		c.notify_one();
		return 0;
	}

	int listen(T& element)
	{
		std::unique_lock<std::mutex> lock(m);
		while (q.empty())
		{
			c.wait(lock);
		}
		element = q.front();
		q.pop_front();
		return 0;
	}
private:
	std::list<T> q;
	mutable std::mutex m;
	std::condition_variable c;
};
