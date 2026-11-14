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

#ifndef AnticheatMgr_h__
#define AnticheatMgr_h__

#include "AnticheatData.h"
#include "ObjectGuid.h"
#include <unordered_map>

class ChatHandler;
class Player;

class TC_GAME_API AnticheatMgr
{
    AnticheatMgr() = default;
    ~AnticheatMgr() = default;

public:
    static AnticheatMgr* instance();

    void StartScripts();
    void LoadConfig();

    bool IsEnabled() const { return _enabled; }
    void SetEnabled(bool enabled) { _enabled = enabled; }
    uint32 GetReportsForIngameWarnings() const { return _reportsForIngameWarnings; }
    uint32 GetDetectionsEnabled() const { return _detectionsEnabled; }

    void StartHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode);
    void SavePlayerData(Player* player);

    void HandlePlayerLogin(Player* player);
    void HandlePlayerLogout(Player* player);

    uint32 GetTotalReports(ObjectGuid::LowType guid) const;
    float GetAverage(ObjectGuid::LowType guid) const;
    uint32 GetTypeReports(ObjectGuid::LowType guid, uint8 type) const;

    void AnticheatGlobalCommand(ChatHandler* handler);
    void AnticheatDeleteCommand(ObjectGuid::LowType guid);

private:
    bool IsDetectionEnabled(DetectionTypes detection) const { return (_detectionsEnabled & detection) != 0; }

    void SpeedHackDetection(Player* player, MovementInfo const& movementInfo, AnticheatData& data);
    void FlyHackDetection(Player* player, AnticheatData& data);
    void WalkOnWaterHackDetection(Player* player, AnticheatData& data);
    void JumpHackDetection(Player* player, uint32 opcode, AnticheatData& data);
    void TeleportPlaneHackDetection(Player* player, MovementInfo const& movementInfo, AnticheatData& data);
    void ClimbHackDetection(Player* player, MovementInfo const& movementInfo, uint32 opcode, AnticheatData& data);

    void BuildReport(Player* player, uint8 reportType, AnticheatData& data);

    std::unordered_map<ObjectGuid::LowType, AnticheatData> _players;
    bool _enabled = true;
    uint32 _reportsForIngameWarnings = 70;
    uint32 _detectionsEnabled = 31;
};

#define sAnticheatMgr AnticheatMgr::instance()

#endif // AnticheatMgr_h__
