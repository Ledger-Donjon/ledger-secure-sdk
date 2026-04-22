# Fuzzing Template

Copy this directory to `<APP_SRC>/fuzzing/`, then compare each file with
`app-boilerplate/fuzzing/` to fill in the app-specific parts.

## What to edit

| File | What you fill in |
|---|---|
| `fuzz-manifest.toml` | CLA, INS list, key coverage files, dictionary tokens |
| `CMakeLists.txt` | project name, source globs, include directories, compile definitions |
| `harness/fuzz_dispatcher.c` | command table, app reset, dispatcher call |
| `mock/mocks.h` / `mock/mocks.c` | extra globals or app-specific mocks only if needed |
| `invariants/zero-symbols.txt` | app globals that should be removed from the prefix |
| `invariants/domain-overrides.txt` | enum and state constraints after the first sync |

## First run

```bash
BOLOS_SDK=/path/to/ledger-secure-sdk \
  "$BOLOS_SDK"/fuzzing/scripts/app-campaign.sh \
  --app-dir /path/to/your-app my-first-run
```

- **`my-first-run`** is the campaign name (optional); output goes to
  `/path/to/your-app/.fuzz-artifacts/my-first-run/`. Omit it to use a UTC
  timestamp.
- Defaults are a **quick local profile**: `WARMUP_SEC=30`, `MAIN_SEC=60`,
  `WORKERS=min(2, nproc)`. Override for longer jobs (see SDK fuzzing README).

Example: reuse a merged corpus from an earlier campaign (must match current
`.compat-key` if present):

```bash
EXTRA_CORPUS=/path/to/your-app/.fuzz-artifacts/prior-run/targets/fuzz_globals/corpus \
  "$BOLOS_SDK"/fuzzing/scripts/app-campaign.sh --app-dir /path/to/your-app chained-run
```

The campaign builds the app, syncs the invariant, updates `scenario_layout.h`,
generates seeds, runs fuzzing, and writes the coverage report.

## Reference

See `${BOLOS_SDK}/fuzzing/docs/APP_CONTRACT.md` for the full app-facing contract
(env vars, `EXTRA_CORPUS`, CLI flags) and `${BOLOS_SDK}/fuzzing/cmake/LedgerAppFuzz.cmake`
for the CMake integration module.
