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

Service::Service(uint32 ip, uint16 port, uint32 maxConnection)
	: mMaxSessionCount(maxConnection)
{
	mAddr = NetAddress(ip, port);
	mIocpCore = xmake_shared<IocpCore>();
	mListener = xmake_shared<Listener>(mMaxSessionCount);
}

Service::~Service()
{
	mIocpCore = nullptr;
	mListener = nullptr;
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
	shared_ptr<Session> session = xmake_shared<Session>();
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
	int removed = mSessions.erase(session);
	ASSERT_CRASH(removed != 0);
	--mCurrentSessionCount;
}
