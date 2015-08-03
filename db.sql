#change password
UPDATE mysql.user SET Password=PASSWORD('newpwd') WHERE User='root';
FLUSH PRIVILEGES;

#for remote connection
CREATE USER 'remote_root'@'%' IDENTIFIED BY 'secret';
GRANT ALL PRIVILEGES ON *.* TO 'remote_root'@'%' WITH GRANT OPTION;

