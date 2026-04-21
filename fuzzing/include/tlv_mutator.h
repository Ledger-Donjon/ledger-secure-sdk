#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * Grammar-aware TLV custom mutator for LibFuzzer.
 *
 * Set current_tlv_fuzz_config from your harness with the tag grammar
 * of your TLV use case, then the LLVMFuzzerCustomMutator in the harness
 * delegates tail mutation to tlv_custom_mutate().
 */

typedef struct {
    uint8_t tag;
    uint8_t min_len;
    uint8_t max_len;
} tlv_tag_info_t;

typedef struct {
    const tlv_tag_info_t *tags_info;
    size_t                num_tags;
} tlv_fuzz_config_t;

extern tlv_fuzz_config_t current_tlv_fuzz_config;

/**
 * Apply a grammar-aware TLV mutation to the given buffer.
 * Returns the new size of the mutated data.
 */
size_t tlv_custom_mutate(uint8_t *data, size_t size, size_t max_size, unsigned int seed);
