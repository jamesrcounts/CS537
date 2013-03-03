###################################
# Makefile
# HW 1
# CS537 - Data Communications - Spring 2013
# Jim Counts
###################################
CC = g++
INCLUDE = -I ../igloo
CFLAGS = -lpthread
SRCS = httpClient.cpp httpServer.cpp HttpHandler.h test.cpp

all:  httpClient httpServer runtest check

runtest: test
	./test
	
test: test.cpp httpServer
	g++ $(INCLUDE) test.cpp -o test 

httpServer: httpServer.cpp HttpHandler.h
	g++ httpServer.cpp -o httpServer $(CFLAGS)

httpClient: httpClient.cpp
	g++ httpClient.cpp -o httpClient

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: format
	cppcheck -v --inline-suppr  --enable=all $(SRCS)

# DO NOT DELETE