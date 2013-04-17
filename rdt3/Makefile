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
	rm -f tests/*.orig tests/bin/*_tests 

runtest: tests
	./tests/bin/rdt_tests

tests: tests/bin tests/bin/rdt_tests 

tests/bin/rdt_tests: headers tests/rdt_tests.cpp 
	cppcheck $(CHKFLAGS) tests/rdt_tests.cpp
	astyle $(STYLE) tests/rdt_tests.cpp
	$(CC) $(TI) tests/rdt_tests.cpp -o tests/bin/rdt_tests $(CFLAGS)

headers: tests/packet.ck tests/rdt.ck tests/udt.ck

tests/packet.ck: tests/packet.h
	cppcheck $(CHKFLAGS) tests/packet.h
	astyle $(STYLE) tests/packet.h
	touch tests/packet.ck

tests/rdt.ck: tests/rdt.h
	cppcheck $(CHKFLAGS) tests/rdt.h
	astyle $(STYLE) tests/rdt.h
	touch tests/rdt.ck

tests/udt.ck: tests/udt.h
	cppcheck $(CHKFLAGS) tests/udt.h
	astyle $(STYLE) tests/udt.h
	touch tests/udt.ck

tests/bin: 
	mkdir tests/bin

# DO NOT DELETE