PROG = IM_Server
CC=g++
INCLUDE=-I../mongoose -I../mysql_lib/include/
LIB=-L../mysql_lib/lib -lmysqlclient -ljsoncpp
$(PROG):main.cc ../mongoose/mongoose.c
	$(CC) -Werror -std=c++11 -o $@ $^ $(INCLUDE) $(LIB) -g
.PHONY:clean
clean:
	rm -f $(PROG)
