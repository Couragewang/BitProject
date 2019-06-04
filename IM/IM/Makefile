PROG = IM_Server
CC=g++
INCLUDE=-I../mongoose
$(PROG):main.cc ../mongoose/mongoose.c
	$(CC) -std=c++11 -o $@ $^ $(INCLUDE)
.PHONY:clean
clean:
	rm -f $(PROG)
