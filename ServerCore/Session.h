#pragma once
#include "IocpEvent.h"
#include "IocpHandler.h"
#include "NetAddress.h"

class Service;
class CircularBuffer;

/*------------------------------------
				Session
	: IOCP object class responsible 
	for storing client data
	and processing IOCP events
-------------------------------------*/

constexpr int INPUT_BUF_SIZE = 1024;

class Session : public IocpHandler
{
	enum
	{
		BUFFER_SIZE = 65536  // 0x10000
	};

public:
	Session();
	~Session();

	void				SetSocket(SOCKET sock);
	void				SetSessionId(uint64 sid);
	uint64				GetSessionId();
	void				SetNetAddress(SOCKADDR_IN addr);
	void				SetService(shared_ptr<Service> service) { mService = service; }
	SOCKET				GetSocket() { return mSock; }
	shared_ptr<Service> GetService();
	bool				IsConnected() { return mIsConnected; }
	
	bool				Connect();
	bool				Send(const char* msg);
	void				Disconnect();

public:
	/* Methods for user level implementation */
	virtual void	OnConnect() {}
	virtual void	OnDisconnect() {}
	virtual int32	OnRecv(char* buffer, int32 recvBytes) { return recvBytes; }
	virtual void	OnSend(int32 sendBytes) {}

public:
	/* IocpHandler Interface methods */
	virtual HANDLE	GetHandle() override;
	virtual bool	IsHandleValid() override;
	virtual void	Dispatch(IocpEvent* event, int32 transferredBytes) override;

private:
	/* Event registration methods */
	bool registerConnect();
	void registerRecv();
	bool registerSend();

	/* Event handler methods */
	void processAccept();
	void processConnect();
	void processRecv(int32 recvBytes);
	void processSend(int32 sendBytes);

public:
	char			mSendBuf[INPUT_BUF_SIZE] = {};

private:
	shared_ptr<CircularBuffer> mRecvBuffer = nullptr;

private:
	atomic<uint64>	mSessionId = 0;
	SOCKET			mSock = INVALID_SOCKET;  // client socket handle
	NetAddress		mAddr = {};
	atomic<bool>	mIsConnected = false;

private:
	weak_ptr<Service>	mService;

private:
	/* reusable event objects */
	ConnectEvent		mConnectEvent = {};
	RecvEvent			mRecvEvent = {};
	SendEvent			mSendEvent = {};
};
