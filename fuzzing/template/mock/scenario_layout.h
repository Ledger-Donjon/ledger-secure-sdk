#pragma once

/*
 * Bootstrap prefix layout.
 *
 * scripts/update-scenario-layout.py overwrites these values after the first
 * build. Advanced apps can add extra SCEN_* offsets for app-specific globals,
 * but the minimal template only needs the three definitions below.
 */
#define SCEN_PREFIX_SIZE 64
#define SCEN_CTRL_OFF    0
#define SCEN_CTRL_LEN    16
