###################################
# Makefile
###################################
CC = g++
INCLUDE = -I ../igloo -I /usr/local/boost_1_53_0
CFLAGS = -lpthread -Wall
SRCS = test/*.cpp *.h

all: format runtest check 

runtest: NamerTests Namer.gch
	./test/bin/NamerTests

NamerTests: test/NamerTests.cpp
	g++ $(INCLUDE) test/NamerTests.cpp -o test/bin/NamerTests -g $(CFLAGS)

Namer.gch: Namer.h
	g++ -c Namer.h -O3 -Wall

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: format
	cppcheck -v --inline-suppr --enable=all $(SRCS)

clean:
	rm -f ./test/bin/*

# DO NOT DELETE