FLAGS=-std=c++11
LDFLAGS=-lpthread #-static

.PHONY:all
all:ChatClient ChatServer

ChatClient:ChatClient.cc
	g++ -o $@ $^ -I./lib/include -L./lib/lib -ljsoncpp -lncurses++ $(FLAGS)  $(LDFLAGS)
ChatServer:ChatServer.cc
	g++ -o $@ $^ -I./lib/include -L./lib/lib -ljsoncpp $(FLAGS) $(LDFLAGS)

.PHONY:clean
clean:
	rm -f ChatClient ChatServer
