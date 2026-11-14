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
#include "Config.h"
#include "DatabaseEnv.h"
#include "Player.h"
#include "WorldSession.h"

namespace WoI
{
namespace
{
    constexpr int64 CopperPerGold = 10000;

    Settings _settings;

    bool IsWhitelisted(Player* player)
    {
        QueryResult result = LoginDatabase.PQuery("SELECT 1 FROM `woi_access` WHERE `account_id` = {}", player->GetSession()->GetAccountId());
        return bool(result);
    }
}

void LoadSettings()
{
    Settings s;
    s.accessMode = uint32(sConfigMgr->GetIntDefault("WoI.Access.Mode", ACCESS_VOTE)) & (ACCESS_VOTE | ACCESS_WHITELIST);

    uint32 const currency = uint32(sConfigMgr->GetIntDefault("WoI.InstanceReset.Currency", uint32(Currency::VotePoints)));
    s.instanceResetCurrency = currency <= uint32(Currency::Gold) ? Currency(currency) : Currency::VotePoints;
    s.instanceResetVotePoints = uint32(sConfigMgr->GetIntDefault("WoI.InstanceReset.VotePoints", int32(s.instanceResetVotePoints)));
    s.instanceResetGold = uint32(sConfigMgr->GetIntDefault("WoI.InstanceReset.Gold", int32(s.instanceResetGold)));
    s.teleporterCooldownResetGold = uint32(sConfigMgr->GetIntDefault("WoI.Teleporter.CooldownResetGold", int32(s.teleporterCooldownResetGold)));
    s.funTeleporterCooldownResetGold = uint32(sConfigMgr->GetIntDefault("WoI.FunTeleporter.CooldownResetGold", int32(s.funTeleporterCooldownResetGold)));
    s.guildHouseEnabled = sConfigMgr->GetBoolDefault("WoI.GuildHouse.Enable", true);
    _settings = s;
}

Settings const& GetSettings()
{
    return _settings;
}

bool HasAccess(Player* player)
{
    if (_settings.accessMode == 0)
        return true;
    if ((_settings.accessMode & ACCESS_VOTE) && Vote::HasVoted(player))
        return true;
    if ((_settings.accessMode & ACCESS_WHITELIST) && IsWhitelisted(player))
        return true;
    return false;
}

void SendAccessDenied(Player* player)
{
    ChatHandler(player->GetSession()).SendSysMessage(_settings.accessMode == ACCESS_VOTE ? WOI_STRING_TELE_VOTE_CHECK_FAILED : WOI_STRING_TELE_ACCESS_DENIED);
}

// A zero price is free whatever the currency says.
Currency InstanceResetCurrency()
{
    switch (_settings.instanceResetCurrency)
    {
        case Currency::VotePoints:
            return _settings.instanceResetVotePoints ? Currency::VotePoints : Currency::Free;
        case Currency::Gold:
            return _settings.instanceResetGold ? Currency::Gold : Currency::Free;
        default:
            return Currency::Free;
    }
}

std::string GoldPrice(uint32 gold)
{
    return gold ? std::to_string(gold) + " Gold" : std::string();
}

std::string InstanceResetPrice()
{
    switch (InstanceResetCurrency())
    {
        case Currency::VotePoints:
            return std::to_string(_settings.instanceResetVotePoints) + " VP";
        case Currency::Gold:
            return GoldPrice(_settings.instanceResetGold);
        default:
            return std::string();
    }
}

bool TryChargeGold(Player* player, uint32 gold)
{
    if (!gold)
        return true;

    int64 const copper = int64(gold) * CopperPerGold;
    if (int64(player->GetMoney()) < copper)
    {
        ChatHandler(player->GetSession()).SendSysMessage("You do not have enough gold.");
        return false;
    }

    player->ModifyMoney(-copper);
    return true;
}
}
