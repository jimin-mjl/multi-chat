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
	bool	Register(SOCKET sock);
	bool	Dispatch(uint32 timeout = INFINITE);

private:
	HANDLE	mCpObject = INVALID_HANDLE_VALUE;
};
