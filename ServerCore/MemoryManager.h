#pragma once

#include "pch.h"
#include "Allocator.h"

template<typename T, typename... Args>
T* xnew(Args&&... args)
{
	T* obj = static_cast<T*>(CountAllocator::Alloc(sizeof(T)));
	new(obj)T(forward<Args>(args)...);  // 메모리 할당 후 초기화(placement new) 
	return obj;
}

template <typename T>
void xdelete(T* obj)
{
	obj->~T();
	CountAllocator::Release(obj);
} 

template<typename T, typename... Args>
shared_ptr<T> xmake_shared(Args... args)
{
	return shared_ptr<T>{ xnew<T>(args...), xdelete<T> };
}
