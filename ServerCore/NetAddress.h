#pragma once

/*--------------------------------
			NetAddress
	: Network address data class 
----------------------------------*/

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(uint32 ip, uint16 port) : ip(ip), port(port)
	{
		SetSocketAddr();
	}

public:
	void			SetSocketAddr()
	{
		mSocketAddr.sin_family = AF_INET;
		mSocketAddr.sin_addr.s_addr = htonl(ip);
		mSocketAddr.sin_port = htons(port);
	}

	SOCKADDR_IN&	GetSocketAddr() { return mSocketAddr; }

public:
	uint32 ip;
	uint16 port;

private:
	SOCKADDR_IN mSocketAddr = {};
};
