###################################
# Makefile
# HW 1
# CS537 - Data Communications - Spring 2013
# Jim Counts
###################################
CC = g++
INCLUDE = -I ../igloo
CFLAGS = -lpthread -Wall
SRCS = test.cpp InternetSocket.h

all: format runtest check

runtest: test
	./test
	
test: test.cpp InternetSocket.h
	g++ $(INCLUDE) test.cpp -o test -g $(CFLAGS)

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: format
	cppcheck -v --inline-suppr  --enable=all $(SRCS)

# DO NOT DELETE