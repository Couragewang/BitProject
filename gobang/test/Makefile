.PHONY:all
all: server client

client:client.cc
	g++ -o $@ $^ -std=c++1z -lzmq -lpthread
server:server.cc
	g++ -o $@ $^ -std=c++1z -lzmq -lpthread
.PHONY:clean
clean:
	rm -f server client
