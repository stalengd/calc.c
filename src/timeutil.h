#pragma once

#include <time.h>
#ifdef _WIN32
#include <windows.h>
#endif

typedef struct {
	long long sec;
	unsigned int nsec;
} datetime;

datetime time_now();