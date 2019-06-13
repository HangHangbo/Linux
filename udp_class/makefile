.PHONY:all
all:client server

server:dict_server.cc
	g++ $^ -o $@ -std=c++11 

client:client.cc
	g++ $^ -o $@ -std=c++11

.PHONY:clean
clean:
	 -rm edit client server
