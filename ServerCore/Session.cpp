#include "pch.h"
#include "IocpEvent.h"
#include "Session.h"
#include "ServerService.h"
#include "SocketUtils.h"

/*-----------------
	  Session
------------------*/

Session::~Session()
{
	mService.reset();
	SocketUtils::CloseSocket(&mSock);
}

void Session::SetSocket(SOCKET sock)
{
	mSock = sock;
}

bool Session::IsHandleValid()
{
	return mSock != INVALID_SOCKET;
}

void Session::SetSessionId(uint64 sid)
{
	mSessionId.store(sid);
}

uint64 Session::GetSessionId()
{
	return mSessionId.load();
}

void Session::SetNetAddress(SOCKADDR_IN addr)
{
	mAddr = NetAddress(addr);
}

shared_ptr<Service> Session::GetService()
{
	ASSERT_CRASH(mService.expired() == false);
	return mService.lock();
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(mSock);
}

void Session::Dispatch(IocpEvent* event, int32 transferredBytes)
{
	switch (event->GetType())
	{
	case EventType::ACCEPT:
		ProcessAccept();
		xdelete(event);
		break;
	case EventType::CONNECT:
		ProcessConnect();
		break;
	case EventType::RECV:
		ProcessRecv(transferredBytes);
		break;
	case EventType::SEND:
		ProcessSend(transferredBytes);
		break;
	default:
		break;
	}
}

bool Session::Connect()
{
	if (IsConnected())
		return false;

	// Set ReuseAddress option
	bool option = true;
	if (SocketUtils::SetSocketOption(&mSock, SOL_SOCKET, SO_REUSEADDR, static_cast<void*>(&option), sizeof(option)) == false)
		return false;

	// Bind socket
	mAddr = NetAddress(0);  // Bind any port
	if (SocketUtils::BindSocket(&mSock, mAddr.GetSocketAddr()) == false)
		return false;

	return registerConnect();
}

bool Session::Send(const char* msg)
{
	size_t msgLen = strlen(msg) + 1;
	strcpy_s(mSendBuf, msgLen, msg);
	return registerSend();
}

void Session::Disconnect()
{
	if (mIsConnected.exchange(false) == false)
		return;  // already disconnected

	GetService()->ReleaseSession(static_pointer_cast<Session>(shared_from_this()));
	OnDisconnect();  // for user overrrided implementaion
}

void Session::ProcessAccept()
{
	shared_ptr<Service> service = GetService();
	if (service->GetType() != ServiceType::SERVER)
		return;

	SOCKET listenSock = static_pointer_cast<ServerService>(service)->GetListeningSocket();
	int32 result = SocketUtils::SetSocketOption(&mSock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, static_cast<void*>(&listenSock), sizeof(listenSock));
	if (result == false)
		return;

	// Set network address
	SOCKADDR_IN addr = {};
	int32 addrSize = sizeof(addr);
	if (::getpeername(mSock, reinterpret_cast<SOCKADDR*>(&addr), &addrSize) == SOCKET_ERROR)
	{
		Logger::log_error("Getting accept socket address failed: {}", ::WSAGetLastError());
		return;
	}
	SetNetAddress(addr);
	ProcessConnect();
}

void Session::ProcessConnect()
{
	mConnectEvent.SetOwner(nullptr);  // release reference
	mIsConnected.store(true);  // TODO : Lock °É±â

	// register session for service
	GetService()->RegisterSession(static_pointer_cast<Session>(shared_from_this()));
	registerRecv();  // register recv event for the first time
	OnConnect();  // for user overrrided implementaion
}

void Session::ProcessRecv(int32 recvBytes)
{
	mRecvEvent.SetOwner(nullptr);  // release reference

	if (recvBytes == 0)  // when disconnected
	{
		Logger::log_info("Client disconnected");
		Disconnect();
		return;
	}

	OnRecv(mRecvBuf, recvBytes);  // for user overrrided implementaion
	registerRecv();  // register recv event again
}

void Session::ProcessSend(int32 sendBytes)
{
	mSendEvent.SetOwner(nullptr);  // release reference

	if (sendBytes == 0)
	{
		Logger::log_info("Send 0");
		Disconnect();
		return;
	}

	OnSend(sendBytes);  // for user overrrided implementaion
}

bool Session::registerConnect()
{
	mConnectEvent.Init();
	mConnectEvent.SetOwner(shared_from_this());

	DWORD recvBytes = 0;
	SOCKADDR_IN& sockAddr = GetService()->GetNetAddress().GetSocketAddr();
	bool result = SocketUtils::ConnectEx(mSock, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &recvBytes, &mConnectEvent);
	if (result == false)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			mConnectEvent.SetOwner(nullptr); // release reference
			Logger::log_error("Registering connection failed: {}", error);
			return false;
		}	
	}
	return true;
}

void Session::registerRecv()
{
	if (IsConnected() == false)
		return;

	// initialize IOCP event object
	mRecvEvent.Init();
	mRecvEvent.SetOwner(shared_from_this());

	WSABUF dataBuf;
	dataBuf.buf = mRecvBuf;
	dataBuf.len = OUTPUT_BUF_SIZE;
	DWORD recvBytes = 0;
	DWORD flag = 0;

	int result = ::WSARecv(mSock, &dataBuf, 1, &recvBytes, &flag, &mRecvEvent, nullptr);
	if (result == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			Logger::log_error("Socket recv data failed: {}", error);
			mRecvEvent.SetOwner(nullptr);
		}
	}
	else  // returned immediately 
	{
		ProcessRecv(recvBytes);
	}
}

bool Session::registerSend()
{
	if (IsConnected() == false)
		return false;

	// initialize IOCP event object
	mSendEvent.Init();
	mSendEvent.SetOwner(shared_from_this());

	WSABUF dataBuf;
	dataBuf.buf = mSendBuf;
	dataBuf.len = INPUT_BUF_SIZE;

	DWORD sendBytes = 0;
	int result = ::WSASend(mSock, &dataBuf, 1, &sendBytes, 0, &mSendEvent, nullptr);
	if (result == false)
	{
		int32 error = ::WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			Logger::log_error("Socket send data failed: {}", error);
			mSendEvent.SetOwner(nullptr); // release reference
			return false;
		}
	}

	return true;
}
