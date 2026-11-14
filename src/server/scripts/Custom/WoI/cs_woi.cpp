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

#include "ScriptMgr.h"
#include "Chat.h"
#include "ChatCommand.h"
#include "Language.h"
#include "Player.h"
#include "RBAC.h"
#include "WoI.h"
#include "WorldSession.h"
#include <algorithm>

using namespace Trinity::ChatCommands;

namespace
{
    // Custom permission ids (1000+); see sql/custom/auth/woi_rbac.sql.
    constexpr rbac::RBACPermissions WOI_PERM_VP_SHOW   = rbac::RBACPermissions(1001);
    constexpr rbac::RBACPermissions WOI_PERM_VP_MODIFY = rbac::RBACPermissions(1002);
    constexpr rbac::RBACPermissions WOI_PERM_VP_CHECK  = rbac::RBACPermissions(1003);
    constexpr rbac::RBACPermissions WOI_PERM_VP_FORCE  = rbac::RBACPermissions(1004);
    constexpr rbac::RBACPermissions WOI_PERM_HEAL      = rbac::RBACPermissions(1005);
}

class woi_commandscript : public CommandScript
{
public:
    woi_commandscript() : CommandScript("woi_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable votePanelCommandTable =
        {
            { "show",   HandleShowVotePointsCommand,   WOI_PERM_VP_SHOW,   Console::No },
            { "modify", HandleModifyVotePointsCommand, WOI_PERM_VP_MODIFY, Console::Yes },
            { "check",  HandleCheckHasVotedCommand,    WOI_PERM_VP_CHECK,  Console::Yes },
            { "force",  HandleForceVoteCommand,        WOI_PERM_VP_FORCE,  Console::Yes },
        };

        static ChatCommandTable commandTable =
        {
            { "vp",   votePanelCommandTable },
            { "heal", HandleHealCommand, WOI_PERM_HEAL, Console::Yes },
        };

        return commandTable;
    }

    static bool HandleShowVotePointsCommand(ChatHandler* handler, Optional<PlayerIdentifier> target)
    {
        // Regular players always see their own balance, staff may look up another online player.
        if (handler->GetSession()->GetSecurity() < SEC_MODERATOR)
        {
            int32 const points = WoI::Vote::GetPoints(handler->GetSession()->GetPlayer());
            if (points == -1)
                handler->SendSysMessage(WOI_STRING_NEVER_VOTED);
            else
                handler->PSendSysMessage(WOI_STRING_SHOW_VOTE_POINTS, points);
            return true;
        }

        if (!target)
            target = PlayerIdentifier::FromTarget(handler);

        if (!target || !target->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_EXIST_OR_OFFLINE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = target->GetConnectedPlayer();
        handler->PSendSysMessage(WOI_STRING_SHOW_VOTE_POINTS_PLAYER, player->GetName().c_str(), std::max(WoI::Vote::GetPoints(player), 0));
        return true;
    }

    static bool HandleModifyVotePointsCommand(ChatHandler* handler, PlayerIdentifier target, int32 amount)
    {
        if (!target.IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = target.GetConnectedPlayer();
        int32 const current = WoI::Vote::GetPoints(player);
        if (current == -1)
        {
            handler->SendSysMessage("The character you are modifying vote points for has not voted yet.");
            handler->SetSentErrorMessage(true);
            return false;
        }

        handler->PSendSysMessage(WOI_STRING_CHANGED_VOTE_POINTS, player->GetName().c_str(), current, std::max(current + amount, 0));
        WoI::Vote::ModPoints(player, amount);
        return true;
    }

    static bool HandleCheckHasVotedCommand(ChatHandler* handler, PlayerIdentifier target)
    {
        if (!target.IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = target.GetConnectedPlayer();
        handler->PSendSysMessage(WoI::Vote::HasVoted(player) ? WOI_STRING_HAS_VOTED : WOI_STRING_HAS_NOT_VOTED, player->GetName().c_str());
        return true;
    }

    static bool HandleForceVoteCommand(ChatHandler* handler, PlayerIdentifier target)
    {
        if (!target.IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = target.GetConnectedPlayer();
        WoI::Vote::ForceVote(player);
        ChatHandler(player->GetSession()).SendSysMessage(WOI_STRING_FORCED_YOUR_VOTE);
        handler->PSendSysMessage(WOI_STRING_FORCED_VOTE, player->GetName().c_str());
        return true;
    }

    static bool HandleHealCommand(ChatHandler* handler, Optional<PlayerIdentifier> target)
    {
        if (!target)
            target = PlayerIdentifier::FromTargetOrSelf(handler);

        if (!target || !target->IsConnected())
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        Player* player = target->GetConnectedPlayer();
        player->ResurrectPlayer(1.0f);
        handler->PSendSysMessage(WOI_STRING_HEAL_PLAYER, handler->GetNameLink(player).c_str());
        return true;
    }
};

void AddSC_cs_woi()
{
    new woi_commandscript();
}
