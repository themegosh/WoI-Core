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
#include "Creature.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "SpellHistory.h"
#include "WoI.h"
#include "WorldSession.h"

struct tele_npc_vote : public ScriptedAI
{
    tele_npc_vote(Creature* creature) : ScriptedAI(creature) { }

    bool OnGossipHello(Player* player) override
    {
        bool votecheck = WoI::Vote::HasVoted(player);
        if (!votecheck)
            ChatHandler(player->GetSession()).SendSysMessage(WOI_STRING_TELE_VOTE_CHECK_FAILED);

        // remove stealth
        if (player->HasAura(1784))
            player->RemoveAura(1784);
        // remove vanish
        if (player->HasAura(26889))
            player->RemoveAura(26889);
        // remove prowl
        if (player->HasAura(5215))
            player->RemoveAura(5215);


        // Main Menu for Alliance
        if (player->GetTeam() == ALLIANCE)
        {
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Alliance Town ->", GOSSIP_SENDER_MAIN, 1000);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Neutral Town ->", GOSSIP_SENDER_MAIN, 3000);
            if (votecheck)
            {
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "Dungeons ->", GOSSIP_SENDER_MAIN, 5000);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "Player Tools ->", GOSSIP_SENDER_MAIN, 4000);
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Alliance vs Horde PvP", GOSSIP_SENDER_MAIN, 3500);
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Guild House Menu", GOSSIP_SENDER_MAIN, 10010);
            }
        }
        else // Main Menu for Horde
        {
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Horde Town ->", GOSSIP_SENDER_MAIN, 2000);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Neutral Town ->", GOSSIP_SENDER_MAIN, 3000);
            if (votecheck)
            {
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "Dungeons ->", GOSSIP_SENDER_MAIN, 5000);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "Player Tools ->", GOSSIP_SENDER_MAIN, 4000);
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Alliance vs Horde PvP", GOSSIP_SENDER_MAIN, 3500);
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Guild House Menu", GOSSIP_SENDER_MAIN, 10010);
            }
        }

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());

    return true;
    }

    void SendDefaultMenu(Player* player, uint32 action)
    {
    bool votecheck = WoI::Vote::HasVoted(player);
    // Not allow in combat
    if (player->IsInCombat())
    {
        CloseGossipMenuFor(player);
        me->Say("You are in combat!", LANG_UNIVERSAL);
        return;
    }

    switch(action)
    {

    case 1000: //Alliance Town
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Darnassus", GOSSIP_SENDER_MAIN, 1001);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Exodar", GOSSIP_SENDER_MAIN, 1005);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Ironforge", GOSSIP_SENDER_MAIN, 1010);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Stormwind", GOSSIP_SENDER_MAIN, 1015);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 2000: //Horde Town
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Orgrimmar", GOSSIP_SENDER_MAIN, 2001);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Silvermoon", GOSSIP_SENDER_MAIN, 2005);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Thunder Bluff", GOSSIP_SENDER_MAIN, 2010);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Undercity", GOSSIP_SENDER_MAIN, 2015);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 3000: //Neutral Town
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Booty Bay", GOSSIP_SENDER_MAIN, 3001);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Dalaran", GOSSIP_SENDER_MAIN, 3005);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Everlook", GOSSIP_SENDER_MAIN, 3010);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Gadgetzan", GOSSIP_SENDER_MAIN, 3015);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Mudsprocket", GOSSIP_SENDER_MAIN, 3020);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Ratchet", GOSSIP_SENDER_MAIN, 3025);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Shattrath City", GOSSIP_SENDER_MAIN, 3030);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Isle Of Quel'Danas", GOSSIP_SENDER_MAIN, 3035);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 5000: //Dungeons
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Kalimdor ->", GOSSIP_SENDER_MAIN, 5010);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Eastern Kingdoms ->", GOSSIP_SENDER_MAIN, 5015);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Outland ->", GOSSIP_SENDER_MAIN, 5025);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Northrend ->", GOSSIP_SENDER_MAIN, 5030);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 5005: //Back To Main Menu
        // Main Menu for Alliance
        if (player->GetTeam() == ALLIANCE)
        {
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Alliance Town ->", GOSSIP_SENDER_MAIN, 1000);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Neutral Town ->", GOSSIP_SENDER_MAIN, 3000);
            if (votecheck)
            {
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "Dungeons ->", GOSSIP_SENDER_MAIN, 5000);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "Player Tools ->", GOSSIP_SENDER_MAIN, 4000);
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Alliance vs Horde PvP", GOSSIP_SENDER_MAIN, 3500);
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Take me to my Guild House", GOSSIP_SENDER_MAIN, 10010);
            }
        }
        else // Main Menu for Horde
        {
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Horde Town ->", GOSSIP_SENDER_MAIN, 2000);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "Neutral Town ->", GOSSIP_SENDER_MAIN, 3000);
            if (votecheck)
            {
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "Dungeons ->", GOSSIP_SENDER_MAIN, 5000);
                AddGossipItemFor(player, GOSSIP_ICON_TALK, "Player Tools ->", GOSSIP_SENDER_MAIN, 4000);
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Alliance vs Horde PvP", GOSSIP_SENDER_MAIN, 3500);
                AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Take me to my Guild House", GOSSIP_SENDER_MAIN, 10010);
            }
        }

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 5010: //Kalimdor
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Blackfathom Deeps", GOSSIP_SENDER_MAIN, 6001);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Caverns of Time", GOSSIP_SENDER_MAIN, 6005);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Dire Maul", GOSSIP_SENDER_MAIN, 6010);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Maraudon", GOSSIP_SENDER_MAIN, 6015);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Onyxia's Lair", GOSSIP_SENDER_MAIN, 6020);
        if (player->GetTeam() == HORDE)
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Ragefire Chasm", GOSSIP_SENDER_MAIN, 6025);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Razorfen Downs", GOSSIP_SENDER_MAIN, 6030);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Razorfen Kraul", GOSSIP_SENDER_MAIN, 6035);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Ruins of Ahn'Qiraj", GOSSIP_SENDER_MAIN, 6040);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Temple of Ahn'Qiraj", GOSSIP_SENDER_MAIN, 6045);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Wailing Caverns", GOSSIP_SENDER_MAIN, 6050);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Zul'Farrak", GOSSIP_SENDER_MAIN, 6055);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN, 5000);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 5015: //Eastern Kingdoms 1
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Blackrock Depths", GOSSIP_SENDER_MAIN, 7001);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Blackrock Spire", GOSSIP_SENDER_MAIN, 7005);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Blackwing Lair", GOSSIP_SENDER_MAIN, 7010);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Deadmines", GOSSIP_SENDER_MAIN, 7015);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Gnomeregan", GOSSIP_SENDER_MAIN, 7020);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Isle Of Quel'Danas", GOSSIP_SENDER_MAIN, 7025);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Karazhan", GOSSIP_SENDER_MAIN, 7030);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Molten Core", GOSSIP_SENDER_MAIN, 7035);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "More ->", GOSSIP_SENDER_MAIN, 5020);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN, 5000);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 5020: //Eastern Kingdoms 2
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Scarlet Monastery", GOSSIP_SENDER_MAIN, 7040);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Scholomance", GOSSIP_SENDER_MAIN, 7045);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Shadowfang Keep", GOSSIP_SENDER_MAIN, 7050);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Stratholme", GOSSIP_SENDER_MAIN, 7055);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Sunken Temple", GOSSIP_SENDER_MAIN, 7060);
        if (player->GetTeam() == ALLIANCE)
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "The Stockade", GOSSIP_SENDER_MAIN, 7065);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Uldaman", GOSSIP_SENDER_MAIN, 7070);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Zul'Aman", GOSSIP_SENDER_MAIN, 7075);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Zul'Gurub", GOSSIP_SENDER_MAIN, 7080);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN, 5015);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 5025: //Outland
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Auchindoun", GOSSIP_SENDER_MAIN, 8001);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Black Temple", GOSSIP_SENDER_MAIN, 8005);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Coilfang Reservoir", GOSSIP_SENDER_MAIN, 8010);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Gruul's Lair", GOSSIP_SENDER_MAIN, 8015);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Hellfire Citadel", GOSSIP_SENDER_MAIN, 8020);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Tempest Keep", GOSSIP_SENDER_MAIN, 8025);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN, 5000);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 5030: //Northrend
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Ahn'Kahet", GOSSIP_SENDER_MAIN, 9001);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Azjol-Nerub", GOSSIP_SENDER_MAIN, 9005);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Drak'Tharon Keep", GOSSIP_SENDER_MAIN, 9010);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Gun'Drak", GOSSIP_SENDER_MAIN, 9015);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Icecrown Citadel", GOSSIP_SENDER_MAIN, 9020);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Naxxramas", GOSSIP_SENDER_MAIN, 9025);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "The Nexus", GOSSIP_SENDER_MAIN, 9030);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "The Obsidian Sanctum", GOSSIP_SENDER_MAIN, 9035);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Utgarde Keep", GOSSIP_SENDER_MAIN, 9040);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Utgarde Pinnacle", GOSSIP_SENDER_MAIN, 9045);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Ulduar", GOSSIP_SENDER_MAIN, 9050);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Violet Hold", GOSSIP_SENDER_MAIN, 9055);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Vault of Archavon", GOSSIP_SENDER_MAIN, 9060);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- Back", GOSSIP_SENDER_MAIN, 5000);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 4000: // Player Tools
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Heal Me!", GOSSIP_SENDER_MAIN, 10030);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Reset Talent Points", GOSSIP_SENDER_MAIN, 4010);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Reset Pet Talent Points", GOSSIP_SENDER_MAIN, 4015);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Remove Resurrect Sickness", GOSSIP_SENDER_MAIN, 4020);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Reset Instance Locks Cost: [2 VP]", GOSSIP_SENDER_MAIN, 10015);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Reset Cooldowns Cost: [100 Gold]", GOSSIP_SENDER_MAIN, 10025);
        AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- [Main Menu]", GOSSIP_SENDER_MAIN, 5005);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 10010: // Guild House Menu
        if (WoI::GuildHouse::GetPhaseMask(player) > 1)
        {
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Guild Hall", GOSSIP_SENDER_MAIN, 10035);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Guild Arena", GOSSIP_SENDER_MAIN, 10040);
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Summit Course", GOSSIP_SENDER_MAIN, 10045);
            AddGossipItemFor(player, GOSSIP_ICON_TALK, "<- [Main Menu]", GOSSIP_SENDER_MAIN, 5005);

            SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        }
        else
        {
            CloseGossipMenuFor(player);
            ChatHandler(player->GetSession()).SendSysMessage(WOI_STRING_NO_GUILD_HOUSE);
        }
    break;

    //////////////////////////////////////////////////ALLIANCE///////////////////////////////////////////////////////////////

    case 1001: // Darnassus
        CloseGossipMenuFor(player);
        player->TeleportTo(1, 9947.52f, 2482.73f, 1316.21f, 0.0f);

    break;

    case 1005: // Exodar
        CloseGossipMenuFor(player);
        player->TeleportTo(530, -3954.20f, -11656.54f, -138.69f, 0.0f);

    break;

    case 1010: // Ironforge
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -4924.07f, -951.95f, 501.55f, 5.40f);

    break;

    case 1015: // Stormwind
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -8960.14f, 516.266f, 96.3568f, 0.0f);

    break;

    //////////////////////////////////////////////////HORDE///////////////////////////////////////////////////////////////

    case 2001: // Orgrimmar
        CloseGossipMenuFor(player);
        player->TeleportTo(1, 1552.5f, -4420.66f, 8.94802f, 0.0f);

    break;

    case 2005: // Silvermoon
        CloseGossipMenuFor(player);
        player->TeleportTo(530, 9338.74f, -7277.27f, 13.7895f, 0.0f);

    break;

    case 2010: // Thunder Bluff
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -1290.0f, 147.033997f, 129.682007f, 4.919000f);

    break;

    case 2015: // Undercity
        CloseGossipMenuFor(player);
        player->TeleportTo(0, 1819.71f, 238.79f, 60.5321f, 0.0f);

    break;

    //////////////////////////////////////////////////NEUTRAL///////////////////////////////////////////////////////////////

    case 3001:// Booty Bay
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -14438.2f, 472.22f, 15.32f, 0.65f);

    break;

    case 3005: //Dalaran
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 5819.08f, 643.99f, 647.80f, 3.32f);

    break;

    case 3010: //Everlook
        CloseGossipMenuFor(player);
        player->TeleportTo(1, 6724.77f, -4610.68f, 720.78f, 4.78f);

    break;

    case 3015: //Gadgetzan
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -7173.26f, -3785.60f, 8.37f, 6.13f);

    break;

    case 3020: //Mudsprocket
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -4564.79f, -3172.38f, 33.93f, 3.21f);

    break;

    case 3025: //Ratchet
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -956.664f, -3754.71f, 5.33239f, 0.996637f);

    break;

    case 3030:// Shattrath City
        CloseGossipMenuFor(player);
        player->TeleportTo(530, -1850.209961f, 5435.821777f, -10.961435f, 3.403913f);

    break;

    case 3035:// Isle Of Quel'Danas
        CloseGossipMenuFor(player);
        player->TeleportTo(530, 12947.4f,-6893.31f,5.68398f,3.09154f);

    break;

    //////////////////////////////////////////////////KALIMDOR///////////////////////////////////////////////////////////////

    case 6001:// Blackfathom Deeps
        CloseGossipMenuFor(player);
        player->TeleportTo(1, 4248.72f, 744.35f, -24.67f, 1.34f);

    break;

    case 6005:// Caverns of Time
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -8173.66f, -4746.36f, 33.84f, 4.94f);

    break;

    case 6010:// Dire Maul
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -3960.95f, 1130.64f, 161.05f, 0.0f);

    break;

    case 6015:// Maraudon
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -1431.33f, 2962.34f, 98.23f, 4.74f);

    break;

    case 6020:// Onyxia's Lair
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -4707.44f, -3726.82f, 54.6723f, 3.8f);

    break;

    case 6025:// Ragefire Chasm
        CloseGossipMenuFor(player);
        player->TeleportTo(1, 1814.47f, -4419.46f, -18.78f, 5.28f);

    break;

    case 6030:// Razorfen Downs
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -4657.88f, -2525.59f, 81.4f, 4.16f);

    break;

    case 6035:// Razorfen Kraul
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -4463.6f, -1664.53f, 82.26f, 0.85f);

    break;

    case 6040:// Ruins of Ahn'Qiraj
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -8413.33f, 1501.27f, 29.64f, 2.61f);

    break;

    case 6045:// Temple of Ahn'Qiraj
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -8245.837891f, 1983.736206f, 129.071686f, 0.936195f);

    break;

    case 6050:// Wailing Caverns
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -722.53f,-2226.30f,16.94f,2.71f);

    break;

    case 6055:// Zul'Farrak
        CloseGossipMenuFor(player);
        player->TeleportTo(1, -6801.9f, -2890.22f, 8.88f, 6.25f);

    break;

    //////////////////////////////////////////////////EASTERN KINGDOMS///////////////////////////////////////////////////////////////

    case 7001:// Blackrock Depths
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -7180.57f, -920.04f, 165.49f, 5.02f);

    break;

    case 7005:// Blackrock Spire
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -7526.77f, -1225.64f, 285.73f, 5.31f);

    break;

    case 7010:// Blackwing Lair
        CloseGossipMenuFor(player);
        player->TeleportTo(469, -7672.61f, -1107.21f, 396.65f, 3.75f);

    break;

    case 7015:// Deadmines
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -11208.2f, 1675.92f, 24.57f, 1.48f);

    break;

    case 7020:// Gnomeregan
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -5163.32f, 927.18f, 257.158f, 1.44f);

    break;

    case 7025:// Isle Of Quel'Danas
        CloseGossipMenuFor(player);
        player->TeleportTo(530, 13010.00f, -6907.64f, 9.58f, 4.58f);

    break;

    case 7030:// Karazhan
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -11119.6f, -2011.42f, 47.09f, 0.65f);

    break;

    case 7035:// Molten Core
        CloseGossipMenuFor(player);
        player->TeleportTo(230, 1114.85f, -457.76f, -102.81f, 3.83f);

    break;

    case 7040:// Scarlet Monastery
        CloseGossipMenuFor(player);
        player->TeleportTo(0, 2843.89f,-693.74f,139.32f,5.11f);

    break;

    case 7045:// Scholomance
        CloseGossipMenuFor(player);
        player->TeleportTo(0, 1273.06f, -2574.01f, 92.66f, 2.06f);

    break;

    case 7050:// Shadowfang Keep
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -239.54f, 1550.8f, 76.89f, 1.18f);

    break;

    case 7055:// Stratholme
        CloseGossipMenuFor(player);
        player->TeleportTo(0, 3370.76f, -3343.63f, 142.26f, 5.23f);

    break;

    case 7060:// Sunken Temple
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -10346.92f, -3851.90f, -43.41f, 6.09f);

    break;

    case 7065:// The Stockade
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -8766.89f, 844.6f, 88.43f, 0.69f);

    break;

    case 7070:// Uldaman
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -6070.72f, -2955.33f, 209.78f, 0.05f);

    break;

    case 7075:// Zul'Aman
        CloseGossipMenuFor(player);
        player->TeleportTo(530, 6851.09f, -7979.71f, 183.54f, 4.72f);

    break;

    case 7080:// Zul'Gurub
        CloseGossipMenuFor(player);
        player->TeleportTo(0, -11916.4f, -1216.23f, 92.28f, 4.75f);

    break;

    //////////////////////////////////////////////////OUTLAND///////////////////////////////////////////////////////////////

    case 8001:// Auchindoun
        CloseGossipMenuFor(player);
        player->TeleportTo(530, -3322.92f, 4931.02f, -100.56f, 1.86f);

    break;

    case 8005:// Black Temple
        CloseGossipMenuFor(player);
        player->TeleportTo(530, -3649.1f, 317.33f, 35.19f, 2.97f);

    break;

    case 8010:// Coilfang Reservoir
        CloseGossipMenuFor(player);
        player->TeleportTo(530, 721.08f, 6846.77f, -68.75f, 0.34f);

    break;

    case 8015:// Gruul's Lair
        CloseGossipMenuFor(player);
        player->TeleportTo(530, 3539.01f, 5082.36f, 1.69f, 0.0f);

    break;

    case 8020:// Hellfire Citadel
        CloseGossipMenuFor(player);
        player->TeleportTo(530, -292.71f, 3146.77f, 31.60f, 2.05f);

    break;

    case 8025:// Tempest Keep
        CloseGossipMenuFor(player);
        player->TeleportTo(530, 3087.62f, 1376.27f, 184.8f, 4.63f);

    break;

    //////////////////////////////////////////////////NORTHREND///////////////////////////////////////////////////////////////

    case 9001:// Ahn'Kahet
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 3646.76f, 2045.17f, 1.79f, 4.37f);

    break;

    case 9005:// Azjol-Nerub
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 3675.43f, 2169.00f, 35.90f, 2.29f);

    break;

    case 9010:// Drak'Tharon Keep
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 4450.86f, -2045.25f, 162.83f, 0.00f);

    break;

    case 9015:// Gun'Drak
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 6937.54f, -4455.98f, 450.68f, 1.00f);

    break;

    case 9020:// Icecrown Citadel
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 6139.24f, 2233.72f, 510.79f, 3.56f);

    break;

    case 9025:// Naxxramas
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 3668.72f, -1262.46f, 243.622f, 4.785f);

    break;

    case 9030:// The Nexus
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 3789.95f, 6961.48f, 104.23f, 0.28f);

    break;

    case 9035:// The Obsidian Sanctum
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 3625.78f, 280.4f, -120.14f, 3.25f);

    break;

    case 9040:// Ulduar
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 1219.72f, -4865.28f, 41.25f, 0.31f);

    break;

    case 9045:// Utgarde Keep
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 1245.69f, -4856.59f, 216.86f, 3.45f);

    break;

    case 9050:// Utgarde Pinnacle
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 8976.24f, -1281.33f, 1059.01f, 0.58f);

    break;

    case 9055:// Violet Hold
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 5679.82f, 486.80f, 652.40f, 4.08f);

    break;

    case 9060:// Vault of Archavon
        CloseGossipMenuFor(player);
        player->TeleportTo(571, 5440.11f, 2840.76f, 420.42f, 0.01f);

    break;

    //////////////////////////////////////////Player Tools////////////////////////////////////////////////////////

    case 4005:// Innkeeper Bind your new Home
        CloseGossipMenuFor(player);
        player->SetBindPoint(me->GetGUID());
    break;

    case 4010:// Reset Talent Points
        CloseGossipMenuFor(player);
        player->SendRespecWipeConfirm(me->GetGUID(), player->GetNextResetTalentsCost());
    break;

    case 4015:// Reset Pet Talent Points
        CloseGossipMenuFor(player);
        if(player->GetPet())
            player->ResetPetTalents();
        else
            me->Whisper("You don't have a pet.", LANG_UNIVERSAL, player);
    break;

    case 4020://Remove Res Sickness
        if(!player->HasAura(15007))
        {
            me->Whisper("You don't have resurrection sickness.", LANG_UNIVERSAL, player);
            OnGossipHello(player);
            return;
        }

        me->CastSpell(player, 38588, false); // Healing effect
        player->RemoveAurasDueToSpell(15007);
        CloseGossipMenuFor(player);
    break;

    //////////////////////////////////////////TOOLS////////////////////////////////////////////////////////

    case 3500:// Alliance vs Horde PvP
        CloseGossipMenuFor(player);
        if (player->GetTeam() == ALLIANCE) // Razor Hill PvP For Alliance
            player->TeleportTo(1, 315.721f, -4743.4f, 10.4867f, 0.0f);
        else // Goldshire PvP For Horde
            player->TeleportTo(0, -9464.0f, 62.0f, 56.0f, 0.0f);
    break;

    case 10015: // Player Tools
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "Yes, at a Cost of 2 VP.", GOSSIP_SENDER_MAIN, 10020);
        AddGossipItemFor(player, GOSSIP_ICON_INTERACT_2, "No, Return to Main Menu.", GOSSIP_SENDER_MAIN, 5005);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
    break;

    case 10020: // Player Tools
        CloseGossipMenuFor(player);
        WoI::Vote::UnbindAllInstances(player);
    break;

    case 10025:
        if (player->GetMoney() < 1000000)
        {
            ChatHandler(player->GetSession()).SendSysMessage("You do not have enough gold.");
        }
        else
        {
            player->ModifyMoney(-1000000);
            player->GetSpellHistory()->ResetAllCooldowns();
            ChatHandler(player->GetSession()).SendSysMessage("Your cooldowns have been reset.");
        }
        CloseGossipMenuFor(player);
    break;

    case 10030:
        if (!player->IsInCombat())
            player->ResurrectPlayer(1.0f);
        else
            ChatHandler(player->GetSession()).SendSysMessage("You cannot heal in combat.");
        CloseGossipMenuFor(player);
    break;

    case 10035:// Guild Hall
        CloseGossipMenuFor(player);
        player->TeleportTo(37, 1009.9f, 284.2f, 328.96f, 0.17f);
    break;

    case 10040:// Guild Arena
        CloseGossipMenuFor(player);
        player->TeleportTo(37, 779.7f, 152.2f, 269.5f, 1.770f);
    break;

    case 10045:// Summit Course
        CloseGossipMenuFor(player);
        player->TeleportTo(37, 486.89f, 58.80f, 262.38f, 3.875f);
    break;

    break;

    }
    }

    bool OnGossipSelect(Player* player, uint32 /*menuId*/, uint32 gossipListId) override
    {
        uint32 const action = GetGossipActionFor(player, gossipListId);
        ClearGossipMenuFor(player);
        SendDefaultMenu(player, action);
        return true;
    }
};

void AddSC_tele_npc_vote()
{
    RegisterCreatureAI(tele_npc_vote);
}
