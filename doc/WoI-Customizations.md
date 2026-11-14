# World of Immortality customizations

This tree is upstream TrinityCore `3.3.5` with the World of Immortality features re-applied on top. The old fork lives on in the `master` branch.

## Where things live

| Feature | Code | Core hook |
|---|---|---|
| Vote points (`.vp show/modify/check/force`, `.heal`) | `src/server/game/WoI/WoIVote.cpp`, `src/server/scripts/Custom/WoI/cs_woi.cpp` | none |
| Teleporter NPCs | `src/server/scripts/Custom/WoI/tele_npc_vote.cpp`, `tele_npc_fun_vote.cpp` | none |
| Guild house phasing (map 37) | `src/server/game/WoI/WoIGuildHouse.cpp` | `Player::TeleportTo` |
| External mail queue | `src/server/game/WoI/WoIExternalMail.cpp`, `src/server/scripts/Custom/WoI/woi_world.cpp` | none |
| Anticheat | `src/server/game/WoI/Anticheat/`, `src/server/scripts/Custom/WoI/cs_anticheat.cpp` | `HandleMovementOpcode` |
| Renamed commands | `cs_misc.cpp`: `.appear` is `.goname`, `.summon` is `.namego` | |

Scripts are registered in `src/server/scripts/Custom/custom_script_loader.cpp`. The NPC script names (`tele_npc_vote`, `tele_npc_fun_vote`) are unchanged, so existing `creature_template.ScriptName` values keep working.

## Configuration

All options live in the "WORLD OF IMMORTALITY CUSTOMIZATIONS" section of `worldserver.conf.dist`.

- `WoI.Access.Mode` picks who gets the restricted teleporter menus: `0` everybody, `1` voters, `2` whitelisted accounts, `3` voters or whitelisted. The whitelist is the `woi_access` table in the auth database (one row per account id), so donors and friends are a single `INSERT`. At `0` no vote messages are shown.
- `WoI.InstanceReset.Currency` is `0` free, `1` vote points or `2` gold, with `WoI.InstanceReset.VotePoints` and `WoI.InstanceReset.Gold` as the prices. A price of 0 means free, and the confirmation step is skipped. Menu labels follow the configured price.
- `WoI.Teleporter.CooldownResetGold` and `WoI.FunTeleporter.CooldownResetGold` price "Reset Cooldowns" on each NPC (the original NPCs charged 100 and 1000 gold).
- `WoI.GuildHouse.Enable` hides the Guild House menu entry. The map id stays 37 and existing `guild_house_masks` rows keep working.
- `Anticheat.*` and `ExternalMail*` as before. `Anticheat.Enable` now ships disabled.

Teleporting in combat stays blocked, and the `.vp` and GM commands keep their RBAC permissions.

## Database

Apply the files in `sql/custom/` (`auth`, `characters`, `world`). `woi_access.sql` is only needed for whitelist mode. The schemas of `voting`, `voting_points`, `guild_house_masks` and the `mail_external*` tables, and the texts of the custom `trinity_string` rows, were reconstructed from how the code uses them. Prefer your live definitions where they differ.

Custom strings moved from `11002-11016` to `11100+` because upstream now uses `11002-11021`.

## Dropped because upstream covers them

- Lifebloom dispel heal and Spellsteal stacking: upstream `spell_dru_lifebloom` and `Unit::RemoveAurasDueToSpellBySteal`.
- Shadowmourne 71903/71905 stack handling: upstream `spell_item_shadowmourne*`.
- Ruby Sanctum scripts: upstream has a full implementation.
- Sap and Gouge no longer share a diminishing-returns group with Polymorph in the old fork. Upstream groups all three as `DIMINISHING_DISORIENT`. Restoring the split needs a new DR group.
- `Player::AddItemVS` (unused), `.groupsummon` renamed to `.groupgo` (upstream now has `.group summon`).

## Intentional behavior fixes during the port

- External mail without attached items was never deleted from the queue and would have been resent every interval. Rows are now removed per mail.
- `.vp modify` reports the resulting balance correctly when the balance was zero.
