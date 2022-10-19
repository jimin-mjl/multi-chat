#pragma once

/*--------------------
	Circular Buffer
---------------------*/

class CircularBuffer : public enable_shared_from_this<CircularBuffer>
{
	enum
	{
		DEFAULT_STOCK_SIZE = 10
	};

public:
	CircularBuffer(int32 size);
	CircularBuffer(int32 size, int32 stockSize);
	virtual ~CircularBuffer();
	int32 Size();
	int32 DataSize();
	char* WritePos() { return &mBuffer[mWritePos]; }  // 쓰기 시작 위치
	char* ReadPos() { return &mBuffer[mReadPos]; }  // 읽기 시작 위치
	bool OnRead(int32 bytes);
	bool OnWrite(int32 bytes);

private:
	int32 freeSize();
	void clear();

private:
	int32 mSize = 0;
	int32 mCapacity = 0;
	int32 mWritePos = 0;
	int32 mReadPos = 0;
	vector<char> mBuffer;
};
