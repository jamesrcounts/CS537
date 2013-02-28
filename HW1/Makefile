###################################
# Makefile
# HW 1
# CS537 - Data Communications - Spring 2013
# Jim Counts
###################################
CC = g++
INCLUDE = -I ../igloo
CFLAGS = -O2 -Wall -lpthread
SRCS = test.cpp

all: check 

runtest: test
	./test

ClientTest: Client.gch clientTest.cpp format
	$(CC) $(INCLUDE) clientTest.cpp -o clientTest $(CFLAGS)
	
test: Client.gch Socket.gch Server.gch test.cpp format
	$(CC) $(INCLUDE) test.cpp -o test $(CFLAGS)	

Socket.gch: Socket.h
	$(CC) $(INCLUDE) -c Socket.h $(CFLAGS)

Server.gch: Socket.gch
	$(CC) $(INCLUDE) -c Server.h $(CFLAGS)

Client.gch: Socket.gch
	$(CC) $(INCLUDE) -c Client.h $(CFLAGS)

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: runtest
	cppcheck -v --inline-suppr --suppress=unmatchedSuppression --enable=all $(SRCS)

# DO NOT DELETE