#pragma once

#include <slib/types.h>

u64 get_os_timer_freq();
u64 read_os_timer();
inline u64 read_cpu_timer();
u64 get_cpu_freq();
