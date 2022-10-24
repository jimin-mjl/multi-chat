#pragma once

/*-------------------
	PacketHeader
--------------------*/

class PacketHeader
{
public:
	uint16 contentType = 0;
	uint16 contentLength = 0;
};
