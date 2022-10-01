#pragma once

class IocpEvent;

/*--------------------------------
			IocpObject

	: Abstract base class 
	for IOCP process interface
---------------------------------*/

class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE			GetHandle() abstract;
	virtual void			Dispatch(IocpEvent* event, int32 bytes) abstract;
	virtual bool			IsHandleValid() abstract;

protected:
	shared_ptr<IocpObject>	getSelfRef() { return shared_from_this(); }
};
