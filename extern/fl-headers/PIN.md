# Engine interface header pin

Verbatim copies of the fighters-legacy engine's content-pack interface headers.
Vendored because the engine exports no CMake package; the set is closed and
std-only. **This pin is load-bearing** — the `IContentPack` interface may still
gain pure virtuals, and a stale pin means the plugin compiles against a contract
the engine no longer honours. Bump deliberately, never casually.

## Current pin

| | |
|---|---|
| Source | [fighters-legacy/fighters-legacy](https://github.com/fighters-legacy/fighters-legacy) `engine/content/` |
| Tag | `v0.2.6` |
| Commit | `7ff14645d86de8538c353a4c4f55adf7abc4d998` |
| Vendored | 2026-07-02 |
| Verified | identical to `origin/main` as of 2026-07-02 |

## Files

```
53398c2ee64467154e7b7aa2e59fa8d67aa872a762592d532e9fac0e351765d4  content/AssetTypes.h
b875dd5e697d9f6c84a8aec4ffee584dbdb686e107878380218ed3a35a4ddc64  content/IContentPack.h
02ef07e5c918ea449b0054999d02fd2a343058059b211cbdbbce9945f6261a2f  content/TrustLevel.h
```

Verify integrity (from `extern/fl-headers/`):

```bash
grep -E '^[0-9a-f]{64}' PIN.md | sha256sum -c -
```

Verify against an engine checkout:

```bash
for h in IContentPack.h AssetTypes.h TrustLevel.h; do
  git -C ../../../fighters-legacy show "v0.2.6:engine/content/$h" \
    | diff - "content/$h" && echo "$h OK"
done
```

## Update procedure

1. Pick the new engine tag; copy the three headers verbatim from
   `engine/content/` at that tag (`git show <tag>:engine/content/<file>`).
   Never edit the copies — not even the SPDX lines.
2. `diff` the old and new copies. **If `IContentPack` gained pure virtuals,
   implement them in `FaContentPack` in the same PR** — the plugin must never
   ship compiled against a narrower interface than the engine calls.
3. Regenerate the sha256 list above (`sha256sum content/*.h`), update the
   pin table (tag, commit, dates), and update `manifest.toml.in`'s
   `engine-api` if the engine's major API version changed.

## No automation watches this pin

Dependabot watches `extern/fx_lib` (a git submodule); these are plain files —
updates are manual and deliberate, by design.

## Licensing

The headers are GPL-3.0-or-later, © MKZ Systems LLC (see `REUSE.toml`).
`IContentPack.h` carries the engine's linking exception for content-pack
implementors in its header comment; the exception has no SPDX identifier, so
the in-file text is authoritative.
