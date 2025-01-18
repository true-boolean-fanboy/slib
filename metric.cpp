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

u64 get_cpu_freq()
{
    u64 ms_to_wait = 100;

    u64 os_freq = get_os_timer_freq();

    u64 cpu_start = read_cpu_timer();
    u64 os_start = read_os_timer();
    u64 os_end = 0;
    u64 os_elapsed = 0;
    u64 os_wait_time = os_freq * ms_to_wait / 1000;

    while(os_elapsed < os_wait_time)
    {
        os_end = read_os_timer();
        os_elapsed = os_end - os_start;
    }

    u64 cpu_end = read_cpu_timer();
    u64 cpu_elapsed = cpu_end - cpu_start;
    u64 cpu_freq = 0;

    if(os_elapsed)
    {
        cpu_freq = os_freq * cpu_elapsed / os_elapsed;
    }

    return cpu_freq;
}

