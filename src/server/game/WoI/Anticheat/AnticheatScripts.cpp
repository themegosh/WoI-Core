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

#include "AnticheatScripts.h"
#include "AnticheatMgr.h"

void AnticheatPlayerScript::OnLogin(Player* player, bool /*firstLogin*/)
{
    sAnticheatMgr->HandlePlayerLogin(player);
}

void AnticheatPlayerScript::OnLogout(Player* player)
{
    sAnticheatMgr->HandlePlayerLogout(player);
}

void AnticheatPlayerScript::OnSave(Player* player)
{
    sAnticheatMgr->SavePlayerData(player);
}

void AnticheatWorldScript::OnConfigLoad(bool /*reload*/)
{
    sAnticheatMgr->LoadConfig();
}
