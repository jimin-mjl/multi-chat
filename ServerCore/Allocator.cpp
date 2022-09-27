#include "pch.h"
#include "Allocator.h"

/*------------------
	CountAllocator
-------------------*/

int64 CountAllocator::allocCnt = 0;
int64 CountAllocator::releaseCnt = 0;

void* CountAllocator::Alloc(size_t size)
{
	void* obj = ::malloc(size);
	CountAllocator::allocCnt++;
	return obj;
}

void CountAllocator::Release(void* obj)
{
	::free(obj);
	CountAllocator::releaseCnt++;
}

void CountAllocator::Debug()
{
	Logger::log_debug("������ ��ü ��: {}", CountAllocator::allocCnt);
	Logger::log_debug("������ ��ü ��: {}", CountAllocator::releaseCnt);
}
