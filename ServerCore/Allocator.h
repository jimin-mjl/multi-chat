#pragma once

/*------------------
	CountAllocator
-------------------*/

class CountAllocator 
{
public:
	static void* Alloc(size_t size);
	static void Release(void* obj);
	static void Debug();

public:
	static int64 allocCnt;
	static int64 releaseCnt;
};
