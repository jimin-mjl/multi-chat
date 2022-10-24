#pragma once
#include "IocpEvent.h"
#include "IocpHandler.h"
#include "NetAddress.h"

class Service;
class CircularBuffer;

constexpr int INPUT_BUF_SIZE = 1024;

/*------------------------------------
				Session
	: IOCP object class responsible
	for storing client data
	and processing IOCP events
-------------------------------------*/

class Session : public IocpHandler
{
	enum
	{
		RECV_BUFFER_SIZE = 0x10000,  // 65536
		SEND_BUFFER_SIZE = 0x1000  // 4096
	};

public:
	Session();
	~Session();

	void							SetSocket(SOCKET sock) { mSock = sock; }
	void							SetNetAddress(SOCKADDR_IN addr) { mAddr = NetAddress(addr); }
	void							SetService(shared_ptr<Service> service) { mService = service; }
	SOCKET							GetSocket() { return mSock; }
	shared_ptr<Service>				GetService();
	bool							IsConnected() { return mIsConnected.load(); }
	
	bool							Connect();
	void							Disconnect();
	bool							Send(shared_ptr<CircularBuffer> buffer);
	shared_ptr<CircularBuffer>		CreateSendBuffer(const char* msg, int32 len);

	/* IocpHandler Interface methods */
	virtual HANDLE		GetHandle() override { return reinterpret_cast<HANDLE>(mSock); }
	virtual bool		IsHandleValid() override { return mSock != INVALID_SOCKET; };
	virtual void		Dispatch(IocpEvent* event, int32 transferredBytes) override;

	/* Methods for user level implementation */
	virtual void		OnConnect() {}
	virtual void		OnDisconnect() {}
	virtual int32		OnRecv(char* buffer, int32 recvBytes) { return recvBytes; }
	virtual void		OnSend(int32 sendBytes) {}

private:
	/* Event registration methods */
	bool				registerConnect();
	void				registerDisconnect();
	void				registerRecv();
	void				registerSend();

	/* Event handler methods */
	void				processAccept(AcceptEvent* event);
	void				processConnect();
	void				processDisconnect();
	void				processRecv(int32 recvBytes);
	void				processSend(int32 sendBytes);

	void				handleError(const char* reason, int32 error = SOCKET_ERROR);

private:
	SOCKET									mSock = INVALID_SOCKET;  // client socket handle
	NetAddress								mAddr = {};
	atomic<bool>							mIsConnected = false;
	weak_ptr<Service>						mService;

	/* IO buffers */
	mutex									mSendLock;
	atomic<bool>							mIsSendOccupied = false;
	queue<shared_ptr<CircularBuffer>>		mSendQueue;
	shared_ptr<CircularBuffer>				mRecvBuffer = nullptr;

	/* reusable event objects */
	ConnectEvent							mConnectEvent = {};
	RecvEvent								mRecvEvent = {};
	SendEvent								mSendEvent = {};
	DisconnectEvent							mDisconnectEvent = {};
};
