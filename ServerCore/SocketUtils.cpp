#include "pch.h"
#include "NetAddress.h"
#include "SocketUtils.h"

/*-----------------
	SocketUtils
------------------*/

bool SocketUtils::InitializeWS()
{
	WSADATA wsaData;

	int32 result = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		Logger::log_error("WSAStartup failed: {}", result);
		return false;
	}

	return true;
}

void SocketUtils::CleanupWS()
{
	::WSACleanup();
}

SOCKET SocketUtils::CreateSocket(int32 af, int32 type, int32 protocol)
{
	// creates non-blocking socket
	u_long mode = 1;
	SOCKET sock = ::WSASocket(af, type, protocol, 0, NULL, WSA_FLAG_OVERLAPPED);
	::ioctlsocket(sock, FIONBIO, &mode);
	if (sock == INVALID_SOCKET)
	{
		Logger::log_error("Creating socket failed");
		return INVALID_SOCKET;
	}
		
	return sock;
}

bool SocketUtils::BindSocket(SOCKET* sock, int32 af, const NetAddress& netaddr)
{
	SOCKADDR_IN addr = SOCKADDR_IN();
	addr.sin_family = af;  
	addr.sin_addr.s_addr = netaddr.ip;
	addr.sin_port = netaddr.port;

	int32 result = ::bind(*sock, (sockaddr*)&addr, sizeof(addr));
	if (result == SOCKET_ERROR)
	{
		Logger::log_error("Binding socket failed: {}", result);
		return false;
	}

	return true;
}

bool SocketUtils::ListenSocket(SOCKET* sock, int32 backlog)
{
	int32 result = ::listen(*sock, backlog);
	if (result == SOCKET_ERROR)
	{
		Logger::log_error("Listening socket failed: {}", result);
		return false;
	}

	return true;
}

void SocketUtils::ShutdownSocket(SOCKET* sock, int32 option)
{
	::shutdown(*sock, option);
}

void SocketUtils::CloseSocket(SOCKET* sock)
{
	closesocket(*sock);
}

bool SocketUtils::SetSocketOption(SOCKET* sock, int32 level, int32 optname,  const void* val, int32 len)
{
	int32 result = ::setsockopt(*sock, level, optname, static_cast<const char*>(val), len);
	if (result == SOCKET_ERROR)
	{
		Logger::log_error("Setting socket option failed: {}", result);
		return false;
	}

	return true;
}


