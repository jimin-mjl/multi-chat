#include "pch.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"
#include "SocketUtils.h"

/*-----------------
	  Session
------------------*/

void Session::SetSocket(SOCKET sock)
{
	mSock = sock;
}

Session::~Session()
{
	mService.reset();
	SocketUtils::CloseSocket(&mSock);
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

void Session::SetNetAddress(uint32 ip, uint16 port)
{
	mAddr = NetAddress(ip, port);
}

uint32 Session::GetNetIp()
{
	return mAddr.ip;
}

uint16 Session::GetNetPort()
{
	return mAddr.port;
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

void Session::Dispatch(IocpEvent* event, int32 recvBytes)
{
	switch (event->GetType())
	{
	case EventType::CONNECT:
		ProcessConnect();
		break;
	case EventType::RECV:
		ProcessRecv(recvBytes);
		break;
	case EventType::SEND:
		// ProcessSend();
		break;
	default:
		break;
	}
}

void Session::RegisterRecv()
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

	int result = ::WSARecv(GetSocket(), &dataBuf, 1, &recvBytes, &flag, &mRecvEvent, nullptr);
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

void Session::ProcessConnect()
{
	// register connect 시 필요한 라인 
	// mConnectEvent.SetOwner(nullptr);  // release reference
	mIsConnected.store(true);

	// register session for service
	GetService()->RegisterSession(static_pointer_cast<Session>(shared_from_this()));
	OnConnect();  // for user overrrided implementaion
	RegisterRecv();  // register recv event for the first time
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
	RegisterRecv();  // register recv event again
}

void Session::Disconnect()
{
	if (mIsConnected.exchange(false) == false)
		return;  // already disconnected

	GetService()->ReleaseSession(static_pointer_cast<Session>(shared_from_this()));
	OnDisconnect();  // for user overrrided implementaion
}
