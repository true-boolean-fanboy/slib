#ifdef __APPLE__

#include <mach/mach_time.h>
#include <slib/types.h>

// Use uint64_t for portability across macOS
uint64_t get_os_timer_freq()
{
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);

    // Convert nanoseconds to frequency (Hz)
    return (uint64_t)((1e9 * timebase.denom) / timebase.numer);
}

uint64_t read_os_timer()
{
    return mach_absolute_time();
}

inline uint64_t read_cpu_timer()
{
    uint32_t high, low;

    // Use assembly for the `rdtsc` equivalent
    __asm__ volatile(
        "rdtsc" : "=a"(low), "=d"(high));

    return ((uint64_t)high << 32) | low;
}

#endif
