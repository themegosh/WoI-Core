/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WoI_h__
#define WoI_h__

#include "Define.h"

class Player;

// trinity_string ids for the World of Immortality customizations.
// 11000-11021 are taken by upstream, so these start at 11100.
enum WoIStrings : uint32
{
    WOI_STRING_HEAL_PLAYER              = 11103,
    WOI_STRING_SHOW_VOTE_POINTS         = 11104,
    WOI_STRING_SHOW_VOTE_POINTS_PLAYER  = 11105,
    WOI_STRING_HAS_NOT_VOTED            = 11106,
    WOI_STRING_HAS_VOTED                = 11107,
    WOI_STRING_CHANGED_VOTE_POINTS      = 11108,
    WOI_STRING_NEVER_VOTED              = 11109,
    WOI_STRING_FORCED_VOTE              = 11110,
    WOI_STRING_FORCED_YOUR_VOTE         = 11111,
    WOI_STRING_TELE_VOTE_CHECK_FAILED   = 11112,
    WOI_STRING_NO_GUILD_HOUSE           = 11114
};

namespace WoI
{
    namespace Vote
    {
        constexpr uint32 VoteCooldownSeconds = 12 * 60 * 60;
        constexpr int32 InstanceResetCost = 2;

        // -1 when the account has no `voting_points` row (never voted).
        TC_GAME_API int32 GetPoints(Player* player);
        TC_GAME_API void ModPoints(Player* player, int32 amount, bool required = false);
        TC_GAME_API bool HasVoted(Player* player);
        // Records a vote for the player's IP as if they had just voted (GM command).
        TC_GAME_API void ForceVote(Player* player);
        TC_GAME_API void UnbindAllInstances(Player* player);
        // GM tool: wipes every row of `character_instance`.
        TC_GAME_API void ResetAllInstances(Player* player);
    }

    namespace GuildHouse
    {
        constexpr uint32 MapId = 37;

        // 1 (the normal phase) when the guild has no `guild_house_masks` row.
        TC_GAME_API uint32 GetPhaseMask(Player const* player);
        // Applies the guild-house phase rules to a teleport. False vetoes the teleport.
        TC_GAME_API bool OnTeleport(Player* player, uint32 destinationMapId);
    }

    namespace ExternalMail
    {
        // Delivers and removes everything queued in `mail_external` / `mail_external_items`.
        TC_GAME_API void SendQueued();
    }
}

#endif
