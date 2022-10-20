#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "SocketUtils.h"
#include "IocpCore.h"
#include "NetAddress.h"

/*-----------------
	  Service
------------------*/

Service::Service(const char* ip, uint16 port, uint32 maxConnection, SessionFactory sf, ServiceType type)
	: mMaxSessionCount(maxConnection)
	, mSessionFactory(sf)
	, mType(type)
{
	mAddr = NetAddress(ip, port);
}

Service::~Service()
{
}

bool Service::Initialize()
{
	// winsock initialization
	if (SocketUtils::InitializeWS() == false)
		return false;

	// create IO completion port
	mIocpCore = std::make_shared<IocpCore>();
	return true;
}

void Service::Finalize()
{
	joinIoWorkerThreads();
	mIocpCore = nullptr;
	SocketUtils::CleanupWS();
}

void Service::Start()
{
	startIoWorkerThreads();
	ASSERT_CRASH(start());
}

shared_ptr<Session> Service::CreateSession()
{
	shared_ptr<Session> session = mSessionFactory();

	SOCKET sock = SocketUtils::CreateSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!sock)
		return nullptr;

	session->SetSocket(sock);
	session->SetService(shared_from_this());
	if (mIocpCore->Register(sock) == false)
		return nullptr;

	return session;
}

void Service::startIoWorkerThreads()
{
	SYSTEM_INFO sysInfo = {};
	GetSystemInfo(&sysInfo);
	int32 numOfThreads = sysInfo.dwNumberOfProcessors;

	for (int32 i = 0; i < numOfThreads; i++)
	{
		mIoWorkerThreads.push_back(thread([=]() 
			{
				while (true)
				{
					mIocpCore->Dispatch(INFINITE);
				}
			}));
	}
}

void Service::joinIoWorkerThreads()
{
	for (thread& worker : mIoWorkerThreads)
	{
		if (worker.joinable())
			worker.join();
	}

	mIoWorkerThreads.clear();
}

void Service::RegisterSession(shared_ptr<Session> session)
{
	lock_guard<mutex> guard(mMutex);
	mSessions.insert(session);
	++mCurrentSessionCount;
}

void Service::ReleaseSession(shared_ptr<Session> session)
{
	lock_guard<mutex> guard(mMutex);
	size_t removed = mSessions.erase(session);
	ASSERT_CRASH(removed != 0);
	--mCurrentSessionCount;
}
