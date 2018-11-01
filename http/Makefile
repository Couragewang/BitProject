.PHONY:all
all:Httpd TestCGI

TestCGI:TestCGI.cpp
	g++ -o $@ $^

Httpd:Httpd.cpp
	g++ -o $@ $^ -std=c++11 -lpthread #-static #-g -fpermissive
.PHONY:clean
clean:
	rm -f Httpd TestCGI
