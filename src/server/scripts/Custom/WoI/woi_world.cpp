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
#include "Config.h"
#include <algorithm>
#include "Timer.h"
#include "WoI.h"

// Delivers mail queued by external tools (web shop, vote site) in `mail_external`.
class woi_external_mail : public WorldScript
{
public:
    woi_external_mail() : WorldScript("woi_external_mail"), _enabled(false) { }

    void OnConfigLoad(bool /*reload*/) override
    {
        WoI::LoadSettings();
        _enabled = sConfigMgr->GetIntDefault("ExternalMail", 0) != 0;
        uint32 const intervalMinutes = std::max<uint32>(1, sConfigMgr->GetIntDefault("ExternalMailInterval", 1));
        _timer.SetInterval(intervalMinutes * MINUTE * IN_MILLISECONDS);
        _timer.Reset();
    }

    void OnUpdate(uint32 diff) override
    {
        if (!_enabled)
            return;

        _timer.Update(diff);
        if (!_timer.Passed())
            return;

        WoI::ExternalMail::SendQueued();
        _timer.Reset();
    }

private:
    bool _enabled;
    IntervalTimer _timer;
};

void AddSC_woi_world()
{
    new woi_external_mail();
}
