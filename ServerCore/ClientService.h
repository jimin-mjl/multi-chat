#pragma once
#include "Service.h"

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
