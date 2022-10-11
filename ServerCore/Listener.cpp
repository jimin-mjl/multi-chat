#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "Listener.h"
#include "SocketUtils.h"
#include "Service.h"
#include "Session.h"

/*-----------------
	  Listener
------------------*/

Listener::Listener(int limit)
	: mMaxConn(limit)
{
}

Listener::~Listener()
{
	mService.reset();

	if (mSock != INVALID_SOCKET)
		SocketUtils::CloseSocket(&mSock);
}

void Listener::SetMaxConnection(int limit)
{
	mMaxConn = limit;
}

void Listener::SetService(shared_ptr<ServerService> service)
{
	mService = service;
}

int Listener::GetMaxConnection()
{
	return mMaxConn;
}

shared_ptr<ServerService> Listener::GetService()
{
	ASSERT_CRASH(mService.expired() == false);
	return mService.lock();
}

bool Listener::StartAccept(shared_ptr<ServerService> service)
{
	SetService(service); 

	// create listening socket
	if (createSocket() == false)
		return false;

	setReuseAddr();

	// register listening socket for CP object
	if (service->GetIocpCore()->Register(shared_from_this()) == false) 
		return false;

	if (bindSocket(service->GetNetAddress()) == false)
		return false;

	if (listenSocket() == false)
		return false;

	for (int i = 0; i < mMaxConn; i++)
	{
		AcceptEvent* event = xnew<AcceptEvent>();
		event->SetOwner(shared_from_this());
		mAcceptEvents.push_back(event);
		registerAccept(event);
	}

	return true;
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(mSock);
}

bool Listener::IsHandleValid()
{
	return mSock != INVALID_SOCKET;
}

void Listener::Dispatch(IocpEvent* event, int32 bytes)
{
	processAccept(static_cast<AcceptEvent*>(event));
}

bool Listener::createSocket()
{
	SOCKET sock = SocketUtils::CreateSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!sock)
		return false;

	mSock = sock;
	return true;
}

bool Listener::setReuseAddr()
{
	bool option = true;
	return SocketUtils::SetSocketOption(&mSock, SOL_SOCKET, SO_REUSEADDR, static_cast<void*>(&option), sizeof(option));
}

// Set listen socket options to accept socket
bool Listener::setUpdateAcceptContext(SOCKET sock)
{
	return SocketUtils::SetSocketOption(&mSock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, static_cast<void*>(&sock), sizeof(sock));
}

bool Listener::bindSocket(NetAddress& addr)
{
	if (SocketUtils::BindSocket(&mSock, addr.GetSocketAddr()) == false)
		return false;
	return true;
}

bool Listener::listenSocket()
{
	if (SocketUtils::ListenSocket(&mSock) == false)
		return false;
	return true;
}

void Listener::registerAccept(AcceptEvent* event)
{
	shared_ptr<Session> session = GetService()->CreateSession();
	if (session == nullptr)
	{
		Logger::log_error("Creating session failed, returned nullptr");
		registerAccept(event);
	}

	// reset event content for reuse
	event->Init();
	event->SetSession(session);

	DWORD recvBytes = 0;

	SOCKADDR_IN addr = {};
	int32 aSize = sizeof(addr);
	// SOCKET clientSock = ::accept(mSock, (sockaddr*) & addr, &aSize);
	// if (clientSock != INVALID_SOCKET) cout << "socket accepted" << endl;
	bool result = ::AcceptEx(mSock, session->GetSocket(), session->mRecvBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &recvBytes, static_cast<LPOVERLAPPED>(event));
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

void Listener::processAccept(AcceptEvent* event)
{
	shared_ptr<Session> session = event->GetSession();
	if (setUpdateAcceptContext(session->GetSocket()) == false)
	{
		registerAccept(event);
		return;
	}

	// Set session net address
	SOCKADDR_IN addr = {};
	int32 addrSize = sizeof(addr);
	if (::getpeername(session->GetSocket(), reinterpret_cast<SOCKADDR*>(&addr), &addrSize) == SOCKET_ERROR)
	{
		Logger::log_error("Getting accept socket address failed: {}", ::WSAGetLastError());
		registerAccept(event);
		return;
	}

	session->SetNetAddress(addr);
	session->ProcessConnect(); 

	registerAccept(event);  // reuse event object
}
