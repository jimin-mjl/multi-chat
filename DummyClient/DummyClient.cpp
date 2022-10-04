#include "pch.h"
#include "IocpCore.h"
#include "Session.h"
#include "Service.h"
#include "ThreadManager.h"

constexpr int32 MAX_CONNECTION = 1000;
constexpr int16 PORT = 27015;

class ServerSession : public Session
{
public:
	virtual void OnConnect() override
	{
		cout << "Connected" << endl;
	}
};

int main()
{
	this_thread::sleep_for(1s);

	// Create Service
	int32 ip = 0;
	int16 port = PORT;
	inet_pton(AF_INET, "127.0.0.1", &ip);
	shared_ptr<ClientService> service = std::make_shared<ClientService>(ip, port, MAX_CONNECTION, std::make_shared<ServerSession>);

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
