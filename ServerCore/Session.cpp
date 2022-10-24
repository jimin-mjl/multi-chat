#include "pch.h"
#include "CircularBuffer.h"
#include "IocpEvent.h"
#include "Session.h"
#include "ServerService.h"
#include "SocketUtils.h"

/*-----------------
	  Session
------------------*/

Session::Session()
{
	mRecvBuffer = make_shared<CircularBuffer>(RECV_BUFFER_SIZE);
}

Session::~Session()
{
	shared_ptr<Service> service = GetService();
	if (service->GetType() == ServiceType::SERVER)
	{
		// 서버측에서는 소켓 재사용을 위해 소켓을 반납한다. 
		static_pointer_cast<ServerService>(service)->ReclaimSocket(mSock);
	}
	else
		SocketUtils::CloseSocket(&mSock);
}

shared_ptr<Service> Session::GetService()
{
	ASSERT_CRASH(mService.expired() == false);
	return mService.lock();
}

void Session::Dispatch(IocpEvent* event, int32 transferredBytes)
{
	switch (event->GetType())
	{
	case EventType::ACCEPT:
		processAccept(static_cast<AcceptEvent*>(event));
		break;
	case EventType::CONNECT:
		processConnect();
		break;
	case EventType::DISCONNECT:
		processDisconnect();
		break;
	case EventType::RECV:
		processRecv(transferredBytes);
		break;
	case EventType::SEND:
		processSend(transferredBytes);
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

void Session::Disconnect()
{
	if (mIsConnected.exchange(false) == false)
		return;  // already disconnected 
	
	registerDisconnect();
}

bool Session::Send(shared_ptr<CircularBuffer> buffer)
{
	{
		lock_guard<mutex> writeLock(mSendLock);
		mSendQueue.push(buffer);
	}

	// send 이벤트의 순차 처리를 위해 send 플래그가 false일 때만 이벤트를 등록한다.
	if (mIsSendOccupied.exchange(true) == false)
		registerSend();

	return true;
}

shared_ptr<CircularBuffer> Session::CreateSendBuffer(const char* msg, int32 len)
{
	// Send Buffer 구성
	shared_ptr<CircularBuffer> buffer = make_shared<CircularBuffer>(SEND_BUFFER_SIZE, 1);
	::memcpy(buffer->WritePos(), msg, len);
	if (buffer->OnWrite(len) == false)
		return nullptr;
	return buffer;
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

void Session::registerDisconnect() 
{
	// initialize IOCP event object
	mDisconnectEvent.Init();
	mDisconnectEvent.SetOwner(shared_from_this());

	bool result = SocketUtils::DisconnectEx(mSock, &mDisconnectEvent, TF_REUSE_SOCKET, 0);  // 소켓 재사용 플래그 전달
	if (result == false)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			mDisconnectEvent.SetOwner(nullptr); // release reference
			Logger::log_error("Register disconnect failed: {}", error);
		}
	}
}

void Session::registerRecv()
{
	if (IsConnected() == false)
		return;

	// initialize IOCP event object
	mRecvEvent.Init();
	mRecvEvent.SetOwner(shared_from_this());

	WSABUF dataBuf = {};
	dataBuf.buf = mRecvBuffer->WritePos();
	dataBuf.len = mRecvBuffer->Size();
	DWORD recvBytes = 0;
	DWORD flag = 0;

	int result = ::WSARecv(mSock, &dataBuf, 1, &recvBytes, &flag, &mRecvEvent, nullptr);
	if (result == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			mRecvEvent.SetOwner(nullptr);
			handleError("Register recv failed: {}", error);
		}
	}
}

