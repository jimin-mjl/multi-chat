#pragma once

class Session;

enum class EventType
{
	NONE,
	CONNECT,
	ACCEPT,
	SEND,
	RECV,
	DISCONNECT,
};

/*--------------------------------------------------
						IocpEvent
	: Iocp event context 관리를 위한 데이터 클래스
---------------------------------------------------*/

class IocpHandler;
class CircularBuffer;

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent();
	IocpEvent(EventType type);
	void Init();
	void SetOwner(shared_ptr<IocpHandler> obj);
	shared_ptr<IocpHandler> GetOwner();
	EventType GetType();

private:
	EventType mType = EventType::NONE;
	shared_ptr<IocpHandler> mOwner = nullptr;
};

/*----------------
	AcceptEvent
-----------------*/

class AcceptEvent : public IocpEvent
{
	enum
	{
		BUFFER_SIZE = 1024
	};

public:
	AcceptEvent();

public:
	char mRecvBuffer[BUFFER_SIZE];
};

/*-----------------
	ConnectEvent
------------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent();
};

/*--------------
	SendEvent
---------------*/

class SendEvent : public IocpEvent
{
public:
	SendEvent();
	void Init();

public:
	vector<shared_ptr<CircularBuffer>> mSendBuffers;
};

/*--------------
	RecvEvent
---------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent();
};

/*-------------------
	DisconnectEvent
--------------------*/

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent();
	void	SetSocket(SOCKET sock) { mSock = sock; }
	SOCKET	GetSocket() { return mSock; }

public:
	SOCKET mSock = INVALID_SOCKET;
};
