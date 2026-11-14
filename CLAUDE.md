# WoI-Core notes for Claude

World of Immortality server core: upstream TrinityCore `3.3.5` plus the WoI customizations. See `doc/WoI-Customizations.md` for where everything lives and for the `WoI.*` config options.

## Git remotes: where to push

| Remote | URL | Use |
|---|---|---|
| `fork` | `https://github.com/themegosh/WoI-Core.git` | **The user's own repo. Push here.** |
| `origin` | `https://github.com/brucey125/WoI-Core.git` | Original owner's repo. The user has no write access. **Never push here.** |
| `upstream` | `https://github.com/TrinityCore/TrinityCore.git` | Upstream TrinityCore, fetch only. |

- `master` on `fork` is the current WoI tree (upstream 3.3.5 with the customizations applied). The pre-port fork history stays reachable from it.
- Push with `git push fork <branch>`. A bare `git push` only works on branches that track `fork/...`.
- `gh` is logged in as `themegosh`.

## Build

No build tree is checked in. New `.cpp` files under `src/server/game/WoI/` need a CMake re-run to be picked up.
