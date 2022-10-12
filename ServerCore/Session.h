#pragma once
#include "IocpEvent.h"
#include "IocpObject.h"
#include "NetAddress.h"

class Service;

/*------------------------------------
				Session
	: IOCP object class responsible 
	for storing client data
	and processing IOCP events
-------------------------------------*/

constexpr int OUTPUT_BUF_SIZE = 1024;
constexpr int INPUT_BUF_SIZE = 1024;

class Session : public IocpObject
{
public:
	Session() = default;
	~Session();

public:
	void				SetSocket(SOCKET sock);
	void				SetSessionId(uint64 sid);
	uint64				GetSessionId();
	void				SetNetAddress(SOCKADDR_IN addr);
	void				SetService(shared_ptr<Service> service) { mService = service; }
	SOCKET				GetSocket() { return mSock; }
	shared_ptr<Service> GetService();
	bool				IsConnected() { return mIsConnected; }
	
public:
	bool Connect();
	bool Send(const char* msg);
	void Disconnect();

public:
	/* Methods for user implementation */
	virtual void	OnConnect() {}
	virtual void	OnDisconnect() {}
	virtual void	OnRecv(char* buffer, int32 recvBytes) {}
	virtual void	OnSend(int32 sendBytes) {}

public:
	/* IocpObject Interface methods */
	virtual HANDLE	GetHandle() override;
	virtual bool	IsHandleValid() override;
	virtual void	Dispatch(IocpEvent* event, int32 transferredBytes) override;

public:
	/* Event handler methods */
	void ProcessAccept();
	void ProcessConnect();
	void ProcessRecv(int32 recvBytes);
	void ProcessSend(int32 sendBytes);

private:
	/* Event registration methods */
	bool registerConnect();
	void registerRecv();
	bool registerSend();

public:
	char			mRecvBuf[OUTPUT_BUF_SIZE] = {};
	char			mSendBuf[INPUT_BUF_SIZE] = {};

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
