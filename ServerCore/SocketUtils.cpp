#include "pch.h"
#include "NetAddress.h"
#include "SocketUtils.h"

/*-----------------
	SocketUtils
------------------*/

LPFN_CONNECTEX SocketUtils::ConnectEx = nullptr;

bool SocketUtils::InitializeWS()
{
	WSADATA wsaData;

	int32 result = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		Logger::log_error("WSAStartup failed: {}", WSAGetLastError());
		return false;
	}

	if (InitializeMsws() == false)
		return false;

	return true;
}

void SocketUtils::CleanupWS()
{
	::WSACleanup();
}

bool SocketUtils::InitializeMsws()
{
	/* Load the ConnectEx function into memory using WSAIoctl at runtime */
	GUID guidConnectEx = WSAID_CONNECTEX;
	LPVOID* fn = reinterpret_cast<LPVOID*>(&ConnectEx);
	DWORD bytes = 0;
	SOCKET dummySocket = CreateSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	int32 result = ::WSAIoctl(dummySocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx), fn, sizeof(ConnectEx), &bytes, NULL, NULL);
	if (result == false)
	{
		Logger::log_error("Load ConnectEx fn failed: {}", WSAGetLastError());
		CloseSocket(&dummySocket);
		CleanupWS();
		return false;
	}

	CloseSocket(&dummySocket);
	return true;
}

SOCKET SocketUtils::CreateSocket(int32 af, int32 type, int32 protocol)
{
	// creates non-blocking socket
	u_long mode = 1;
	SOCKET sock = ::WSASocket(af, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sock == INVALID_SOCKET)
	{
		int error = WSAGetLastError();
		Logger::log_error("Creating socket failed: {}", error);
		return INVALID_SOCKET;
	}

	::ioctlsocket(sock, FIONBIO, &mode);
	return sock;
}

bool SocketUtils::BindSocket(SOCKET* sock, int32 af, const NetAddress& netaddr)
{
	SOCKADDR_IN addr = SOCKADDR_IN();
	addr.sin_family = af;  
	addr.sin_addr.s_addr = htonl(netaddr.ip);
	addr.sin_port = htons(netaddr.port);

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

bool SocketUtils::ConnectSocket(SOCKET* sock, SOCKADDR_IN& addr)
{
	int32 result = ::connect(*sock, (sockaddr*)&addr, sizeof(addr));
	if (result == SOCKET_ERROR)
	{
		if (result != WSAEWOULDBLOCK)
		{
			Logger::log_error("Connecting socket failed: {}", WSAGetLastError());
			return false;
		}
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
		Logger::log_error("Setting socket option failed: {}", optname);
		return false;
	}

	return true;
}
