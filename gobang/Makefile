.PHONY:all
all: GobangServer GobangClient

GobangClient:GobangClient.cc
	g++ -o $@ $^ -std=c++1z -lzmq
GobangServer:GobangServer.cc
	g++ -o $@ $^ -std=c++1z -lzmq
.PHONY:clean
clean:
	rm -f GobangServer GobangClient
