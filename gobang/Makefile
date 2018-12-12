.PHONY:all
all: GobangClient GobangServer

GobangClient:GobangClient.cc
	g++ -o $@ $^ -std=c++1z -lzmq -lpthread
GobangServer:GobangServer.cc
	g++ -o $@ $^ -std=c++1z -lzmq -lpthread
.PHONY:clean
clean:
	rm -f GobangServer GobangClient
