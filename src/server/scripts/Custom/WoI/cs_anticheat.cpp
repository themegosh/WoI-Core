/*
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

#include "ScriptMgr.h"
#include "AnticheatMgr.h"
#include "Chat.h"
#include "Language.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "RBAC.h"
#include "WorldSession.h"

using namespace Trinity::ChatCommands;

class anticheat_commandscript : public CommandScript
{
public:
    anticheat_commandscript() : CommandScript("anticheat_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable anticheatCommandTable =
        {
            { "global", HandleAntiCheatGlobalCommand, rbac::RBAC_PERM_COMMAND_GM,             Console::Yes },
            { "player", HandleAntiCheatPlayerCommand, rbac::RBAC_PERM_COMMAND_GM,             Console::Yes },
            { "delete", HandleAntiCheatDeleteCommand, rbac::RBAC_PERM_COMMAND_RELOAD_CONFIG,  Console::Yes },
            { "handle", HandleAntiCheatHandleCommand, rbac::RBAC_PERM_COMMAND_RELOAD_CONFIG,  Console::Yes },
            { "jail",   HandleAnticheatJailCommand,   rbac::RBAC_PERM_COMMAND_GM,             Console::No },
            { "warn",   HandleAnticheatWarnCommand,   rbac::RBAC_PERM_COMMAND_GM,             Console::Yes },
        };

        static ChatCommandTable commandTable =
        {
            { "anticheat", anticheatCommandTable },
        };

        return commandTable;
    }

    static bool HandleAnticheatWarnCommand(ChatHandler* handler, Optional<PlayerIdentifier> target)
    {
        if (!sAnticheatMgr->IsEnabled())
            return false;

        if (!target)
            target = PlayerIdentifier::FromTarget(handler);

        if (!target || !target->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        ChatHandler(target->GetConnectedPlayer()->GetSession()).SendSysMessage("The anticheat system has reported several times that you may be cheating. You will be monitored to confirm if this is accurate.");
        return true;
    }

    static bool HandleAnticheatJailCommand(ChatHandler* handler, Optional<PlayerIdentifier> target)
    {
        if (!sAnticheatMgr->IsEnabled())
            return false;

        if (!target)
            target = PlayerIdentifier::FromTarget(handler);

        if (!target || !target->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = target->GetConnectedPlayer();
        if (player == handler->GetPlayer())
            return false;

        WorldLocation const jail(1, 16226.5f, 16403.6f, -64.5f, 3.2f);

        // both the target and the GM are sent to the jail
        player->TeleportTo(jail);
        handler->GetPlayer()->TeleportTo(jail);
        player->SetHomebind(jail, 876);
        return true;
    }

    static bool HandleAntiCheatDeleteCommand(ChatHandler* handler, std::string const& arg)
    {
        if (!sAnticheatMgr->IsEnabled())
            return false;

        if (arg == "deleteall")
        {
            sAnticheatMgr->AnticheatDeleteCommand(0);
            return true;
        }

        std::string name = arg;
        normalizePlayerName(name);
        Player* player = ObjectAccessor::FindPlayerByName(name);
        if (!player)
        {
            handler->SendSysMessage("Player doesn't exist");
            return true;
        }

        sAnticheatMgr->AnticheatDeleteCommand(player->GetGUID().GetCounter());
        return true;
    }

    static bool HandleAntiCheatPlayerCommand(ChatHandler* handler, Optional<PlayerIdentifier> target)
    {
        if (!sAnticheatMgr->IsEnabled())
            return false;

        if (!target)
            target = PlayerIdentifier::FromTarget(handler);

        if (!target || !target->IsConnected())
        {
            handler->SendSysMessage("There is no player.");
            return true;
        }

        ObjectGuid::LowType guid = target->GetGUID().GetCounter();

        handler->PSendSysMessage("Information about player %s", target->GetName().c_str());
        handler->PSendSysMessage("Average: %f || Total Reports: %u ", sAnticheatMgr->GetAverage(guid), sAnticheatMgr->GetTotalReports(guid));
        handler->PSendSysMessage("Speed Reports: %u || Fly Reports: %u || Jump Reports: %u ",
            sAnticheatMgr->GetTypeReports(guid, SPEED_HACK_REPORT), sAnticheatMgr->GetTypeReports(guid, FLY_HACK_REPORT), sAnticheatMgr->GetTypeReports(guid, JUMP_HACK_REPORT));
        handler->PSendSysMessage("Walk On Water Reports: %u  || Teleport To Plane Reports: %u",
            sAnticheatMgr->GetTypeReports(guid, WALK_WATER_HACK_REPORT), sAnticheatMgr->GetTypeReports(guid, TELEPORT_PLANE_HACK_REPORT));
        handler->PSendSysMessage("Climb Reports: %u", sAnticheatMgr->GetTypeReports(guid, CLIMB_HACK_REPORT));
        return true;
    }

    static bool HandleAntiCheatHandleCommand(ChatHandler* handler, std::string const& mode)
    {
        if (mode == "on")
        {
            sAnticheatMgr->SetEnabled(true);
            handler->SendSysMessage("The Anticheat System is now: Enabled!");
        }
        else if (mode == "off")
        {
            sAnticheatMgr->SetEnabled(false);
            handler->SendSysMessage("The Anticheat System is now: Disabled!");
        }

        return true;
    }

    static bool HandleAntiCheatGlobalCommand(ChatHandler* handler)
    {
        if (!sAnticheatMgr->IsEnabled())
        {
            handler->SendSysMessage("The Anticheat System is disabled.");
            return true;
        }

        sAnticheatMgr->AnticheatGlobalCommand(handler);
        return true;
    }
};

void AddSC_cs_anticheat()
{
    new anticheat_commandscript();
}
