#include "pch.h"
#include "ChatSession.h"
#include "ChatSessionManager.h"
#include "CircularBuffer.h"
#include "PacketHeader.h"

/*-----------------
	ChatSession
------------------*/

void ChatSession::OnConnect()
{
	cout << "Client Connected" << endl;
	GSessionManager->AddSession(static_pointer_cast<ChatSession>(shared_from_this()));
}

void ChatSession::OnDisconnect()
{
	cout << "Client Disconnected" << endl;
	GSessionManager->RemoveSession(static_pointer_cast<ChatSession>(shared_from_this()));
}

int32 ChatSession::OnRecv(char* buffer, int32 recvBytes)
{
	cout << "recv bytes: " << recvBytes << endl;

	// Broadcast
	shared_ptr<CircularBuffer> sendBuffer = CreateSendBuffer(buffer, recvBytes);
	/*if (sendBuffer)
		GSessionManager->Broadcast(sendBuffer);
	*/
	Send(sendBuffer);
	return recvBytes;
}

void ChatSession::OnSend(int32 sendBytes)
{
	cout << "Msg sent len: " << sendBytes << endl;
}
