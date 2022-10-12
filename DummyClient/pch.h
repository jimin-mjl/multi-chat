#pragma once

// 라이브러리 파일 포함 설정
#ifdef _DEBUG
#pragma comment(lib, "Libs\\ServerCore\\Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Libs\\ServerCore\\Release\\ServerCore.lib")
#endif 

#include "CorePch.h"
