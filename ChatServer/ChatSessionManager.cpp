#include "pch.h"
#include "ChatSession.h"
#include "ChatSessionManager.h"

/*-----------------------
	ChatSessionManager
------------------------*/

ChatSessionManager* GSessionManager = nullptr;

void ChatSessionManager::AddSession(shared_ptr<ChatSession> session)
{
	lock_guard<mutex> writeLock(mLock);
	mSessions.insert(session);
	mSessionCount.fetch_add(1);
}

void ChatSessionManager::RemoveSession(shared_ptr<ChatSession> session)
{
	lock_guard<mutex> writeLock(mLock);
	mSessions.erase(session);
	mSessionCount.fetch_sub(1);
}

void ChatSessionManager::Broadcast(shared_ptr<CircularBuffer> buffer)
{
	lock_guard<mutex> writeLock(mLock);
	for (shared_ptr<ChatSession> session : mSessions)
	{
		session->Send(buffer);
	}
}
