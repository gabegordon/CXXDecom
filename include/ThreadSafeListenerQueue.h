#pragma once

#include <queue>
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
		q.push(std::move(element));
		c.notify_one();
		return 0;
	}

	T listen()
	{
		std::unique_lock<std::mutex> lock(m);
		while (q.empty())
		{
			c.wait(lock);
		}
		T element = std::move(q.front());
		q.pop();
		return element;
	}
private:
	std::queue<T> q;
	mutable std::mutex m;
	std::condition_variable c;
};
