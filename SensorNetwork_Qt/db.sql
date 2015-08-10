CREATE USER 'remote_root'@'%' IDENTIFIED BY 'secret';
GRANT ALL PRIVILEGES ON *.* TO 'remote_root'@'%' WITH GRANT OPTION;

#create new database
create database if not exists lu_xuemei;
use lu_xuemei;
DROP TABLE IF EXISTS sensor;

create TABLE sensor(
	Id INT PRIMARY KEY AUTO_INCREMENT,
	sensor_id INT UNSIGNED,
	sensor_value DECIMAL(4,2),
	timestamp TIMESTAMP);
