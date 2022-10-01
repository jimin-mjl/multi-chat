#pragma once

/*--------------------------------
			NetAddress
	: Network address data class 
----------------------------------*/

class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(uint32 ip, uint16 port) : ip(ip), port(port) {};

public:
	uint32 ip;
	uint16 port;
};

