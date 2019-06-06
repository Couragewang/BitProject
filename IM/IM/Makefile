PROG = IM_Server
CC=g++
INCLUDE=-I../mongoose -I../mysql_lib/include/
LIB=-L../mysql_lib/lib -lmysqlclient
$(PROG):main.cc ../mongoose/mongoose.c
	$(CC) -std=c++11 -o $@ $^ $(INCLUDE) $(LIB)
.PHONY:clean
clean:
	rm -f $(PROG)
