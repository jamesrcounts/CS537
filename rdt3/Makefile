###################################
# Makefile
# rdt
# CS537 - Spring 2013
# Jim Counts && Michael Ngyuen
###################################
CC = g++
TI = -I igloo -I approvals -I rdt
CFLAGS = -Wall -lpthread
CHKFLAGS = -v --inline-suppr --enable=all
STYLE = -YfDHUbps4yj -k3
HDRS = rdt/*.h
OBJ = rdt/*.o

all: server client

server: rdt/bin rdt/rdt.a rdt/rdt_server.cpp 
	cppcheck $(CHKFLAGS) rdt/rdt_server.cpp $(HDRS)
	astyle $(STYLE) rdt/rdt_server.cpp $(HDRS)
	$(CC) rdt/rdt_server.cpp -L./rdt -lrdt -o rdt/bin/rdt_server -Wall 

client: rdt/bin rdt/rdt.a rdt/rdt_client.cpp 
	cppcheck $(CHKFLAGS) rdt/rdt_client.cpp $(HDRS)
	astyle $(STYLE) rdt/rdt_client.cpp $(HDRS)
	$(CC) rdt/rdt_client.cpp -L./rdt -lrdt -o rdt/bin/rdt_client -Wall 

rdt/rdt.a: rdt/rdt.o
	ar rcs rdt/librdt.a rdt/rdt.o
	
rdt/rdt.o: rdt/rdt.h rdt/rdt.cpp
	cppcheck $(CHKFLAGS) rdt/rdt.cpp $(HDRS)
	astyle $(STYLE) rdt/rdt.cpp  $(HDRS)
	$(CC) -c rdt/rdt.cpp  -o rdt/rdt.o -Wall 



rdt/bin:
	mkdir rdt/bin

clean: 
	rm -f rdt/*.o rdt/*.orig 


# DO NOT DELETE