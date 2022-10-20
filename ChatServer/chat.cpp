#include "pch.h"
#include "Session.h"
#include "ServerService.h"
#include "IocpCore.h"


constexpr int32 MAX_CONNECTION = 5;
constexpr int16 PORT = 27015;

int main()
{
	shared_ptr<ServerService> service = std::make_shared<ServerService>("127.0.0.1", PORT, MAX_CONNECTION, std::make_shared<ChatSession>);
	ASSERT_CRASH(service->Initialize());
	service->Start();
	service->Finalize();
}
