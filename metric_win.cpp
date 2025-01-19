#ifdef _WIN32

#include <intrin.h>
#include <windows.h>
#include <slib/types.h>

u64
get_os_timer_freq()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
}

u64
read_os_timer()
{
    LARGE_INTEGER value;
    QueryPerformanceCounter(&value);
    return value.QuadPart;
}

inline u64
read_cpu_timer()
{
    return __rdtsc();
}

#endif
