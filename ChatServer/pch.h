#pragma once

// include library files
#ifdef _DEBUG
#pragma comment(lib, "Libs\\ServerCore\\Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Libs\\ServerCore\\Release\\ServerCore.lib")
#endif 

// Standard libraries
#include <thread>

#include "CorePch.h"
