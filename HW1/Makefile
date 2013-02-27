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

test: test.cpp format
	$(CC) $(INCLUDE) test.cpp -o test $(CFLAGS)	

server.o: 
	$(CC) $(INCLUDE) -c server.h $(CFLAGS)

object: 
	$(CC) -c LinkedStack.h LinkedStack.cpp $(CFLAGS)

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: runtest
	cppcheck -v --inline-suppr --suppress=unmatchedSuppression --enable=all $(SRCS)

# DO NOT DELETE