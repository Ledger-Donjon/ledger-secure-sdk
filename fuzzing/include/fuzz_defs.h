#pragma once
/*
 * Framework-wide constants for Absolution-based Ledger app fuzzers.
 *
 * This header has no dependencies and can be included from anywhere:
 * mocks.h, harness, mutator wiring, etc.
 */

/*
 * Structured lane threshold: control header byte 0 > this value means
 * structured lane (~60%), otherwise raw lane (~40%).
 *
 * All lane predicates, seed generators, and harness code must use this
 * value consistently.  Changing it invalidates all existing corpus inputs.
 */
#define FUZZ_STRUCTURED_LANE_THRESHOLD 102
