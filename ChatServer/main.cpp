#include "pch.h"
#include "ChatSession.h"
#include "ChatSessionManager.h"
#include "Session.h"
#include "ServerService.h"
#include "IocpCore.h"


constexpr int32 MAX_CONNECTION = 1000;
constexpr int16 PORT = 27015;

int main()
{
	GSessionManager = new ChatSessionManager;
	
	shared_ptr<ServerService> service = std::make_shared<ServerService>("127.0.0.1", PORT, MAX_CONNECTION, std::make_shared<ChatSession>);
	ASSERT_CRASH(service->Initialize());
	service->Start();
	service->Finalize();

	delete GSessionManager;
}
