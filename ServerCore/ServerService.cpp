#include "pch.h"
#include "Listener.h"
#include "ServerService.h"

/*----------------------
	  ServerService
----------------------*/

ServerService::ServerService(const char* ip, uint16 port, uint32 maxConnection, SessionFactory sf)
	: Service(ip, port, maxConnection, sf, ServiceType::SERVER)
{
}

ServerService::~ServerService()
{
}

bool ServerService::Initialize()
{
	if (Service::Initialize() == false)
		return false;

	mListener = std::make_shared<Listener>(GetMaxSessionCount());
	return true;
}

void ServerService::Finalize()
{
	Service::Finalize();
	mListener = nullptr;
}

bool ServerService::start()
{
	if (canStart() == false)
	{
		Logger::log_error("Service start failed: No session factory provided");
		return false;
	}

	if (mListener == nullptr)
	{
		Logger::log_error("Service start failed: No listener object");
		return false;
	}

	bool result = mListener->StartAccept(static_pointer_cast<ServerService>(shared_from_this()));
	if (result == false)
		return false;

	return true;
}
