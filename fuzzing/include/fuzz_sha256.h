#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t state[8];
    uint64_t count;
    uint8_t  buf[64];
    size_t   buf_len;
} fuzz_sha256_ctx;

void fuzz_sha256_init(fuzz_sha256_ctx *ctx);
void fuzz_sha256_update(fuzz_sha256_ctx *ctx, const uint8_t *data, size_t len);
void fuzz_sha256_final(fuzz_sha256_ctx *ctx, uint8_t digest[32]);
void fuzz_sha256(const uint8_t *data, size_t len, uint8_t digest[32]);

void fuzz_sha256_iovec(const void *iovec, size_t iovec_len, uint8_t digest[32]);
void fuzz_sha256_transform(uint32_t state[8], const uint8_t block[64]);
