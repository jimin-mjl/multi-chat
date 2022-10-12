#pragma once

class Session;

enum class EventType
{
	NONE,
	CONNECT,
	ACCEPT,
	SEND,
	RECV,
};

/*--------------
	IocpEvent
	: 
---------------*/

class IocpHandler;

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
public:
	AcceptEvent();
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
};

/*--------------
	RecvEvent
---------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent();
};
