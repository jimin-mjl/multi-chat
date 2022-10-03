#pragma once
#include "NetAddress.h"

class Session;
class IocpCore;
class Listener;

enum class ServiceType : int16
{
	SERVER,
	CLIENT,
};

/*--------------------------------------------
					Service
	: User interface class responsible for 
	TCP communication and session management
---------------------------------------------*/

using SessionFactory = function<shared_ptr<Session>(void)>;

class Service : public enable_shared_from_this<Service>
{
public:
	Service(uint32 ip, uint16 port, uint32 maxConnection, SessionFactory sf, ServiceType type);
	virtual ~Service();

public:
	shared_ptr<IocpCore>	GetIocpCore() { return mIocpCore; }
	NetAddress&				GetNetAddress() { return mAddr; }
	bool					CanStart() { return mSessionFactory != nullptr; }
	virtual bool			Start() abstract;
	shared_ptr<Session>		CreateSession();
	void					RegisterSession(shared_ptr<Session> session);
	void					ReleaseSession(shared_ptr<Session> session);

private:
	// uint64					generateSessionId();
	shared_ptr<Session>		createEmptySession();

private:
	mutex						mMutex;
	ServiceType					mType;
	NetAddress					mAddr;
	set<shared_ptr<Session>>	mSessions;
	SessionFactory				mSessionFactory = nullptr;
	uint32						mCurrentSessionCount = 0;
	uint32						mMaxSessionCount = 0;

private:
	shared_ptr<IocpCore>		mIocpCore;
};

/*----------------------
	  ServerService
----------------------*/

class ServerService : public Service
{
public:
	ServerService(uint32 ip, uint16 port, uint32 maxConnection, SessionFactory sf);
	virtual ~ServerService();

public:
	virtual bool Start() override;

private:
	shared_ptr<Listener>	mListener;
};

/*----------------------
	  ClientService
----------------------*/

class ClientService : public Service
{
public:
	ClientService(uint32 ip, uint16 port, uint32 maxConnection, SessionFactory sf);
	// virtual bool Start() override;
};
