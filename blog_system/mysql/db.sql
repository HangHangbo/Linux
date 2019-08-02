drop database blog_system;
create database blog_system;
use blog_system;

drop table if exists blog_table;
create table blog_table(
	blog_id int not null primary key auto_increment,
	title varchar(50),
	content text,
	author_id int,
	tag_id int,
	create_time varchar(50)
);


drop table if exists tag_table;
create table tag_table(
	tag_id int not null primary key auto_increment,
	tag_name varchar(50)
);

create table author_table(
    author_id int not null primary key auto_increment,
    user_name varchar(50),
    passwords varchar(50) 
);