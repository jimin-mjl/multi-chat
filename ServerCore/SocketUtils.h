#pragma once

/*-----------------
    SocketUtils
------------------*/

class SocketUtils
{
public:
	bool InitializeWS();
	bool CleanupWS();
	SOCKET CreateSocket(int32 af, int32 type, int32 protocol);
	bool BindSocket(SOCKET* sock, int32 af, int16 port, int64 ip = INADDR_ANY);
	bool ShutdownSocket(SOCKET* sock, int option);
};

/*-----------------
      Listener
------------------*/

class Listener
{
public:
	Listener() = default;
	Listener(SOCKET* sock);
	bool Listen(int32 backlog = SOMAXCONN);

private:
	SOCKET* mSock = nullptr;
};
