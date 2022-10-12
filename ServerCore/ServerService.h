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
	virtual void	Finalize() override;
	SOCKET			GetListeningSocket() { return mListenSock; }

protected:
	virtual bool	start() override;
	bool			startAccept();
	void			registerAccept(AcceptEvent* event);

private:
	SOCKET mListenSock = INVALID_SOCKET;
};
