-- Vote system tables. Schemas are inferred from how the code uses them; if your vote website
-- already owns these tables, keep yours (CREATE TABLE IF NOT EXISTS leaves existing tables alone).
CREATE TABLE IF NOT EXISTS `voting` (
  `user_ip` varchar(45) NOT NULL,
  `time` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`user_ip`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

CREATE TABLE IF NOT EXISTS `voting_points` (
  `id` int unsigned NOT NULL COMMENT 'account id',
  `points` int unsigned NOT NULL DEFAULT '0',
  `time` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
