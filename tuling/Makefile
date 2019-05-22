INCLUDE=-I. -I./speech
LIB=-ljsoncpp -lcurl -lcrypto -lpthread
Jarvis:Jarvis.cc
	g++ -o $@ $^ $(LIB) -std=c++11 $(INCLUDE) #-static
.PHONY:clean
clean:
	rm -f Jarvis
