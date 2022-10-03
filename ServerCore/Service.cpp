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

Service::Service(uint32 ip, uint16 port, uint32 maxConnection, SessionFactory sf, ServiceType type)
	: mMaxSessionCount(maxConnection)
	, mSessionFactory(sf)
	, mType(type)
{
	SocketUtils::InitializeWS();
	mAddr = NetAddress(ip, port);
	mIocpCore = std::make_shared<IocpCore>();
}

Service::~Service()
{
	mIocpCore = nullptr;
	SocketUtils::CleanupWS();
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

ServerService::ServerService(uint32 ip, uint16 port, uint32 maxConnection, SessionFactory sf)
	: Service(ip, port, maxConnection, sf, ServiceType::SERVER)
{
	mListener = std::make_shared<Listener>(maxConnection);
}

ServerService::~ServerService()
{
	mListener = nullptr;
}

bool ServerService::Start()
{
	if (CanStart() == false)
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

ClientService::ClientService(uint32 ip, uint16 port, uint32 maxConnection, SessionFactory sf)
	: Service(ip, port, maxConnection, sf, ServiceType::CLIENT)
{

}