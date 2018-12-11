.PHONY:all
all: server client

client:client.cc
	g++ -o $@ $^ -std=c++1z -lzmq
server:server.cc
	g++ -o $@ $^ -std=c++1z -lzmq
.PHONY:clean
clean:
	rm -f server client