void Session::registerSend()
{
	if (IsConnected() == false)
		return;

	// initialize IOCP event object
	mSendEvent.Init();
	mSendEvent.SetOwner(shared_from_this());

	// sendQueue에 등록된 sendBuffer를 모두 꺼내서 한꺼번에 보낸다 (scatter-gather)
	{
		lock_guard<mutex> writeLock(mSendLock);
		while (mSendQueue.empty() == false)
		{
			mSendEvent.mSendBuffers.push_back(mSendQueue.front());
			mSendQueue.pop();
		}
	}

	vector<WSABUF> dataBufs;
	dataBufs.reserve(mSendEvent.mSendBuffers.size());
	for (shared_ptr<CircularBuffer> buffer : mSendEvent.mSendBuffers)
	{
		WSABUF dataBuf = {};
		dataBuf.buf = buffer->ReadPos();
		dataBuf.len = buffer->DataSize();
		dataBufs.push_back(dataBuf);
	}

	DWORD sendBytes = 0;
	int result = ::WSASend(mSock, dataBufs.data(), static_cast<DWORD>(dataBufs.size()), &sendBytes, 0, &mSendEvent, nullptr);
	if (result == SOCKET_ERROR)
	{
		int32 error = ::WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			mSendEvent.SetOwner(nullptr); // release reference
			mIsSendOccupied.store(false);
			handleError("Register send failed: {}", error);
		}
	}
}

void Session::processAccept(AcceptEvent* event)
{
	event->SetOwner(nullptr);  // release ref

	shared_ptr<ServerService> service = static_pointer_cast<ServerService>(GetService());
	if (service->GetType() != ServiceType::SERVER)
		return;

	SOCKET listenSock = service->GetListeningSocket();
	int32 result = SocketUtils::SetSocketOption(&mSock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, static_cast<void*>(&listenSock), sizeof(listenSock));
	if (result == false)
		return;

	// Set network address
	SOCKADDR_IN addr = {};
	int32 addrSize = sizeof(addr);
	if (::getpeername(mSock, reinterpret_cast<SOCKADDR*>(&addr), &addrSize) == SOCKET_ERROR)
	{
		Logger::log_error("Get client socket address failed: {}", ::WSAGetLastError());
		return;
	}
	SetNetAddress(addr);
	processConnect();
	service->ReclaimAcceptEvent(event);  // 이벤트 반납
}

void Session::processConnect()
{
	mConnectEvent.SetOwner(nullptr);  // release reference
	mIsConnected.store(true);  

	// register session for service
	GetService()->RegisterSession(static_pointer_cast<Session>(shared_from_this()));
	registerRecv();  // register recv event for the first time
	OnConnect();  // for user overrrided implementaion
}

void Session::processDisconnect()
{
	mDisconnectEvent.SetOwner(nullptr);  // release reference
	GetService()->ReleaseSession(static_pointer_cast<Session>(shared_from_this()));
	OnDisconnect();
}

void Session::processRecv(int32 recvBytes)
{
	mRecvEvent.SetOwner(nullptr);  // release reference

	if (recvBytes == 0)  // when disconnected
	{
		cout << "recv 0" << endl;
		handleError("Client disconnected");
		return;
	}

	if (mRecvBuffer->OnWrite(recvBytes) == false)
	{
		handleError("Write overflow");
		return;
	}

	int32 dataSize = mRecvBuffer->DataSize();
	int32 processedBytes = OnRecv(mRecvBuffer->ReadPos(), dataSize);
	if (processedBytes < 0 || dataSize < processedBytes ||  mRecvBuffer->OnRead(processedBytes) == false)
	{
		handleError("Read overflow");
		return;
	}

	registerRecv();  // register recv event again
}

void Session::processSend(int32 sendBytes)
{
	mSendEvent.SetOwner(nullptr);  // release reference

	if (sendBytes == 0)
	{
		handleError("Send 0");
		return;
	}

	OnSend(sendBytes);  // for user implementaion
	
	// sendQueue를 확인해서 남은 buffer가 있으면 마저 보내고 아니면 플래그를 풀어준다. 
	{
		lock_guard<mutex> writeLock(mSendLock);
		if (mSendQueue.empty() == false)
			registerSend();
		else
			mIsSendOccupied.store(false);
	}
}

void Session::handleError(const char* reason, int32 error)
{
	Logger::log_info(reason, error);
	Disconnect();
}
