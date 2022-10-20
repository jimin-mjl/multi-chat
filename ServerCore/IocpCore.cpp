#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "IocpHandler.h"

/*--------------
	IocpCore
--------------*/

IocpCore::IocpCore()
{
	mCpObject = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	ASSERT_CRASH(mCpObject != NULL);
}

IocpCore::~IocpCore()
{
	if (mCpObject != INVALID_HANDLE_VALUE)
		::CloseHandle(mCpObject);
}

bool IocpCore::Register(SOCKET sock)
{
	return ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(sock), mCpObject, 0, 0) != NULL;
}

/* Entrance function for threads */
bool IocpCore::Dispatch(uint32 timeout)
{
	DWORD recvBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	int result = ::GetQueuedCompletionStatus(mCpObject, &recvBytes, &key, reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeout);
	if (result == false)
	{
		int32 error = ::WSAGetLastError();
		if (error == ERROR_NETNAME_DELETED)  // Ŭ���̾�Ʈ ������ ����
		{
			Logger::log_info("Client closed unexpectedly");
		}
		else
		{
			Logger::log_error("Getting finished IOCP job failed: {}", error);
			return false;
		}
	}

	shared_ptr<IocpHandler> iocpHandler = iocpEvent->GetOwner();
	ASSERT_CRASH(iocpHandler != nullptr);
	iocpHandler->Dispatch(iocpEvent, recvBytes);

	return true;
}
