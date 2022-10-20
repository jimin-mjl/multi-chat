#include "pch.h"
#include "IocpEvent.h"
#include "IocpHandler.h"

/*--------------
	IocpEvent
---------------*/

IocpEvent::IocpEvent()
{
	Init();
}

IocpEvent::IocpEvent(EventType type)
	: mType(type)
{
	Init();
}

void IocpEvent::Init()
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}

void IocpEvent::SetOwner(shared_ptr<IocpHandler> obj)
{
	mOwner = obj;
}

shared_ptr<IocpHandler> IocpEvent::GetOwner()
{
	return mOwner;
}

EventType IocpEvent::GetType()
{
	return mType;
}

/*--------------
	AcceptEvent
---------------*/

AcceptEvent::AcceptEvent()
	: IocpEvent(EventType::ACCEPT)
{
}

/*-----------------
	ConnectEvent
------------------*/

ConnectEvent::ConnectEvent()
	: IocpEvent(EventType::CONNECT)
{
}

/*--------------
	SendEvent
---------------*/

SendEvent::SendEvent()
	: IocpEvent(EventType::SEND)
{
}

void SendEvent::Init()
{
	IocpEvent::Init();
	mSendBuffers.clear();
}

/*--------------
	RecvEvent
---------------*/

RecvEvent::RecvEvent()
	: IocpEvent(EventType::RECV)
{
}

/*-------------------
	DisconnectEvent
--------------------*/

DisconnectEvent::DisconnectEvent()
	: IocpEvent(EventType::DISCONNECT)
{
}
