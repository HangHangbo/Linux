.PHONY:all
all:insert select update delete

select:select.cc
	g++ $^ -o $@ -L/usr/lib64/mysql  -lmysqlclient

insert:insert.cc
	g++ $^ -o $@ -L/usr/lib64/mysql  -lmysqlclient 

update:update.cc
	g++ $^ -o $@ -L/usr/lib64/mysql  -lmysqlclient 

delete:delete.cc
	g++ $^ -o $@ -L/usr/lib64/mysql  -lmysqlclient 	

.PHONY:clean
clean:
	rm insert select