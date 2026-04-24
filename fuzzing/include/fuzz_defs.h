#pragma once
/*
 * Framework-wide constants and POD types for Absolution-based Ledger app
 * fuzzers.  Keeps zero dependencies beyond <stdint.h> so it can be
 * included from anywhere: mocks.h, harness, mutator wiring, etc.
 */

#include <stdint.h>

/*
 * Structured lane threshold: control header byte 0 > this value means
 * structured lane (~60%), otherwise raw lane (~40%).
 *
 * All lane predicates, seed generators, and harness code must use this
 * value consistently.  Changing it invalidates all existing corpus inputs.
 */
#define FUZZ_STRUCTURED_LANE_THRESHOLD 102

/* ── APDU policy flags ─────────────────────────────────────────────────────── */

#define FUZZ_CMD_HAS_DATA    (1u << 0) /* command expects a data payload     */
#define FUZZ_CMD_NO_RESPONSE (1u << 2) /* skip response handling             */

/* ── Command spec ──────────────────────────────────────────────────────────── *
 *
 * Declared here (not in fuzz_harness.h) so apps can pre-declare lane-specific
 * command tables before overriding FUZZ_PICK_COMMAND_RAW / _STRUCTURED.
 */
typedef struct {
    uint8_t cla;
    uint8_t ins;
    uint8_t p1_max; /* max valid P1; 0 = full range [0,255] */
    uint8_t p2_max; /* max valid P2; 0 = full range [0,255] */
    uint8_t flags;  /* FUZZ_CMD_* bitfield                  */
} fuzz_command_spec_t;
