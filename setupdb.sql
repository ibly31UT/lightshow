CREATE USER 'user'@'localhost' IDENTIFIED BY 'password';
CREATE DATABASE lightsdb;
GRANT ALL PRIVILEGES ON lightsdb.* TO 'user'@'localhost';
FLUSH PRIVILEGES;
