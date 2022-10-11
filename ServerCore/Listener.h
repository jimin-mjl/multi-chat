#pragma once
#include "IocpObject.h"

class NetAddress;
class IocpEvent;
class AcceptEvent;
class ServerService;

/*--------------------------------------
				Listener
	: IOCP object class responsible 
	for accepting connections 
	and registering sessions
---------------------------------------*/

class Listener : public IocpObject
{
public:
	Listener() = default;
	Listener(int limit);
	~Listener();

public:
	void				SetMaxConnection(int limit);
	void				SetService(shared_ptr<ServerService> service);
	int					GetMaxConnection();
	shared_ptr<ServerService> GetService();
	bool				StartAccept(shared_ptr<ServerService> service);

public:
	/* IocpObject Interface methods */
	virtual HANDLE	GetHandle() override;
	virtual bool	IsHandleValid() override;
	virtual void	Dispatch(IocpEvent* event, int32 bytes) override;

private:
	bool	createSocket();
	bool	setReuseAddr();
	bool	setUpdateAcceptContext(SOCKET sock);
	bool	bindSocket(NetAddress& addr);
	bool	listenSocket();

private:
	/* Event handler methods */
	void	registerAccept(AcceptEvent* event);
	void	processAccept(AcceptEvent* event);

private:
	SOCKET	mSock = INVALID_SOCKET;  // listening socket
	int		mMaxConn = 0;

private:
	weak_ptr<ServerService>		mService;
	vector<AcceptEvent*>	mAcceptEvents;
};
