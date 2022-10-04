#include "pch.h"
#include "Session.h"
#include "Service.h"
#include "IocpCore.h"
#include "ThreadManager.h"

constexpr int32 MAX_CONNECTION = 1000;
constexpr int16 PORT = 27015;

class ChatSession : public Session
{
};

int main()
{
	// Create Service
	int32 ip = INADDR_ANY;
	int16 port = PORT;
	shared_ptr<ServerService> service = std::make_shared<ServerService>(ip, port, MAX_CONNECTION, std::make_shared<ChatSession>);
	
	ASSERT_CRASH(service->Start());

	// Create ThreadManager
	shared_ptr<ThreadManager> threadManager = std::make_shared<ThreadManager>();

	SYSTEM_INFO sysInfo = {};
	GetSystemInfo(&sysInfo);
	int32 numOfThreads = sysInfo.dwNumberOfProcessors;
	for (int i = 0; i < numOfThreads; i++)
	{
		threadManager->Work([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	cout << "Thread Created" << endl;

	threadManager->Join();
}
