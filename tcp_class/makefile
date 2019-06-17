.PHONY:all
all:dict_server dict_client

dict_server:dict_server.cc tcp_socket.hpp tcp_process_server.hpp 
	g++ dict_server.cc -o $@ -std=c++11

dict_client:dict_client.cc tcp_socket.hpp tcp_client.hpp
	g++ dict_client.cc -o $@ -std=c++11

.PHONY:clean
clean:
	rm dict_client dict_server
