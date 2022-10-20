#include "pch.h"
#include "NetAddress.h"
#include "SocketUtils.h"

/*-----------------
	SocketUtils
------------------*/

LPFN_CONNECTEX		SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX	SocketUtils::DisconnectEx = nullptr;

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
	/* 런타임에 함수 포인터를 메모리에 가져온다 */
	LPVOID* connectFn = reinterpret_cast<LPVOID*>(&ConnectEx);
	LPVOID* disconnectFn = reinterpret_cast<LPVOID*>(&DisconnectEx);
	SOCKET dummySocket = CreateSocket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	bool isConnectSucced = GetExFuncPtr(&dummySocket, WSAID_CONNECTEX, connectFn);
	bool isDisconnectSucced = GetExFuncPtr(&dummySocket, WSAID_DISCONNECTEX, disconnectFn);

	CloseSocket(&dummySocket);
	
	if (isConnectSucced == false || isDisconnectSucced == false)
	{
		Logger::log_error("Load Ex function failed: {}", WSAGetLastError());
		CleanupWS();
		return false;
	}

	return true;
}

bool SocketUtils::GetExFuncPtr(SOCKET* sock, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;
	int32 result = ::WSAIoctl(*sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), &bytes, NULL, NULL);
	return result == 0;
}

SOCKET SocketUtils::CreateSocket(int32 af, int32 type, int32 protocol)
{
	// non-blocking socket 생성
	u_long mode = 1;
	SOCKET sock = ::WSASocket(af, type, protocol, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (sock == INVALID_SOCKET)
	{
		Logger::log_error("Creating socket failed: {}", WSAGetLastError());
		return INVALID_SOCKET;
	}
	
	::ioctlsocket(sock, FIONBIO, &mode);  // IO mode non-blocking
	return sock;
}

bool SocketUtils::BindSocket(SOCKET* sock, SOCKADDR_IN& addr)
{
	int32 result = ::bind(*sock, (sockaddr*)&addr, sizeof(addr));
	if (result == SOCKET_ERROR)
	{
		Logger::log_error("Binding socket failed: {}", ::WSAGetLastError());
		return false;
	}

	return true;
}

bool SocketUtils::ListenSocket(SOCKET* sock, int32 backlog)
{
	int32 result = ::listen(*sock, backlog);
	if (result == SOCKET_ERROR)
	{
		Logger::log_error("Listening socket failed: {}", ::WSAGetLastError());
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
		Logger::log_error("Setting socket option failed: {}", ::WSAGetLastError());
		return false;
	}

	return true;
}
