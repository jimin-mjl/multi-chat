#include "pch.h"
#include "MemoryManager.h"

void Hello()
{
	spdlog::set_level(spdlog::level::debug);
	CountAllocator::Debug();
	shared_ptr<int> one = xmake_shared<int>();
	*one = 1;
	CountAllocator::Debug();
	Logger::log_info("Hello: {}", *one);
	one = nullptr;
	CountAllocator::Debug();
}
