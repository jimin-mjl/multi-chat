#pragma once

// Standard libraries
#include <atomic>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

// Winsock libraries
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>  // for AcceptEx()
#pragma comment(lib,"ws2_32.lib")

// Local files
#include "MemoryManager.h"
#include "Types.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

// Macro for Crash
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)					\
{											\
	if (!(expr))							\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expr);			\
	}										\
}

using namespace std;

void Hello();
