#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "Session.h"
#include "ServerService.h"
#include "SocketUtils.h"

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

void ServerService::Finalize()
{
	joinIoWorkerThreads();
	mIocpCore = nullptr;

	if (mListenSock != INVALID_SOCKET)
		SocketUtils::CloseSocket(&mListenSock);
	
	SocketUtils::CleanupWS();
}

shared_ptr<Session> ServerService::CreateSession()
{
	// 반환된 소켓이 있으면 재사용한다
	{
		lock_guard<mutex> writeLock(mSocketLock);

		if (mAvailableSocks.empty() == false)
		{
			SOCKET sock = mAvailableSocks.front();
			mAvailableSocks.pop();

			shared_ptr<Session> session = mSessionFactory();
			session->SetSocket(sock);
			session->SetService(shared_from_this());

			return session;
		}
	}

	//  사용 가능한 소켓이 없으면 새로 만들어서 cp에 등록 후 사용한다
	return Service::CreateSession();
}

void ServerService::ReclaimAcceptEvent(AcceptEvent* event)
{
	// 이벤트를 다시 걸어준다
	registerAccept(event);
}

void ServerService::ReclaimSocket(SOCKET sock)
{
	lock_guard<mutex> writeLock(mSocketLock);
	mAvailableSocks.push(sock);
}

bool ServerService::start()
{
	if (canStart() == false)
	{
		Logger::log_error("Service start failed: No session factory provided");
		return false;
	}

	if (startAccept() == false)
		return false;

	return true;
}

bool ServerService::startAccept()
{
	// create listening socket
	mListenSock = SocketUtils::CreateSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!mListenSock)
		return false;

	bool option = true;
	SocketUtils::SetSocketOption(&mListenSock, SOL_SOCKET, SO_REUSEADDR, static_cast<void*>(&option), sizeof(option));

	// register listening socket for CP object
	if (mIocpCore->Register(mListenSock) == false)
		return false;

	// bind listening socket
	if (SocketUtils::BindSocket(&mListenSock, mAddr.GetSocketAddr()) == false)
		return false;

	// start listening 
	if (SocketUtils::ListenSocket(&mListenSock) == false)
		return false;

	for (uint32 i = 0; i < mMaxSessionCount; i++)
	{
		AcceptEvent* event = xnew<AcceptEvent>();
		mAcceptEvents.push_back(event);
		registerAccept(event);
	}

	return true;
}

void ServerService::registerAccept(AcceptEvent* event)
{
	shared_ptr<Session> session = CreateSession();
	if (session == nullptr)
	{
		Logger::log_error("Creating session failed, returned nullptr");
		registerAccept(event);
	}

	// reset event content for reuse
	event->Init();
	event->SetOwner(static_pointer_cast<IocpHandler>(session));

	DWORD recvBytes = 0;
	bool result = ::AcceptEx(mListenSock, session->GetSocket(), event->mRecvBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &recvBytes, static_cast<LPOVERLAPPED>(event));
	if (result == false)
	{
		int32 error = ::WSAGetLastError();
		if (error != WSA_IO_PENDING)  // WSA_IO_PENDING means the socket is successfully working on the job
		{
			Logger::log_error("Accepting client socket failed: {}", error);
			registerAccept(event);
		}
	}
}
