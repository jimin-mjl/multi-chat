#pragma once

// 라이브러리 파일 포함 설정
#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif 

// 여기에 미리 컴파일하려는 헤더 추가
#include "CorePch.h"
