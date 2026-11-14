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

#include "AnticheatMgr.h"
#include "AnticheatScripts.h"
#include "Chat.h"
#include "Config.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "Opcodes.h"
#include "Player.h"
#include "SpellAuraDefines.h"
#include "Timer.h"
#include "UnitDefines.h"
#include "World.h"
#include "WorldPacket.h"
#include <cmath>

namespace
{
    constexpr float CLIMB_ANGLE = 1.9f;
    constexpr uint32 TEMP_REPORTS_WINDOW_MS = 3000;
    constexpr uint32 TEMP_REPORTS_REQUIRED = 3;
    constexpr uint32 DEEPRUN_TRAM_MAP_ID = 369;
}

AnticheatMgr* AnticheatMgr::instance()
{
    static AnticheatMgr instance;
    return &instance;
}

void AnticheatMgr::StartScripts()
{
    new AnticheatPlayerScript();
    new AnticheatWorldScript();
    LoadConfig();
}

void AnticheatMgr::LoadConfig()
{
    _enabled = sConfigMgr->GetBoolDefault("Anticheat.Enable", false);
    _reportsForIngameWarnings = static_cast<uint32>(sConfigMgr->GetIntDefault("Anticheat.ReportsForIngameWarnings", 70));
    _detectionsEnabled = static_cast<uint32>(sConfigMgr->GetIntDefault("Anticheat.DetectionsEnabled", 31));
}

void AnticheatMgr::StartHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode)
{
    if (!_enabled)
        return;

    if (player->IsGameMaster())
        return;

    AnticheatData& data = _players[player->GetGUID().GetCounter()];

    if (!player->IsInFlight() && !player->GetTransport() && !player->GetVehicle())
    {
        SpeedHackDetection(player, movementInfo, data);
        FlyHackDetection(player, data);
        WalkOnWaterHackDetection(player, data);
        JumpHackDetection(player, opcode, data);
        TeleportPlaneHackDetection(player, movementInfo, data);
        ClimbHackDetection(player, movementInfo, opcode, data);
    }

    data.lastMovementInfo = movementInfo;
    data.lastOpcode = opcode;
}

void AnticheatMgr::JumpHackDetection(Player* player, uint32 opcode, AnticheatData& data)
{
    if (!IsDetectionEnabled(JUMP_HACK_DETECTION))
        return;

    if (data.lastOpcode == MSG_MOVE_JUMP && opcode == MSG_MOVE_JUMP)
    {
        BuildReport(player, JUMP_HACK_REPORT, data);
        TC_LOG_ERROR("anticheat", "AnticheatMgr:: Jump-Hack detected player {}", player->GetGUID().ToString());
    }
}

void AnticheatMgr::WalkOnWaterHackDetection(Player* player, AnticheatData& data)
{
    if (!IsDetectionEnabled(WALK_WATER_HACK_DETECTION))
        return;

    if (!data.lastMovementInfo.HasMovementFlag(MOVEMENTFLAG_WATERWALKING))
        return;

    // ghosts can walk on water
    if (!player->IsAlive())
        return;

    if (player->HasAuraType(SPELL_AURA_FEATHER_FALL) ||
        player->HasAuraType(SPELL_AURA_SAFE_FALL) ||
        player->HasAuraType(SPELL_AURA_WATER_WALK))
        return;

    TC_LOG_ERROR("anticheat", "AnticheatMgr:: Walk on Water - Hack detected player {}", player->GetGUID().ToString());
    BuildReport(player, WALK_WATER_HACK_REPORT, data);
}

void AnticheatMgr::FlyHackDetection(Player* player, AnticheatData& data)
{
    if (!IsDetectionEnabled(FLY_HACK_DETECTION))
        return;

    if (!data.lastMovementInfo.HasMovementFlag(MOVEMENTFLAG_FLYING))
        return;

    if (player->HasAuraType(SPELL_AURA_FLY) ||
        player->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED) ||
        player->HasAuraType(SPELL_AURA_MOD_INCREASE_FLIGHT_SPEED))
        return;

    TC_LOG_ERROR("anticheat", "AnticheatMgr:: Fly-Hack detected player {}", player->GetGUID().ToString());
    BuildReport(player, FLY_HACK_REPORT, data);
}

void AnticheatMgr::TeleportPlaneHackDetection(Player* player, MovementInfo const& movementInfo, AnticheatData& data)
{
    if (!IsDetectionEnabled(TELEPORT_PLANE_HACK_DETECTION))
        return;

    if (data.lastMovementInfo.pos.GetPositionZ() != 0.0f || movementInfo.pos.GetPositionZ() != 0.0f)
        return;

    if (movementInfo.HasMovementFlag(MOVEMENTFLAG_FALLING))
        return;

    float x, y, z;
    player->GetPosition(x, y, z);
    float groundZ = player->GetMap()->GetHeight(x, y, z);

    // we are not really walking there
    if (std::fabs(groundZ - z) > 1.0f)
    {
        TC_LOG_ERROR("anticheat", "AnticheatMgr:: Teleport To Plane - Hack detected player {}", player->GetGUID().ToString());
        BuildReport(player, TELEPORT_PLANE_HACK_REPORT, data);
    }
}

