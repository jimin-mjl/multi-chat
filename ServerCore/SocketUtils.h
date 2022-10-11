#pragma once

/*-------------------------------------
			 SocketUtils
	: Winsock functions wrapping class 
---------------------------------------*/

class SocketUtils
{
public:
	static LPFN_CONNECTEX		ConnectEx;  // ConnectEx function pointer

public:
	static bool		InitializeWS();
	static void		CleanupWS();
	static bool		InitializeMsws();
	static SOCKET	CreateSocket(int32 af, int32 type, int32 protocol);
	static bool		BindSocket(SOCKET* sock, SOCKADDR_IN& addr);
	static bool		ListenSocket(SOCKET* sock, int32 backlog = SOMAXCONN);
	static bool		ConnectSocket(SOCKET* sock, SOCKADDR_IN& addr);
	static void		ShutdownSocket(SOCKET* sock, int32 option);
	static void		CloseSocket(SOCKET* sock);
	static bool		SetSocketOption(SOCKET* sock, int32 level, int32 optname, const void* val, int32 len);
};
