#pragma once

/*------------------------------------------------
					IocpCore
	: Completion Port management class responsible 
	for registering jobs to CP object
	and dispatching them to each IOCP Object
--------------------------------------------------*/

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

public:
	bool	Register(shared_ptr<class IocpObject> obj);
	bool	Dispatch(uint32 timeout);

private:
	HANDLE	mCpObject = INVALID_HANDLE_VALUE;
};
