#include <WinSock2.h>
#include <WS2tcpip.h>

#include "pch.h"
#include "SocketUtils.h"
#include "Logger.h"

/*-----------------
	SocketUtils
------------------*/

bool SocketUtils::InitializeWS()
{
	WSADATA wsaData;

	int32 result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		Logger::log_error("WSAStartup failed: {}", result);
		return false;
	}

	return true;
}

bool SocketUtils::CleanupWS()
{
	int32 result = WSACleanup();
	if (result != 0)
	{
		Logger::log_error("WSACleanup failed: {}", result);
		return false;
	}

	return true;
}

SOCKET SocketUtils::CreateSocket(int32 af, int32 type, int32 protocol)
{
	SOCKET sock = socket(af, type, protocol);
	if (sock == INVALID_SOCKET)
	{
		Logger::log_error("Creating socket failed");
		return INVALID_SOCKET;
	}
		
	return sock;
}

bool SocketUtils::BindSocket(SOCKET* sock, int32 af, int16 port, int64 ip)
{
	SOCKADDR_IN addr = SOCKADDR_IN();
	addr.sin_family = af;  
	// 리틀 엔디안 방식의 서버 IP 주소 정수값 -> 빅 엔디안 방식의 정수값  
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);

	bind(*sock, (sockaddr*)&addr, sizeof(addr));
	int32 err = WSAGetLastError();
	if (err == SOCKET_ERROR)
	{
		Logger::log_error("Binding socket failed: {}", err);
		return false;
	}

	return true;
}

bool SocketUtils::ShutdownSocket(SOCKET* sock, int option)
{
	shutdown(*sock, option);
	int32 err = WSAGetLastError();
	if (err == SOCKET_ERROR)
	{
		Logger::log_error("Shutting down socket failed: {}", err);
		return false;
	}

	return true;
}

/*-----------------
	  Listener
------------------*/

Listener::Listener(SOCKET* sock) : mSock(sock)
{
}

bool Listener::Listen(int32 backlog)
{
	listen(*mSock, backlog);
	int32 err = WSAGetLastError();
	if (err == SOCKET_ERROR)
	{
		Logger::log_error("Listening socket failed: {}", err);
		return false;
	}

	return true;
}
