#include "pch.h"
#include "ClientService.h"
#include "Session.h"

/*----------------------
	  ClientService
----------------------*/

ClientService::ClientService(const char* ip, uint16 port, uint32 maxConnection, SessionFactory sf)
	: Service(ip, port, maxConnection, sf, ServiceType::CLIENT)
{

}

bool ClientService::start()
{
	if (canStart() == false)
		return false;

	int32 sessionCount = GetMaxSessionCount();
	for (int32 i = 0; i < sessionCount; i++)
	{
		shared_ptr<Session> session = CreateSession();

		if (session == nullptr)
			return false;

		if (session->Connect() == false)
			return false;
	}

	return true;
}
