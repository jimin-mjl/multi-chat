#pragma once

class IocpEvent;

/*--------------------------------
			IocpHandler

	: Abstract base class 
	for IOCP process interface
---------------------------------*/

class IocpHandler : public enable_shared_from_this<IocpHandler>
{
public:
	virtual HANDLE			GetHandle() abstract;
	virtual void			Dispatch(IocpEvent* event, int32 bytes) abstract;
	virtual bool			IsHandleValid() abstract;
};
