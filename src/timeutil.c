#include "timeutil.h"

#ifdef _WIN32

#define TIME_100NS  116444736000000000ULL // 100-ns intervals within 1600..1970
#define TIME_1970_SECONDS  62135596800ULL

datetime datetime_from_filetime(FILETIME ft)
{
	datetime t = {0};
	unsigned long long i = ((unsigned long long)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
	if (i > TIME_100NS) {
		i -= TIME_100NS;
		t.sec = TIME_1970_SECONDS + i / (1000000 * 10);
		t.nsec = (i % (1000000 * 10)) * 100;
	}
	return t;
}

datetime time_now()
{
	FILETIME ft;
	GetSystemTimePreciseAsFileTime(&ft);
	return datetime_from_filetime(ft);
}

#else

datetime datetime_from_timespec(struct timespec ts)
{
	datetime t = {
		.sec = TIME_1970_SECONDS + ts.tv_sec,
		.nsec = (unsigned int)ts.tv_nsec,
	};
	return t;
}

/** Get UTC time */
datetime time_now()
{
	struct timespec ts = {};
	clock_gettime(CLOCK_REALTIME, &ts);
	return datetime_from_timespec(ts);
}

#endif