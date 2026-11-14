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

## Database

Apply the files in `sql/custom/` (`auth`, `characters`, `world`). The schemas of `voting`, `voting_points`, `guild_house_masks` and the `mail_external*` tables, and the texts of the custom `trinity_string` rows, were reconstructed from how the code uses them. Prefer your live definitions where they differ.

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
