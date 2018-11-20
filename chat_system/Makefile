FLAGS=-std=c++11
LDFLAGS=-ljsoncpp -lpthread
FIND_PATH=-I./lib/include -L./lib/lib

.PHONY:all
all:ChatClient ChatServer

ChatServer:ChatServer.cc
	g++ -o $@ $^ $(FLAGS) $(FIND_PATH) $(LDFLAGS) -static
ChatClient:ChatClient.cc
	g++ -o $@ $^ $(FLAGS) $(FIND_PATH) $(LDFLAGS) -lncurses #-static

.PHONY:clean
clean:
	rm -f ChatClient ChatServer
