#pragma once
#include "Session.h"

/*-----------------
	ChatSession
------------------*/

class ChatSession : public Session
{
public:
	virtual void OnConnect() override;
	virtual void OnDisconnect();
	virtual int32 OnRecv(char* buffer, int32 recvBytes) override;
	virtual void OnSend(int32 sendBytes) override;
};
