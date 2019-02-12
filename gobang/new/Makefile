INCLUDE=-I./lib/include
LIB_PATH=-L./lib/lib
LD_LIB=-ljsoncpp -lpthread -std=c++11 -static

cc=g++ -Wall -g
.PHONY:all
all:Client Hall Game

Client:Client.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LD_LIB)

Hall:Hall.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LD_LIB)

Game:Game.cc
	$(cc) -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LD_LIB)

.PHONY:clean
clean:
	rm -rf Client Hall Game
