#pragma once

class ChatSession;
class CircularBuffer;

/*-------------------------------------------------
					ChatSessionManager
	: ������ �������� �ְ� ���� ��Ŷ ó�� ���
--------------------------------------------------*/

class ChatSessionManager
{
public:
	void AddSession(shared_ptr<ChatSession> session);
	void RemoveSession(shared_ptr<ChatSession> session);
	void Broadcast(shared_ptr<CircularBuffer> buffer);

private:
	mutex							mLock;
	set<shared_ptr<ChatSession>>	mSessions;
};

extern ChatSessionManager* GSessionManager;
