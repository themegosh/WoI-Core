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
#include "GameTime.h"
#include "Player.h"
#include "WorldSession.h"

namespace WoI::Vote
{
namespace
{
    void Notify(Player* player, char const* text)
    {
        ChatHandler(player->GetSession()).SendSysMessage(text);
    }
}

int32 GetPoints(Player* player)
{
    QueryResult result = LoginDatabase.PQuery("SELECT `points` FROM `voting_points` WHERE `id` = {}", player->GetSession()->GetAccountId());
    if (!result)
        return -1;

    return int32((*result)[0].GetUInt32());
}

void ModPoints(Player* player, int32 amount, bool required)
{
    int32 const current = GetPoints(player);
    if (current == -1)
    {
        Notify(player, "|cffffcc00[Vote Manager]|r|cff00ff00 It appears you have not voted for us before. You must vote at least once to use this utility.|r");
        return;
    }

    uint32 newPoints = uint32(current + amount);
    if (amount < 0 && current < -amount)
    {
        if (required)
        {
            Notify(player, "|cffffcc00[Vote Manager]|r|cffff0000 You do not have enough Vote Points to use this utility.|r");
            return;
        }
        newPoints = 0;
    }

    ChatHandler(player->GetSession()).PSendSysMessage("|cffffcc00[Vote Manager]|r|cff00ff00 You now have %u Vote Points.|r", newPoints);
    LoginDatabase.PExecute("UPDATE `voting_points` SET `points` = {} WHERE `id` = {}", newPoints, player->GetSession()->GetAccountId());
}

bool HasVoted(Player* player)
{
    uint32 const since = uint32(GameTime::GetGameTime() - VoteCooldownSeconds);
    WorldSession* session = player->GetSession();

    QueryResult byIp = LoginDatabase.PQuery("SELECT 1 FROM `voting` WHERE `user_ip` = '{}' AND `time` > {}", session->GetRemoteAddress(), since);
    if (byIp)
        return true;

    QueryResult byAccount = LoginDatabase.PQuery("SELECT 1 FROM `voting_points` WHERE `id` = {} AND `time` > {}", session->GetAccountId(), since);
    return bool(byAccount);
}

void ForceVote(Player* player)
{
    std::string const& ip = player->GetSession()->GetRemoteAddress();
    uint32 const now = uint32(GameTime::GetGameTime());

    QueryResult existing = LoginDatabase.PQuery("SELECT 1 FROM `voting` WHERE `user_ip` = '{}'", ip);
    if (existing)
        LoginDatabase.PExecute("UPDATE `voting` SET `time` = {} WHERE `user_ip` = '{}'", now, ip);
    else
        LoginDatabase.PExecute("INSERT INTO `voting` (`user_ip`, `time`) VALUES ('{}', {})", ip, now);
}

void UnbindAllInstances(Player* player)
{
    Settings const& settings = GetSettings();
    Currency const currency = InstanceResetCurrency();

    if (currency == Currency::VotePoints && GetPoints(player) < int32(settings.instanceResetVotePoints))
    {
        Notify(player, "|cffffcc00[Vote Manager]|r|cffff0000 You do not have enough Vote Points to use this utility.|r");
        return;
    }
    if (currency == Currency::Gold && int64(player->GetMoney()) < int64(settings.instanceResetGold) * 10000)
    {
        Notify(player, "You do not have enough gold.");
        return;
    }

    uint32 counter = 0;
    for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
    {
        Player::BoundInstancesMap& binds = player->GetBoundInstances(Difficulty(i));
        for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end();)
        {
            if (itr->first != player->GetMapId())
            {
                player->UnbindInstance(itr, Difficulty(i));
                ++counter;
            }
            else
                ++itr;
        }
    }

    if (!counter)
    {
        Notify(player, "You do not have any instance bindings to reset.");
        return;
    }

    ChatHandler(player->GetSession()).PSendSysMessage("All (%u) of your instances have been reset!", counter);
    if (currency == Currency::VotePoints)
        ModPoints(player, -int32(settings.instanceResetVotePoints), true);
    else if (currency == Currency::Gold)
        TryChargeGold(player, settings.instanceResetGold);
}

void ResetAllInstances(Player* player)
{
    if (player->GetSession()->GetSecurity() < SEC_ADMINISTRATOR)
    {
        Notify(player, "|cffffcc00[You are not a level 3 GM]|r|cff00ff00 Please contact an admin to reset the instances.|r");
        return;
    }

    CharacterDatabase.Execute("DELETE FROM `character_instance`");
    Notify(player, "Instances have been reset.");
}
}
