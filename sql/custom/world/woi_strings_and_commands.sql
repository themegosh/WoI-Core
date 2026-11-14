-- trinity_string ids used by WoIStrings (src/server/game/WoI/WoI.h). The original texts only
-- existed in the live database, so these are reconstructed from how each string is used.
-- Replace with your own wording if you exported the old rows.
DELETE FROM `trinity_string` WHERE `entry` IN (11103, 11104, 11105, 11106, 11107, 11108, 11109, 11110, 11111, 11112, 11114, 11115);
INSERT INTO `trinity_string` (`entry`, `content_default`) VALUES
(11103, 'You have fully healed %s.'),
(11104, 'You have %u vote points.'),
(11105, '%s has %u vote points.'),
(11106, '%s has not voted in the last 12 hours.'),
(11107, '%s has voted in the last 12 hours.'),
(11108, 'Vote points of %s changed from %d to %d.'),
(11109, 'You have never voted. Vote for the server to earn vote points.'),
(11110, 'You forced a vote for %s.'),
(11111, 'A game master has registered a vote for you.'),
(11112, 'You have not voted in the last 12 hours. Vote to unlock every teleport location.'),
(11115, 'Some teleport locations are reserved for approved accounts.'),
(11114, 'Your guild does not have a guild house.');

-- Command help follows the renamed commands (.appear -> .goname, .summon -> .namego).
UPDATE `command` SET `name` = 'goname' WHERE `name` = 'appear';
UPDATE `command` SET `name` = 'namego' WHERE `name` = 'summon';
DELETE FROM `command` WHERE `name` IN ('vp show', 'vp modify', 'vp check', 'vp force', 'heal');
INSERT INTO `command` (`name`, `help`) VALUES
('vp show', 'Syntax: .vp show [$playername]

Shows your vote points. Staff can look up another online player.'),
('vp modify', 'Syntax: .vp modify $playername $amount

Adds (or removes, if negative) vote points of an online player who has voted before.'),
('vp check', 'Syntax: .vp check $playername

Shows whether the player has voted in the last 12 hours.'),
('vp force', 'Syntax: .vp force $playername

Registers a vote for the player''s IP address as if they had just voted.'),
('heal', 'Syntax: .heal [$playername]

Fully heals and resurrects the selected or named player, or yourself.');
