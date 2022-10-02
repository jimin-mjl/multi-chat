#pragma once
#include "NetAddress.h"

class Session;
class IocpCore;
class Listener;

/*--------------------------------------------
					Service
	: User interface class responsible for 
	TCP communication and session management
---------------------------------------------*/

class Service : public enable_shared_from_this<Service>
{
public:
	Service(uint32 ip, uint16 port, uint32 maxConnection);
	virtual ~Service();

public:
	shared_ptr<IocpCore>	GetIocpCore() { return mIocpCore; }
	NetAddress&				GetNetAddress() { return mAddr; }
	shared_ptr<Session>		CreateSession();
	void					RegisterSession(shared_ptr<Session> session);
	void					ReleaseSession(shared_ptr<Session> session);

private:
	// uint64					generateSessionId();
	shared_ptr<Session>		createEmptySession();

private:
	mutex						mMutex;
	NetAddress					mAddr;
	set<shared_ptr<Session>>	mSessions;
	uint32						mCurrentSessionCount = 0;
	uint32						mMaxSessionCount = 0;

private:
	shared_ptr<IocpCore>	mIocpCore;
	shared_ptr<Listener>	mListener;
};
