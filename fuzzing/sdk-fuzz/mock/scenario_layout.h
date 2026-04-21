#pragma once

/* Multi-target SDK build: prefix size varies per target and is resolved
   from the Absolution-generated fuzzer.c at build time.  No static
   SCEN_PREFIX_SIZE is defined here; harness code must use
   absolution_globals_size at runtime. */
#define SCEN_CTRL_OFF 0
#define SCEN_CTRL_LEN 16
