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

class IocpObject;

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent();
	IocpEvent(EventType type);
	void Init();
	void SetOwner(shared_ptr<IocpObject> obj);
	shared_ptr<IocpObject> GetOwner();
	EventType GetType();

private:
	EventType mType = EventType::NONE;
	shared_ptr<IocpObject> mOwner = nullptr;
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
