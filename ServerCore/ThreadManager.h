#pragma once
// #include <mutex>
#include <functional>  // enables to store callable object 

/*-----------------------------------------------
					ThreadManager
	: Multi-thread management class responsible 
	for creating and joining threads
------------------------------------------------*/

class ThreadManager
{
public:
	ThreadManager() = default;
	~ThreadManager();
	  
public:
	void Work(function<void(void)> callback);  // pass callback function as an object
	void Join();

private:
	mutex			mMutex;
	vector<thread>	mWorkers;
};
