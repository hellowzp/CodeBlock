#change password
UPDATE mysql.user SET Password=PASSWORD('newpwd') WHERE User='root';
FLUSH PRIVILEGES;


