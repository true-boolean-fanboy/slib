#pragma once

#ifndef PROFILE_ENABLE
#define PROFILE_ENABLE 1
#endif

#if PROFILE_ENABLE

#include <slib/types.h>
#include <slib/metric.h>

#define CONCAT(A, B) A##B
#define NAMED_CONCAT(A, B) CONCAT(A, B)
#define UNIQUE NAMED_CONCAT(u_var_, __LINE__)

#define PROFILE_BANDWIDTH(NAME, BYTES) \
    profile_measure_scope_t UNIQUE (NAME, __COUNTER__ + 1, (BYTES));

#define PROFILE_FUNCTION\
    PROFILE_BLOCK(__FUNCTION__);

#define PROFILE_BLOCK(NAME)\
    PROFILE_BANDWIDTH(NAME, 0);

struct profile_measure_point_t
{
    const char* name = nullptr;
    u64 total_time = 0;
    u64 exclusive_time = 0;
    u64 calls = 0;
    u64 processed_bytes = 0;
};

struct profile_measure_scope_t
{
    profile_measure_scope_t() = default;
    profile_measure_scope_t(const char* name, u32 idx, u64 byte_count = 0);
    ~profile_measure_scope_t();

    u32 id;
    u32 parent;
    u64 start;
    u64 old_total_time;
    const char* name;
};

void profiling_begin();
void profiling_end();

#else

#define PROFILE_BANDWIDTH(...)
#define PROFILE_BLOCK(...)
#define PROFILE_FUNCTION
#define profiling_begin(...)
#define profiling_end(...)

#endif /* PROFILE_ENABLE */
