#include "pch.h"
#include "Session.h"
#include "ServerService.h"
#include "IocpCore.h"

#include "SocketUtils.h"
#include "NetAddress.h"

constexpr int32 MAX_CONNECTION = 5;
constexpr int16 PORT = 27015;

class ChatSession : public Session
{
public:
	virtual void OnConnect() override
	{
		cout << "Connected" << endl;
		// Send("hi from client");
	}

	virtual void OnSend(int32 sendBytes) override
	{
		cout << "Msg sent : " << sendBytes << endl;
	}

	virtual int32 OnRecv(char* buffer, int32 recvBytes)
	{
		cout << "recv bytes: " << recvBytes << endl;
		cout << "recv content: " << buffer << endl;

		Send("hi from server");
		return recvBytes;
	}

	virtual void OnDisconnect()
	{
		cout << "Disconnected" << endl;
	}
};

int main()
{
	shared_ptr<ServerService> service = std::make_shared<ServerService>("127.0.0.1", PORT, MAX_CONNECTION, std::make_shared<ChatSession>);
	ASSERT_CRASH(service->Initialize());
	service->Start();
	service->Finalize();
}
