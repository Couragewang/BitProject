.PHONY:all
all:client
#server client

server:server.cc
	g++ -o $@ $^ -I. -lpthread
client:client.cc
	g++ -o $@ $^ -I. -Ilib/include -Llib/lib -ljsoncpp -lpthread -static

.PHONY:clean
clean:
	rm -f server client
