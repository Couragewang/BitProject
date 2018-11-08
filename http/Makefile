.PHONY:all
all:Httpd TestCGI

TestCGI:TestCGI.cpp
	g++ -o $@ $^

Httpd:Httpd.cpp
	g++ -o $@ $^ -std=c++11 -lpthread -g #-static #-g -fpermissive

.PHONY:clean
clean:
	rm -rf Httpd TestCGI output

.PHONY:output
output:
	@mkdir output
	@cp -rf wwwroot output
	@cp Httpd output
	@cp start.sh output
	@cp TestCGI output/wwwroot
