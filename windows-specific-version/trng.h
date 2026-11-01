#pragma once
#ifndef TRNG_H
#define TRNG_H

#include <windows.h>
#include <bcrypt.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#pragma comment(lib, "bcrypt.lib") // Links bcrypt automatically in MSVC

// Generate a random 32-bit unsigned int
static inline uint32_t trng_u32() {
    uint32_t value;
    if (BCryptGenRandom(NULL, (PUCHAR)&value, sizeof(value), BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
        fprintf(stderr, "BCryptGenRandom failed\n");
        exit(EXIT_FAILURE);
    }
    return value;
}

//Generate a random number between to values
static inline int trng_range(int min, int max) {
    if (min > max) { int temp = min; min = max; max = temp; }
    return (int)(trng_u32() % (max - min + 1)) + min;
}

// Guassian
static inline float trng_gaussian(float mean, float stddev) {
    float u1 = trng_float01();
    float u2 = trng_float01();
    float z0 = sqrtf(-2.0f * logf(u1)) * cosf(2.0f * 3.14159265f * u2);
    return z0 * stddev + mean;
}

//64 bit number
static inline uint64_t trng_u64() {
    uint64_t value;
    trng_fill(&value, sizeof(value));
    return value;
}

// Generate a float in range [0.0, 1.0]
static inline float trng_float01() {
    return (float)trng_u32() / (float)UINT32_MAX;
}

// Generate a float in range [-1.0, 1.0]
static inline float trng_float_signed() {
    return trng_float01() * 2.0f - 1.0f;
}
/*
// Fill a buffer with true random bytes
static inline void trng_fill(void* buffer, size_t length) {
    if (BCryptGenRandom(NULL, (PUCHAR)buffer, (ULONG)length, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
        fprintf(stderr, "BCryptGenRandom (buffer) failed\n");
        exit(EXIT_FAILURE);
    }
}*/

#endif // TRNG_H