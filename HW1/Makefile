###################################
# Makefile
# HW 1
# CS537 - Data Communications - Spring 2013
# Jim Counts
###################################
CC = g++
INCLUDE = -I ../igloo
CFLAGS = -O2 -Wall
SRCS = test.cpp

all: format runtest  check

runtest: test
	./test

test: test.cpp
	$(CC) $(INCLUDE) test.cpp -o test $(CFLAGS)	

server.o: 
	$(CC) $(INCLUDE) -c server.h $(CFLAGS)

object: 
	$(CC) -c LinkedStack.h LinkedStack.cpp $(CFLAGS)

format:
	astyle -bps4 $(SRCS)

check:
	cppcheck -v --enable=all $(SRCS)

# DO NOT DELETE