#pragma once
#include "Service.h"

/*----------------------
	  ServerService
----------------------*/

class ServerService : public Service
{
public:
	ServerService(const char* ip, uint16 port, uint32 maxConnection, SessionFactory sf);
	virtual ~ServerService();

public:
	virtual void					Finalize() override;
	SOCKET							GetListeningSocket() { return mListenSock; }
	virtual shared_ptr<Session>		CreateSession() override;	
	void							ReclaimAcceptEvent(AcceptEvent* event);
	void							ReclaimSocket(SOCKET sock);

protected:
	virtual bool					start() override;
	bool							startAccept();
	void							registerAccept(AcceptEvent* event);

private:
	mutex					mSocketLock;
	SOCKET					mListenSock = INVALID_SOCKET;
	vector<AcceptEvent*>	mAcceptEvents;
	queue<SOCKET>			mAvailableSocks;  // 馆券等 家南 历厘家
};
