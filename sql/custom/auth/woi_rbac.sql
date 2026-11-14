-- Permissions for the .vp and .heal commands (ids match src/server/scripts/Custom/WoI/cs_woi.cpp).
-- Roles: 196 Administrator, 197 Gamemaster, 199 Player.
DELETE FROM `rbac_permissions` WHERE `id` BETWEEN 1001 AND 1005;
INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
(1001, 'Command: vp show'),
(1002, 'Command: vp modify'),
(1003, 'Command: vp check'),
(1004, 'Command: vp force'),
(1005, 'Command: heal');

DELETE FROM `rbac_linked_permissions` WHERE `linkedId` BETWEEN 1001 AND 1005;
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
(199, 1001),
(196, 1002),
(196, 1003),
(196, 1004),
(197, 1005);
