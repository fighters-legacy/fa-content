# Development Guide

## Prerequisites

### Linux — Fedora (primary maintainer platform)

```bash
sudo dnf install cmake ninja-build gcc g++ clang clang-tools-extra lcov
```

### Linux — Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y cmake ninja-build gcc g++ clang clang-format lcov
```

### Windows (MSVC)

1. Install [Visual Studio](https://visualstudio.microsoft.com/) with the **Desktop development with C++** workload
2. Optional: install Ninja via `winget install Ninja-build.Ninja`

### macOS (Apple Silicon, 13+)

```bash
xcode-select --install
brew install cmake ninja
```

For platform-specific fighters-legacy prerequisites (Vulkan SDK, SDL3, OpenAL Soft), see [fighters-legacy/docs/development.md](https://github.com/fighters-legacy/fighters-legacy/blob/main/docs/development.md).

---

## First-time setup

```bash
git clone --recurse-submodules https://github.com/fighters-legacy/fa-bridge.git
cd fa-bridge
```

If you already cloned without `--recurse-submodules`:

```bash
git submodule update --init --recursive
```

This pulls `extern/fx_lib` from fighters-codex — the FA format parser library, pinned to a codex release tag.

---

## Building

```bash
# Linux / macOS
cmake --preset debug
cmake --build --preset debug

# Windows (PowerShell)
cmake --preset debug-msvc
cmake --build --preset debug-msvc
```

The build produces the plugin shared library (`libfa-bridge.so` / `fa-bridge.dll` /
`libfa-bridge.dylib`) and stages a drop-in mod directory at
`build/<preset>/stage/mods/fa-bridge/` containing the plugin and its `manifest.toml`.

### `FA_WITH_FX_LIB`

fx_lib does not compile on Linux/macOS until fighters-codex Phase 1 lands, so the
`FA_WITH_FX_LIB` option defaults **OFF**. To build against fx_lib (e.g. on Windows, or
once the codex port lands):

```bash
cmake --preset debug -DFA_WITH_FX_LIB=ON
```

### `FA_BUILD_TESTS`

The test suite builds by default. Set `-DFA_BUILD_TESTS=OFF` to build only the plugin
(e.g. when packaging).

### Running tests

```bash
ctest --preset debug --output-on-failure
```

---

## Runtime testing against a real FA installation (from Phase 2 onward)

Set `FA_INSTALL_DIR` to point at your FA installation directory before running integration tests:

```bash
export FA_INSTALL_DIR="/path/to/Janes Fighters Anthology"
ctest --preset debug --output-on-failure
```

On Windows (PowerShell):

```powershell
$env:FA_INSTALL_DIR = "C:\Games\Janes Fighters Anthology"
ctest --preset debug-msvc --output-on-failure
```

Tests that require a real FA installation are skipped if `FA_INSTALL_DIR` is not set. CI runs without it; only the unit and integration tests that operate on bundled synthetic fixtures run in CI.

---

## Testing the plugin against an engine build

Every build stages a ready-to-load mod directory. Copy it into the `mods/` directory the
engine scans (`fl-server`: the working directory; the game: next to the binary):

```bash
cp -r build/debug/stage/mods/fa-bridge /path/to/engine/mods/
FA_INSTALL_DIR=/path/to/anything ./fl-server
```

Set `FA_INSTALL_DIR` to an existing directory so the stub reports `Ready` —
otherwise the engine's `AssetManager` drops the pack after loading it
(`NeedsConfiguration` with no configure UI), which is correct but noisy.

**Windows:** the plugin and the engine must be built in the **same configuration**
(Debug with Debug, Release with Release) — mixed CRT/iterator-debug-level builds fail
at load time. See the ABI discipline note in [architecture.md](architecture.md).

---

## Git setup

Install the commit-msg hook to auto-append `Signed-off-by` (DCO requirement):

```bash
cp scripts/hooks/commit-msg .git/hooks/
chmod +x .git/hooks/commit-msg
```

---

## Local preset overrides

Create `CMakeUserPresets.json` in the repo root to override preset defaults (e.g. a different build directory or custom toolchain). This file is gitignored.

---

## Code coverage

The coverage workflow runs on every push and pull request. Locally:

```bash
cmake --preset coverage
cmake --build --preset coverage
ctest --preset coverage --output-on-failure
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/tests/*' '*/vendor/*' '*/extern/*' \
     --output-file coverage.info
```

---

## Release workflow

Releases are tagged with `vMAJOR.MINOR.PATCH`. Both scripts are stdlib-only Python — no extra tooling to install.

```bash
python3 scripts/draft-changelog.py     # draft [Unreleased] entries from conventional commits
# curate CHANGELOG.md by hand
python3 scripts/release.py X.Y.Z       # verify fx_lib pin, bump version, rotate changelog, commit + tag
git push origin main --tags
```

`release.py` refuses to release unless `extern/fx_lib` is pinned to a fighters-codex
release tag, and writes that pin into the release notes.
