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
#include "DatabaseEnv.h"
#include "Item.h"
#include "Log.h"
#include "Mail.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Player.h"
#include <vector>

namespace WoI::ExternalMail
{
namespace
{
    struct QueuedItem
    {
        uint32 entry;
        uint32 count;
    };

    struct QueuedMail
    {
        uint32 id;
        ObjectGuid::LowType receiver;
        std::string subject;
        std::string body;
        uint32 money;
        std::vector<QueuedItem> items;
    };

    std::vector<QueuedMail> LoadQueue()
    {
        std::vector<QueuedMail> queue;
        QueryResult result = CharacterDatabase.Query("SELECT e.`id`, e.`receiver`, e.`subject`, e.`message`, e.`money`, i.`item`, i.`count` "
            "FROM `mail_external` e LEFT JOIN `mail_external_items` i ON e.`id` = i.`mail_id` ORDER BY e.`id`");
        if (!result)
            return queue;

        do
        {
            Field* fields = result->Fetch();
            uint32 const id = fields[0].GetUInt32();
            if (queue.empty() || queue.back().id != id)
                queue.push_back({ id, fields[1].GetUInt64(), fields[2].GetString(), fields[3].GetString(), fields[4].GetUInt32(), {} });

            if (uint32 const itemEntry = fields[5].GetUInt32())
                queue.back().items.push_back({ itemEntry, fields[6].GetUInt32() });
        }
        while (result->NextRow());

        return queue;
    }
}

void SendQueued()
{
    std::vector<QueuedMail> const queue = LoadQueue();
    if (queue.empty())
        return;

    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();
    for (QueuedMail const& queued : queue)
    {
        Player* receiver = ObjectAccessor::FindConnectedPlayer(ObjectGuid::Create<HighGuid::Player>(queued.receiver));

        MailDraft draft(queued.subject, queued.body);
        if (queued.money)
            draft.AddMoney(queued.money);

        for (QueuedItem const& entry : queued.items)
        {
            Item* item = Item::CreateItem(entry.entry, entry.count, receiver);
            if (!item)
            {
                TC_LOG_ERROR("misc", "ExternalMail: mail {} references unknown item {}, item skipped.", queued.id, entry.entry);
                continue;
            }
            item->SaveToDB(trans);
            draft.AddItem(item);
        }

        draft.SendMailTo(trans, MailReceiver(receiver, queued.receiver), MailSender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_RETURNED);

        trans->PAppend("DELETE FROM `mail_external_items` WHERE `mail_id` = {}", queued.id);
        trans->PAppend("DELETE FROM `mail_external` WHERE `id` = {}", queued.id);
        TC_LOG_INFO("misc", "ExternalMail: sent mail {} to character {}.", queued.id, queued.receiver);
    }
    CharacterDatabase.CommitTransaction(trans);
}
}
