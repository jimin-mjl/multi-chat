#include "pch.h"
#include "Session.h"
#include "ServerService.h"
#include "IocpCore.h"
#include "ThreadManager.h"

#include "SocketUtils.h"
#include "NetAddress.h"

constexpr int32 MAX_CONNECTION = 10;
constexpr int16 PORT = 27015;

class ChatSession : public Session
{
};

int main()
{
	// Create Service
	shared_ptr<ServerService> service = std::make_shared<ServerService>("127.0.0.1", PORT, MAX_CONNECTION, std::make_shared<ChatSession>);
	
	ASSERT_CRASH(service->Initialize());
	service->Start();

	/*int option = true;
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	SocketUtils::SetSocketOption(&sock, SOL_SOCKET, SO_REUSEADDR, static_cast<void*>(&option), sizeof(option));
	
	SOCKADDR_IN saddr;
	::memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(PORT);
	SocketUtils::BindSocket(&sock, saddr);
	SocketUtils::ListenSocket(&sock);

	SOCKADDR_IN addr = {};
	int32 aSize = sizeof(addr);
	cout << "listening" << endl;
	SOCKET clientSock = ::accept(sock, (sockaddr*)&addr, &aSize);
	if (clientSock != INVALID_SOCKET) cout << "socket accepted" << endl;
	else cout << ::WSAGetLastError() << endl;*/

	service->Finalize();
}
