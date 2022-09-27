#pragma once

/*-----------------
    SocketUtils
------------------*/

class SocketUtils
{
public:
	bool InitializeWS();
	void CleanupWS();
	SOCKET CreateSocket(int32 af, int32 type, int32 protocol);
	bool BindSocket(SOCKET* sock, int32 af, uint16 port, uint32 ip = INADDR_ANY);
	void ShutdownSocket(SOCKET* sock, int option);
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
