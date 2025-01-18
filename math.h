#pragma once

#include <slib/types.h>

#undef max
#undef min

inline i32 max(i32 a, i32 b)
{
    i32 diff = a - b;

    i32 sign = ((diff >> (sizeof(i32) * 8 - 1)) & 1);

    return a - sign * diff;
}

inline i32 min(i32 a, i32 b)
{
    i32 diff = a - b;

    i32 sign = ((diff >> (sizeof(i32) * 8 - 1)) & 1);

    return b + sign * diff;
}
