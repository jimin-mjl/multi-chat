#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "SocketUtils.h"
#include "IocpCore.h"
#include "NetAddress.h"
#include "Listener.h"

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
	for (thread& worker : mIoWorkerThreads)
	{
		if (worker.joinable())
			worker.join();
	}

	mIoWorkerThreads.clear();

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
	shared_ptr<Session> session = createEmptySession();
	SOCKET sock = SocketUtils::CreateSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!sock)
		return nullptr;

	session->SetSocket(sock);
	session->SetService(shared_from_this());
	if (mIocpCore->Register(session) == false)
		return nullptr;

	return session;
}

//uint64 Service::generateSessionId()
//{
//	// TODO : compare_exchange_weak
//	lock_guard<mutex> lg(mMutex);
//	return ++mCurrentSessionCount;
//}

void Service::startIoWorkerThreads()
{
	SYSTEM_INFO sysInfo = {};
	GetSystemInfo(&sysInfo);
	int32 numOfThreads = sysInfo.dwNumberOfProcessors;

	for (int32 i = 0; i < numOfThreads; i++)
	{
		mIoWorkerThreads.push_back(thread(&IocpCore::Dispatch, mIocpCore, INFINITE));
	}
}

shared_ptr<Session> Service::createEmptySession()
{
	shared_ptr<Session> session = mSessionFactory();
	return session;
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

/*----------------------
	  ServerService
----------------------*/

ServerService::ServerService(const char* ip, uint16 port, uint32 maxConnection, SessionFactory sf)
	: Service(ip, port, maxConnection, sf, ServiceType::SERVER)
{
}

ServerService::~ServerService()
{
}

bool ServerService::Initialize()
{
	if (Service::Initialize() == false)
		return false;

	mListener = std::make_shared<Listener>(GetMaxSessionCount());
	return true;
}

void ServerService::Finalize()
{
	Service::Finalize();
	mListener = nullptr;
}

bool ServerService::start()
{
	if (canStart() == false)
	{
		Logger::log_error("Service start failed: No session factory provided");
		return false;
	}

	if (mListener == nullptr)
	{
		Logger::log_error("Service start failed: No listener object");
		return false;
	}
		
	bool result = mListener->StartAccept(static_pointer_cast<ServerService>(shared_from_this()));
	if (result == false)
		return false;

	return true;
}

/*----------------------
	  ClientService
----------------------*/

ClientService::ClientService(const char* ip, uint16 port, uint32 maxConnection, SessionFactory sf)
	: Service(ip, port, maxConnection, sf, ServiceType::CLIENT)
{

}

bool ClientService::start()
{
	if (canStart() == false)
		return false;

	int32 sessionCount = GetMaxSessionCount();
	for (int32 i = 0; i < sessionCount; i++)
	{
		shared_ptr<Session> session = CreateSession();

		if (session == nullptr)
			return false;

		if (session->Connect() == false)
			return false;
	}

	return true;
}