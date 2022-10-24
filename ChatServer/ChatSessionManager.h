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
	int32 GetSessionCount() { return mSessionCount.load(); }

private:
	mutex							mLock;
	atomic<int32>					mSessionCount = 0;
	set<shared_ptr<ChatSession>>	mSessions;
};

extern ChatSessionManager* GSessionManager;
