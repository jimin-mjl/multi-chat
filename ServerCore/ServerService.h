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
	virtual bool Initialize() override;
	virtual void Finalize() override;

protected:
	virtual bool start() override;

private:
	shared_ptr<Listener>	mListener;
};
