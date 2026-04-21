#pragma once
/*
 * Generic prefix-aware custom mutator for Absolution-based Ledger app fuzzers.
 *
 * Splits fuzzer input into an Absolution prefix region and an APDU tail region,
 * applying different mutation strategies depending on the lane (raw vs structured).
 *
 * Required before inclusion:
 *
 *   FUZZ_PREFIX_SIZE_FALLBACK  - compile-time prefix size
 *   FUZZ_CTRL_OFF              - byte offset of the control header in prefix
 *   FUZZ_CTRL_LEN              - byte length of the control header
 *   fuzz_lane_is_structured(data, prefix_size) - nonzero for structured lane
 *
 * Optional:
 *
 *   FUZZ_APP_DATA_OFF / FUZZ_APP_DATA_LEN - app-defined structured data region
 *                                           inside the prefix (set both to 0 or
 *                                           leave undefined to skip)
 *   FUZZ_INJECT_TOKEN(region, region_len, seed) - app token injection callback
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef FUZZ_APP_DATA_OFF
#define FUZZ_APP_DATA_OFF 0
#endif
#ifndef FUZZ_APP_DATA_LEN
#define FUZZ_APP_DATA_LEN 0
#endif

extern size_t       LLVMFuzzerMutate(uint8_t *data, size_t size, size_t max_size);
extern const size_t absolution_globals_size __attribute__((weak));

static size_t fuzz_mutate_with_split(uint8_t *data, size_t size, size_t max_size, unsigned int seed)
{
    size_t prefix_size = FUZZ_PREFIX_SIZE_FALLBACK;
    if (&absolution_globals_size != NULL && absolution_globals_size != 0) {
        prefix_size = absolution_globals_size;
    }
    size_t tail_size;

    if (prefix_size == 0 || prefix_size >= max_size) {
        return LLVMFuzzerMutate(data, size, max_size);
    }

    if (FUZZ_CTRL_OFF + FUZZ_CTRL_LEN > prefix_size) {
        return LLVMFuzzerMutate(data, size, max_size);
    }

#if FUZZ_APP_DATA_LEN > 0
    if (FUZZ_APP_DATA_OFF + FUZZ_APP_DATA_LEN > prefix_size) {
        return LLVMFuzzerMutate(data, size, max_size);
    }
#endif

    if (size == 0) {
        size = prefix_size + 256 <= max_size ? prefix_size + 256 : max_size;
        memset(data, 0, size);
    }

    if (size <= prefix_size) {
        return LLVMFuzzerMutate(data, size, max_size);
    }

    tail_size = size - prefix_size;

    int is_structured = fuzz_lane_is_structured(data, prefix_size);

    if (is_structured) {
        unsigned int dice = seed % 100;

#if FUZZ_APP_DATA_LEN > 0
        if (dice < 40) {
            size_t start = FUZZ_APP_DATA_OFF + ((seed >> 8) % FUZZ_APP_DATA_LEN);
            size_t span  = FUZZ_APP_DATA_LEN - (start - FUZZ_APP_DATA_OFF);
            if (span > 64) {
                span = 64;
            }
            (void) LLVMFuzzerMutate(data + start, span, span);
        }
        else if (dice < 70) {
#else
        if (dice < 60) {
#endif
            tail_size = LLVMFuzzerMutate(data + prefix_size, tail_size, max_size - prefix_size);
        }
        else if (dice < 90) {
            (void) LLVMFuzzerMutate(data + FUZZ_CTRL_OFF, FUZZ_CTRL_LEN, FUZZ_CTRL_LEN);
        }
        else {
            size_t start = (seed >> 8) % prefix_size;
            size_t span  = prefix_size - start;
            if (span > 32) {
                span = 32;
            }
            (void) LLVMFuzzerMutate(data + start, span, span);
        }

#ifdef FUZZ_INJECT_TOKEN
#if FUZZ_APP_DATA_LEN > 0
        if (((seed >> 4) & 1U) == 0) {
            FUZZ_INJECT_TOKEN(data + FUZZ_APP_DATA_OFF, FUZZ_APP_DATA_LEN, seed >> 16);
        }
#endif
#endif
    }
    else {
        if ((seed & 3U) == 0) {
            size_t start = (seed >> 2) % prefix_size;
            size_t span  = prefix_size - start;
            if (span > 32) {
                span = 32;
            }
            (void) LLVMFuzzerMutate(data + start, span, span);
        }

        tail_size = LLVMFuzzerMutate(data + prefix_size, tail_size, max_size - prefix_size);

#ifdef FUZZ_INJECT_TOKEN
        if (tail_size > 0 && ((seed >> 4) & 1U) == 0) {
            FUZZ_INJECT_TOKEN(data + prefix_size, tail_size, seed >> 16);
        }
#endif
    }

    return prefix_size + tail_size;
}

static size_t fuzz_custom_mutator(uint8_t *data, size_t size, size_t max_size, unsigned int seed)
{
    return fuzz_mutate_with_split(data, size, max_size, seed);
}
