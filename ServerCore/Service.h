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
	Service(const char* ip, uint16 port, uint32 maxConnection, SessionFactory sf, ServiceType type);
	virtual ~Service();

public:
	shared_ptr<IocpCore>	GetIocpCore() { return mIocpCore; }
	NetAddress&				GetNetAddress() { return mAddr; }
	ServiceType				GetType() { return mType; }
	uint32					GetMaxSessionCount() { return mMaxSessionCount; }
	uint32					GetCurrSessionCount() { return mCurrentSessionCount; }
	
public:
	virtual bool			Initialize();
	virtual void			Finalize();
	void					Start();
	shared_ptr<Session>		CreateSession();
	void					RegisterSession(shared_ptr<Session> session);
	void					ReleaseSession(shared_ptr<Session> session);

protected:
	bool					canStart() { return mSessionFactory != nullptr; }
	virtual bool			start() abstract;

private:
	// uint64					generateSessionId();
	void					startIoWorkerThreads();
	shared_ptr<Session>		createEmptySession();

private:
	mutex						mMutex;
	ServiceType					mType;
	NetAddress					mAddr;
	set<shared_ptr<Session>>	mSessions;
	SessionFactory				mSessionFactory = nullptr;
	uint32						mCurrentSessionCount = 0;
	uint32						mMaxSessionCount = 0;
	vector<thread>				mIoWorkerThreads;

private:
	shared_ptr<IocpCore>		mIocpCore;
};

/*----------------------
	  ServerService
----------------------*/

class ServerService : public Service
{
public:
	ServerService(const char* ip, uint16 port, uint32 maxConnection, SessionFactory sf);
	virtual ~ServerService();

public:
	virtual bool Initialize() override;
	virtual void Finalize() override;

protected:
	virtual bool start() override;

private:
	shared_ptr<Listener>	mListener;
};

/*----------------------
	  ClientService
----------------------*/

class ClientService : public Service
{
public:
	ClientService(const char* ip, uint16 port, uint32 maxConnection, SessionFactory sf);
	virtual ~ClientService() {}

protected:
	virtual bool start() override;
};
