#pragma once
#include "NetAddress.h"

class Session;
class IocpCore;

enum class ServiceType : int16
{
	SERVER,
	CLIENT,
};

/*--------------------------------------------
					Service
	: User interface class responsible for 
	IOCP management and session management
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
	void					startIoWorkerThreads();
	void					joinIoWorkerThreads();

protected:
	mutex						mMutex;
	ServiceType					mType;
	NetAddress					mAddr;
	shared_ptr<IocpCore>		mIocpCore;
	set<shared_ptr<Session>>	mSessions;
	SessionFactory				mSessionFactory = nullptr;
	uint32						mCurrentSessionCount = 0;
	uint32						mMaxSessionCount = 0;
	vector<thread>				mIoWorkerThreads;
};
