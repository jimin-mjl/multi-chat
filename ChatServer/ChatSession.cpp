#include "pch.h"
#include "ChatSession.h"

/*-----------------
	ChatSession
------------------*/

void ChatSession::OnConnect()
{
	cout << "Client Connected" << endl;
}

void ChatSession::OnDisconnect()
{
	cout << "Client Disconnected" << endl;
}

int32 ChatSession::OnRecv(char* buffer, int32 recvBytes)
{
	// Broadcast
	cout << "recv bytes: " << recvBytes << endl;
	cout << "recv content: " << buffer << endl;

	Send("hi from server");
	return recvBytes;
}

void ChatSession::OnSend(int32 sendBytes)
{
	cout << "Msg sent len: " << sendBytes << endl;
}
