CC=g++
LD_FLAGS=#-static
FLAGS=-std=c++11 -g

.PHONY:all
all:ChatClient MiniChatServer

MiniChatServer:MiniChatServer.cc
	$(CC) -o $@ $^ $(FLAGS) $(LD_FLAGS)
ChatClient:ChatClient.cc
	$(CC) -o $@ $^ $(FLAGS) $(LD_FLAGS)

.PHONY:clean
clean:
	rm -f ChatClient MiniChatServer
