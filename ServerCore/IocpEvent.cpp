#include "pch.h"
#include "IocpEvent.h"
#include "IocpObject.h"

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

void IocpEvent::SetOwner(shared_ptr<IocpObject> obj)
{
	mOwner = obj;
}

shared_ptr<IocpObject> IocpEvent::GetOwner()
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

/*--------------
	RecvEvent
---------------*/

RecvEvent::RecvEvent()
	: IocpEvent(EventType::RECV)
{
}
