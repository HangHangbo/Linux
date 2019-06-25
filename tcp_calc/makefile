.PHONY:all
all:calc_server calc_client

dict_server:calc_server.cc tcp_socket.hpp tcp_server.hpp 
	g++ calc_server.cc -o $@ -std=c++11 -lpthread

dict_client:calc_client.cc tcp_socket.hpp tcp_client.hpp
	g++ calc_client.cc -o $@ -std=c++11

.PHONY:clean
clean:
	rm calc_client calc_server