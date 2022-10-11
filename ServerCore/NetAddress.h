#pragma once

/*--------------------------------
			NetAddress
	: Network address data class 
----------------------------------*/

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(const char* ip, uint16 port)
	{
		SetSocketAddr(ip, port);
	}
	NetAddress(uint16 port)
	{
		SetAnySocketAddr(port);
	}
	NetAddress(SOCKADDR_IN addr) : mSocketAddr(addr)
	{
	}

public:
	void			SetSocketAddr(const char* ip, uint16 port)
	{
		::memset(&mSocketAddr, 0,sizeof(mSocketAddr));
		mSocketAddr.sin_family = AF_INET;
		inet_pton(AF_INET, ip, &mSocketAddr.sin_addr.s_addr);
		mSocketAddr.sin_port = ::htons(port);
	}
	void			SetAnySocketAddr(uint16 port)
	{
		::memset(&mSocketAddr, 0, sizeof(mSocketAddr));
		mSocketAddr.sin_family = AF_INET;
		mSocketAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
		mSocketAddr.sin_port = ::htons(port);
	}

	SOCKADDR_IN&	GetSocketAddr() { return mSocketAddr; }

private:
	SOCKADDR_IN mSocketAddr = {};
};
