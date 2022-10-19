#include "pch.h"
#include "CircularBuffer.h"

/*--------------------
	Circular Buffer
---------------------*/

CircularBuffer::CircularBuffer(int32 size)
	: mSize(size)
{
	mCapacity = size * DEFAULT_STOCK_SIZE;
	mBuffer.resize(mCapacity);
}

CircularBuffer::CircularBuffer(int32 size, int32 stockSize)
	: mSize(size)
{
	mCapacity = size * stockSize;
	mBuffer.resize(mCapacity);
}

CircularBuffer::~CircularBuffer()
{
}

int32 CircularBuffer::Size()
{
	return mSize;
}

int32 CircularBuffer::DataSize()
{
	return mWritePos - mReadPos;
}

bool CircularBuffer::OnRead(int32 bytes)
{
	if (bytes > DataSize())
		return false;

	mReadPos += bytes;
	clear();  // 읽고 난 후 포인터 정리 
	return true;
}

bool CircularBuffer::OnWrite(int32 bytes)
{
	if (bytes > freeSize())
		return false;

	mWritePos += bytes;
	return true;
}

int32 CircularBuffer::freeSize()
{
	return mCapacity - mWritePos;
}

void CircularBuffer::clear() 
{
	/* read, write 포인터를 정리하는 메서드.
	데이터 오버플로우 가능성이 있는 경우, read, write 포인터를 앞으로 당긴다. */

	int32 dataSize = DataSize();

	if (freeSize() < mSize)
	{
		if (dataSize == 0)
		{
			mReadPos = mWritePos = 0;
		}
		else  // 처리할 데이터가 남아있으면 앞부분에 복사한다. 
		{
			::memcpy(&mBuffer[0], &mBuffer[mReadPos], dataSize);
			mReadPos = 0;
			mWritePos = dataSize;
		}
	}
}
