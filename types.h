#pragma once
#include <cstdint>

using u8 = uint8_t;
using i8 = int8_t;

using u16 = uint16_t;
using i16 = int16_t;

using u32 = uint32_t;
using i32 = int32_t;

using u64 = uint64_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

// TODO: Should this be placed in another file?
static_assert(sizeof(u8) == 1);
static_assert(sizeof(i8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(i64) == 8);
static_assert(sizeof(f32) == 4);
static_assert(sizeof(f64) == 8);

/* Support for packed structs */
#if defined(_MSC_VER)
    #define PACKED_STRUCT_BEGIN __pragma(pack(push, 1))
    #define PACKED_STRUCT_END __pragma(pack(pop))
#elif defined(__GNUC__) || defined(__clang__)
    #define PACKED_STRUCT_BEGIN
    #define PACKED_STRUCT_END __attribute__((packed))
#else
    #define PACKED_STRUCT_BEGIN
    #define PACKED_STRUCT_END
#endif

#define ARRAY_ELEMENTS(X)  (sizeof(X) / sizeof((X)[0]))

