.PHONY:all
all: client server
client:main_client.cpp
	g++ -o $@ $^ -std=c++1z -lzmq
server:main_server.cpp
	g++ -o $@ $^ -std=c++1z -lzmq
.PHONY:clean
clean:
	rm -f client server
