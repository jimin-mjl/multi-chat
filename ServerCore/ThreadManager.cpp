#include "pch.h"
#include "ThreadManager.h"

/*-----------------
	ThreadManager
------------------*/

ThreadManager::~ThreadManager()
{
	if (!mWorkers.empty())
		Join();
}

void ThreadManager::Work(std::function<void(void)> callback)
{
	lock_guard<mutex> lock(mMutex);
	mWorkers.push_back(thread(callback));
}

void ThreadManager::Join()
{
	for (thread& worker : mWorkers)
	{
		if (worker.joinable())
			worker.join();
	}

	mWorkers.clear();
}
