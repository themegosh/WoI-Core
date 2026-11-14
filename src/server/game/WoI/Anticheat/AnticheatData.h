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

#ifndef AnticheatData_h__
#define AnticheatData_h__

#include "Define.h"
#include "MovementInfo.h"
#include <array>

enum ReportTypes : uint8
{
    SPEED_HACK_REPORT = 0,
    FLY_HACK_REPORT,
    WALK_WATER_HACK_REPORT,
    JUMP_HACK_REPORT,
    TELEPORT_PLANE_HACK_REPORT,
    CLIMB_HACK_REPORT,

    MAX_REPORT_TYPES
};

enum DetectionTypes : uint32
{
    SPEED_HACK_DETECTION            = 1,
    FLY_HACK_DETECTION              = 2,
    WALK_WATER_HACK_DETECTION       = 4,
    JUMP_HACK_DETECTION             = 8,
    TELEPORT_PLANE_HACK_DETECTION   = 16,
    CLIMB_HACK_DETECTION            = 32
};

struct AnticheatData
{
    void ResetReports();

    uint32 lastOpcode = 0;
    MovementInfo lastMovementInfo;
    uint32 totalReports = 0;
    std::array<uint32, MAX_REPORT_TYPES> typeReports = { };
    float average = 0.0f;
    uint32 creationTime = 0;
    std::array<uint32, MAX_REPORT_TYPES> tempReports = { };
    std::array<uint32, MAX_REPORT_TYPES> tempReportsTimer = { };
};

#endif // AnticheatData_h__
