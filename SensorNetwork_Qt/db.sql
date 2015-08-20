CREATE USER 'remote_root'@'%' IDENTIFIED BY 'secret';
GRANT ALL PRIVILEGES ON *.* TO 'remote_root'@'%' WITH GRANT OPTION;

#create new database
create database if not exists sensors;
use sensors;
DROP TABLE IF EXISTS lu_xuemei;

create TABLE lu_xuemei(
	Id INT PRIMARY KEY AUTO_INCREMENT,
	sensor_id INT UNSIGNED,
	sensor_value DECIMAL(4,2),
	timestamp TIMESTAMP);
