-- Guild house phase per guild. `guid` is the guild id; `phasemask` is a phase index
-- (1 = normal phase, N >= 2 = bit N), see WoI::GuildHouse::GetPhaseMask.
CREATE TABLE IF NOT EXISTS `guild_house_masks` (
  `guid` int unsigned NOT NULL,
  `phasemask` int unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- External mail queue, drained by WoI::ExternalMail::SendQueued when ExternalMail = 1.
CREATE TABLE IF NOT EXISTS `mail_external` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `receiver` int unsigned NOT NULL COMMENT 'character guid',
  `subject` varchar(200) NOT NULL DEFAULT '',
  `message` text NOT NULL,
  `money` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `mail_external_items` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `mail_id` int unsigned NOT NULL,
  `item` int unsigned NOT NULL,
  `count` int unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`),
  KEY `idx_mail` (`mail_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
