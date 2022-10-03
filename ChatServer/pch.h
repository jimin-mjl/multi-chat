#pragma once

// include library files
#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif 

// Standard libraries
#include <thread>

#include "CorePch.h"
