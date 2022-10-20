#include "pch.h"
#include "ChatSession.h"
#include "ChatSessionManager.h"

/*-----------------------
	ChatSessionManager
------------------------*/

void ChatSessionManager::AddSession(shared_ptr<ChatSession> session)
{
	lock_guard<mutex> writeLock(mLock);
	mSessions.insert(session);
}

void ChatSessionManager::RemoveSession(shared_ptr<ChatSession> session)
{
	lock_guard<mutex> writeLock(mLock);
	mSessions.erase(session);
}

void ChatSessionManager::Broadcast(shared_ptr<CircularBuffer> buffer)
{
	lock_guard<mutex> writeLock(mLock);
	for (shared_ptr<ChatSession> session : mSessions)
	{
		session->Send(buffer);
	}
}
