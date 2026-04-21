# Ledger SDK Fuzzing Framework

Coverage-guided fuzzing for the Ledger Secure SDK and Ledger apps. The
framework wraps LibFuzzer and clang sanitizers, and uses
[Absolution](https://github.com/Ledger-Donjon/absolution) to drive global
state through declarative invariants.

This directory is the single source of truth. Every shared piece — headers,
mocks, CMake modules, sanitizer config, campaign scripts, and docs — lives
here. Apps only add their own `fuzzing/` subtree; nothing is copied out of
the SDK.

## Concepts in plain English

- **Campaign**: one fuzzing run. `scripts/app-campaign.sh` builds the fuzzer,
  generates seeds, runs a short **warmup** phase (wide coverage fast), then a
  longer **main** phase (depth-first from the warmup corpus), then replays
  the surviving corpus against a coverage build to produce an HTML report.

- **Corpus**: the set of inputs LibFuzzer keeps around because each one
  triggered a new code path. The fuzzer refines it continuously — adding
  interesting inputs, dropping redundant ones. A campaign grows its corpus
  from an initial set of seeds.

- **Seed**: a starter input used before the fuzzer begins mutating. Seeds
  come from the dictionary and templates declared in `fuzz-manifest.toml` and
  from any `base-corpus/` directory the manifest points at. Good seeds
  shorten the time to first coverage.

- **Base corpus**: a promoted, on-disk corpus checked into the app (by
  convention at `<app>/fuzzing/base-corpus/`). The pipeline picks it up as
  additional seeds for the next campaign. Promote a corpus once it covers
  the features you care about so future runs start from that state.

- **Invariant** (`.zon` file): an Absolution description of the fuzz target's
  global state — which fields exist, their widths, their value domains
  (enums, BIP32 paths, swap flags, etc.). Absolution uses it to interpret
  the start of each fuzzer input as a description of the initial state.

- **Prefix** / **tail**: every fuzzer input is split by Absolution into two
  halves. The *prefix* (first N bytes) sets up global state via the
  invariant. The *tail* (remaining bytes) is the APDU / payload the harness
  actually processes. The split offset is written to `scenario_layout.h` by
  the pipeline after each build.

- **Zero-symbols** (`invariants/zero-symbols.txt`): globals the prefix must
  forcibly zero instead of driving — typically large buffers, display state,
  or framework bookkeeping the app does not need to fuzz. Keeps the prefix
  small and focused.

- **Domain overrides** (`invariants/domain-overrides.txt`): per-symbol
  constraints on what values Absolution may place in the prefix (e.g.
  restrict a `uint8_t` enum to `{0,1,2}`). Improves convergence on the
  states the app actually reaches.

## Quickstart

Fuzz an existing app (it must provide a `fuzzing/` folder that follows
[docs/APP_CONTRACT.md](docs/APP_CONTRACT.md)):

```bash
BOLOS_SDK=/path/to/ledger-secure-sdk \
  "$BOLOS_SDK"/fuzzing/scripts/app-campaign.sh \
  --app-dir /path/to/app my-campaign
```

Fuzz the SDK's own targets (10 built-in fuzzers under `sdk-fuzz/`):

```bash
"$BOLOS_SDK"/fuzzing/scripts/app-campaign.sh \
  --app-dir "$BOLOS_SDK" \
  --fuzz-subdir fuzzing/sdk-fuzz sdk-sanity
```

Each run writes artefacts to `<app-dir>/.fuzz-artifacts/<campaign-name>/`:

- `targets/<fuzzer>/base-corpus/` — starting seeds
- `targets/<fuzzer>/warmup/`, `warmup-merged/`, `main/` — per-worker corpora
- `targets/<fuzzer>/meta.env`, `<fuzzer>.dict` — run metadata
- `report/index.html` — combined LLVM source-level coverage

Crashes, if any, appear as `crash-*` files under the worker directories and
are summarised at the end of the run.

Common environment variables:

| Variable     | Default | Meaning                                 |
|--------------|---------|-----------------------------------------|
| `WARMUP_SEC` | 120     | Warmup phase duration per worker        |
| `MAIN_SEC`   | 900     | Main phase duration per worker          |
| `WORKERS`    | `nproc` | Parallel LibFuzzer workers              |
| `OVERWRITE`  | unset   | Reuse an existing campaign directory    |

Full flag reference: see [docs/APP_CONTRACT.md](docs/APP_CONTRACT.md).

## Prerequisites

- Clang ≥ 14 with `llvm-profdata` and `llvm-cov` (for coverage reports).
- Absolution built and resolvable (see `docs/APP_CONTRACT.md` §Absolution
  resolution — `ABSOLUTION_DIR`, `--absolution-dir`, or a sibling checkout).
- `BOLOS_SDK` pointing at a checkout of this SDK.

## Directory layout

| Path              | Purpose                                                                                   |
|-------------------|-------------------------------------------------------------------------------------------|
| `cmake/`          | `LedgerAppFuzz.cmake` — the CMake module apps include                                     |
| `docs/`           | `APP_CONTRACT.md` (integration contract) + `fuzz_lists.md` (lists mock reference)         |
| `template/`       | Minimal app fuzzing scaffold; copy into a new app as `fuzzing/`                           |
| `scripts/`        | Campaign pipeline (`app-campaign.sh`, seed generators, invariant / layout sync)           |
| `include/`        | Framework headers + optional TLV grammar-aware mutator                                    |
| `mock/common/`    | Mock sources linked into every fuzzer (e.g. streaming SHA-256)                            |
| `mock/custom/`    | Strong mock overrides (crypto, NBGL, system, BN/EC)                                       |
| `mock/gen_mock.py`| Generator for weak syscall stubs                                                          |
| `libs/`           | Per-library CMake modules aggregated into the `secure_sdk` INTERFACE target               |
| `macros/`         | Build macro extraction and add / exclude lists                                            |
| `invariants/`     | SDK-level zero-symbol policy (applied to every app)                                       |
| `sanitizers/`     | UBSan / ASan runtime config and ignorelists                                               |
| `sdk-fuzz/`       | 10 self-fuzz targets exercising the framework with the same app contract                  |
| `shared_libs/`    | Prebuilt NBGL shared libraries                                                            |

## For app developers

Start from [`template/`](template/README.md) and follow
[`docs/APP_CONTRACT.md`](docs/APP_CONTRACT.md). The app owns its `fuzzing/`
folder (harness, manifest, invariants, macro overrides); everything else is
pulled from the SDK through `include(${BOLOS_SDK}/fuzzing/cmake/LedgerAppFuzz.cmake)`.
