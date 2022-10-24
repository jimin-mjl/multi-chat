#pragma once

class ChatSession;
class CircularBuffer;

/*-------------------------------------------------
					ChatSessionManager
	: 컨텐츠 레벨에서 주고 받은 패킷 처리 담당
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
