CREATE TABLE `log_temperature` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `gridId` int(11) NOT NULL,
  `deviceId` int(11) NOT NULL,
  `sensorId` int(11) NOT NULL,
  `temperature` int(11) NOT NULL,
  `timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
