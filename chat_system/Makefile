.PHONY:all
all:ChatClient ChatServer

ChatClient:ChatClient.cc
	g++ -o $@ $^
ChatServer:ChatServer.cc
	g++ -o $@ $^

.PHONY:clean
clean:
	rm -f ChatClient ChatServer
