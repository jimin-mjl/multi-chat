#include "pch.h"
#include "Session.h"
#include "ServerService.h"
#include "IocpCore.h"

#include "SocketUtils.h"
#include "NetAddress.h"

constexpr int32 MAX_CONNECTION = 10;
constexpr int16 PORT = 27015;

class ChatSession : public Session
{
public:
	virtual int32 OnRecv(char* buffer, int32 recvBytes)
	{
		cout << "Recv Bytes: " << recvBytes << endl;

		Send("hi");
		return recvBytes;
	}
};

int main()
{
	shared_ptr<ServerService> service = std::make_shared<ServerService>("127.0.0.1", PORT, MAX_CONNECTION, std::make_shared<ChatSession>);
	
	ASSERT_CRASH(service->Initialize());
	
	service->Start();

	service->Finalize();
}