void AnticheatMgr::ClimbHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode, AnticheatData& data)
{
    if (!IsDetectionEnabled(CLIMB_HACK_DETECTION))
        return;

    if (opcode != MSG_MOVE_HEARTBEAT || data.lastOpcode != MSG_MOVE_HEARTBEAT)
        return;

    // legal flags are handled by the other detections
    if (player->IsInWater() || player->IsFlying() || player->IsFalling())
        return;

    Position playerPos = player->GetPosition();

    float deltaZ = std::fabs(playerPos.GetPositionZ() - movementInfo.pos.GetPositionZ());
    float deltaXY = movementInfo.pos.GetExactDist2d(playerPos);

    float angle = Position::NormalizeOrientation(std::tan(deltaZ / deltaXY));

    if (angle > CLIMB_ANGLE)
    {
        TC_LOG_ERROR("anticheat", "AnticheatMgr:: Climb-Hack detected player {}", player->GetGUID().ToString());
        BuildReport(player, CLIMB_HACK_REPORT, data);
    }
}

void AnticheatMgr::SpeedHackDetection(Player* player, MovementInfo const& movementInfo, AnticheatData& data)
{
    if (!IsDetectionEnabled(SPEED_HACK_DETECTION))
        return;

    // The movement flag can be set by the client, so the map must be checked as well.
    if (data.lastMovementInfo.HasMovementFlag(MOVEMENTFLAG_ONTRANSPORT) && player->GetMapId() == DEEPRUN_TRAM_MAP_ID)
        return;

    uint32 distance2D = static_cast<uint32>(movementInfo.pos.GetExactDist2d(data.lastMovementInfo.pos));

    UnitMoveType moveType;
    if (player->HasUnitMovementFlag(MOVEMENTFLAG_SWIMMING))
        moveType = MOVE_SWIM;
    else if (player->IsFlying())
        moveType = MOVE_FLIGHT;
    else if (player->HasUnitMovementFlag(MOVEMENTFLAG_WALKING))
        moveType = MOVE_WALK;
    else
        moveType = MOVE_RUN;

    // yards per second the player is allowed to cover (integer cast gives a margin of tolerance)
    uint32 speedRate = static_cast<uint32>(player->GetSpeed(moveType) + movementInfo.jump.xyspeed);

    uint32 timeDiff = getMSTimeDiff(data.lastMovementInfo.time, movementInfo.time);
    if (!timeDiff)
        timeDiff = 1;

    uint32 clientSpeedRate = distance2D * 1000 / timeDiff;

    if (clientSpeedRate > speedRate)
    {
        BuildReport(player, SPEED_HACK_REPORT, data);
        TC_LOG_ERROR("anticheat", "AnticheatMgr:: Speed-Hack detected player {}", player->GetGUID().ToString());
    }
}

void AnticheatMgr::HandlePlayerLogin(Player* player)
{
    ObjectGuid::LowType guid = player->GetGUID().GetCounter();

    // a crash may have left a stale row behind
    CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid={}", guid);

    AnticheatData& data = _players[guid];
    data = AnticheatData();
    data.lastMovementInfo.pos = player->GetPosition();
}

void AnticheatMgr::HandlePlayerLogout(Player* player)
{
    ObjectGuid::LowType guid = player->GetGUID().GetCounter();

    // offline players must not appear in the table queried by the GM commands
    CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid={}", guid);
    _players.erase(guid);
}

void AnticheatMgr::SavePlayerData(Player* player)
{
    ObjectGuid::LowType guid = player->GetGUID().GetCounter();

    auto itr = _players.find(guid);
    if (itr == _players.end())
        return;

    AnticheatData const& data = itr->second;
    CharacterDatabase.PExecute("REPLACE INTO players_reports_status (guid, average, total_reports, speed_reports, fly_reports, jump_reports, waterwalk_reports, teleportplane_reports, climb_reports, creation_time) VALUES ({}, {}, {}, {}, {}, {}, {}, {}, {}, {})",
        guid, data.average, data.totalReports,
        data.typeReports[SPEED_HACK_REPORT], data.typeReports[FLY_HACK_REPORT], data.typeReports[JUMP_HACK_REPORT],
        data.typeReports[WALK_WATER_HACK_REPORT], data.typeReports[TELEPORT_PLANE_HACK_REPORT], data.typeReports[CLIMB_HACK_REPORT],
        data.creationTime);
}

