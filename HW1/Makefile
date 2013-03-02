###################################
# Makefile
# HW 1
# CS537 - Data Communications - Spring 2013
# Jim Counts
###################################
CC = g++
INCLUDE = -I ../igloo
CFLAGS = -O2 -Wall -lpthread
SRCS = serverTest.cpp clientTest.cpp Socket.h Server.h Client.h integrationTest.cpp

all: check 

runtest: ServerTest ClientTest IntegrationTest
	./integrationTest
	./serverTest
	./clientTest

IntegrationTest: Client.gch Server.gch integrationTest.cpp format
	$(CC) $(INCLUDE) integrationTest.cpp -o integrationTest $(CFLAGS)

ClientTest: Client.gch clientTest.cpp format
	$(CC) $(INCLUDE) clientTest.cpp -o clientTest $(CFLAGS)	

ServerTest: Server.gch serverTest.cpp format
	$(CC) $(INCLUDE) serverTest.cpp -o serverTest $(CFLAGS)	

Socket.gch: Socket.h
	$(CC) $(INCLUDE) -c Socket.h $(CFLAGS)

Server.gch: Socket.gch Server.h
	$(CC) $(INCLUDE) -c Server.h $(CFLAGS)

Client.gch: Socket.gch Client.h
	$(CC) $(INCLUDE) -c Client.h $(CFLAGS)

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: runtest
	cppcheck -v --inline-suppr --suppress=unmatchedSuppression --enable=all $(SRCS)

# DO NOT DELETE