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

template<typename T, typename... Args>
T* xnew(Args&&... args)
{
	T* obj = static_cast<T*>(CountAllocator::Alloc(sizeof(T)));
	new(obj)T(std::forward<Args>(args)...);  // Initialize obj after allocation (placement new) 
	return obj;
}

template<typename T>
void xdelete(T* obj)
{
	obj->~T();
	CountAllocator::Release(obj);
} 

//template<typename Type, typename... Args>
//shared_ptr<Type> xmake_shared(Args&&... args)
//{
//	return shared_ptr<Type>{ xnew<T>(std::forward<Args>(args)...), xdelete<Type> };
//}
