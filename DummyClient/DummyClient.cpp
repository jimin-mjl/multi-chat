#include "pch.h"
#include "ClientService.h"
#include "IocpCore.h"
#include "Session.h"

constexpr int32 MAX_CONNECTION = 5;
constexpr int16 PORT = 27015;

class ServerSession : public Session
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

	virtual int32 OnRecv(char* buffer, int32 recvBytes) override
	{
		cout << "recv bytes: " << recvBytes << endl;
		cout << "recv content: " << buffer << endl;
		Send("hola from client");
		return recvBytes;
	}

	virtual void OnDisconnect() override
	{
		cout << "disconnected" << endl;
	}
};

int main()
{
	this_thread::sleep_for(1s); 

	// Create Service
	shared_ptr<ClientService> service = std::make_shared<ClientService>("127.0.0.1", PORT, MAX_CONNECTION, std::make_shared<ServerSession>);
	ASSERT_CRASH(service->Initialize());
	service->Start();

	// pragma test
	/*SOCKET sock = socket(PF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addr;
	::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);
	addr.sin_port = htons(PORT);
	int res = ::connect(sock, (sockaddr*)&addr, sizeof(addr));
	if (res == SOCKET_ERROR)
	{
		cout << "Connection status: " << ::WSAGetLastError() << endl;
	}
	else cout << "Connection Succeed" << endl;

	this_thread::sleep_for(5s);
	closesocket(sock);*/
	// end pragma test

	service->Finalize();
}