uint32 AnticheatMgr::GetTotalReports(ObjectGuid::LowType guid) const
{
    auto itr = _players.find(guid);
    return itr != _players.end() ? itr->second.totalReports : 0;
}

float AnticheatMgr::GetAverage(ObjectGuid::LowType guid) const
{
    auto itr = _players.find(guid);
    return itr != _players.end() ? itr->second.average : 0.0f;
}

uint32 AnticheatMgr::GetTypeReports(ObjectGuid::LowType guid, uint8 type) const
{
    auto itr = _players.find(guid);
    if (itr == _players.end() || type >= MAX_REPORT_TYPES)
        return 0;

    return itr->second.typeReports[type];
}

void AnticheatMgr::BuildReport(Player* player, uint8 reportType, AnticheatData& data)
{
    // jump reports are counted immediately, every other type needs repeated detections in a short window
    if (reportType != JUMP_HACK_REPORT)
    {
        uint32 now = getMSTime();

        if (!data.tempReportsTimer[reportType])
            data.tempReportsTimer[reportType] = now;

        if (getMSTimeDiff(data.tempReportsTimer[reportType], now) < TEMP_REPORTS_WINDOW_MS)
        {
            if (++data.tempReports[reportType] < TEMP_REPORTS_REQUIRED)
                return;
        }
        else
        {
            data.tempReportsTimer[reportType] = now;
            data.tempReports[reportType] = 1;
            return;
        }
    }

    if (!data.totalReports)
        data.creationTime = getMSTime();

    ++data.totalReports;
    ++data.typeReports[reportType];

    uint32 diffTime = getMSTimeDiff(data.creationTime, getMSTime()) / IN_MILLISECONDS;
    if (diffTime > 0)
        data.average = float(data.totalReports) / float(diffTime);

    if (data.totalReports > _reportsForIngameWarnings)
    {
        std::string str = "|cFFFFFC00[AC]|cFF00FFFF[|cFF60FF00" + player->GetName() + "|cFF00FFFF] Possible cheater!";
        WorldPacket packet(SMSG_NOTIFICATION, str.size() + 1);
        packet << str;
        sWorld->SendGlobalGMMessage(&packet);
    }
}

void AnticheatMgr::AnticheatGlobalCommand(ChatHandler* handler)
{
    // players are saved so the table holds their current data
    ObjectAccessor::SaveAllPlayers();

    QueryResult result = CharacterDatabase.Query("SELECT guid, average, total_reports FROM players_reports_status WHERE total_reports != 0 ORDER BY average ASC LIMIT 3");
    if (!result)
    {
        handler->SendSysMessage("No players found.");
        return;
    }

    handler->SendSysMessage("=============================");
    handler->SendSysMessage("Players with the lowest averages:");
    do
    {
        Field* fields = result->Fetch();
        ObjectGuid::LowType guid = fields[0].GetUInt32();
        float average = fields[1].GetFloat();
        uint32 totalReports = fields[2].GetUInt32();

        if (Player* player = ObjectAccessor::FindPlayer(ObjectGuid::Create<HighGuid::Player>(guid)))
            handler->PSendSysMessage("Player: %s Average: %f Total Reports: %u", player->GetName().c_str(), average, totalReports);
    } while (result->NextRow());

    result = CharacterDatabase.Query("SELECT guid, average, total_reports FROM players_reports_status WHERE total_reports != 0 ORDER BY total_reports DESC LIMIT 3");
    if (!result)
    {
        handler->SendSysMessage("No players found.");
        return;
    }

    handler->SendSysMessage("=============================");
    handler->SendSysMessage("Players with the more reports:");
    do
    {
        Field* fields = result->Fetch();
        ObjectGuid::LowType guid = fields[0].GetUInt32();
        float average = fields[1].GetFloat();
        uint32 totalReports = fields[2].GetUInt32();

        if (Player* player = ObjectAccessor::FindPlayer(ObjectGuid::Create<HighGuid::Player>(guid)))
            handler->PSendSysMessage("Player: %s Total Reports: %u Average: %f", player->GetName().c_str(), totalReports, average);
    } while (result->NextRow());
}

void AnticheatMgr::AnticheatDeleteCommand(ObjectGuid::LowType guid)
{
    if (!guid)
    {
        for (auto& [lowGuid, data] : _players)
            data.ResetReports();

        CharacterDatabase.Execute("DELETE FROM players_reports_status");
        return;
    }

    if (auto itr = _players.find(guid); itr != _players.end())
        itr->second.ResetReports();

    CharacterDatabase.PExecute("DELETE FROM players_reports_status WHERE guid={}", guid);
}
