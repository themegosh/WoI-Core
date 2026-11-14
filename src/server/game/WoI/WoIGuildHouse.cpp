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

#include "WoI.h"
#include "Chat.h"
#include "DatabaseEnv.h"
#include "Player.h"
#include "WorldSession.h"

namespace WoI::GuildHouse
{
uint32 GetPhaseMask(Player const* player)
{
    if (!player->GetGuildId())
        return 1;

    QueryResult result = CharacterDatabase.PQuery("SELECT `phasemask` FROM `guild_house_masks` WHERE `guid` = {}", player->GetGuildId());
    if (!result)
        return 1;

    // The table stores a phase index: 1 is the normal phase, 0 maps to bit 1, N >= 2 maps to bit N.
    uint64 const index = (*result)[0].GetUInt64();
    if (index == 1)
        return 1;

    return uint32(uint64(2) << index);
}

bool OnTeleport(Player* player, uint32 destinationMapId)
{
    if (destinationMapId == MapId)
    {
        if (player->IsGameMaster())
            player->SetPhaseMask(uint32(-1), true);
        else
        {
            uint32 const mask = GetPhaseMask(player);
            if (mask <= 1)
            {
                ChatHandler(player->GetSession()).SendSysMessage(WOI_STRING_NO_GUILD_HOUSE);
                return false;
            }
            player->SetPhaseMask(mask, true);
        }
    }
    else if (player->GetMapId() == MapId)
        player->SetPhaseMask(1, true);

    return true;
}
}
