###################################
# Makefile
# rdt
# CS537 - Spring 2013
# Jim Counts && Michael Ngyuen
###################################
CC = g++
TI = -I ../igloo -I ../approvals -I rdt
CFLAGS = -Wall -lpthread
CHKFLAGS = -v --inline-suppr --enable=all
STYLE = -YfDHUbps4yj -k3

all: runtest 

clean: 
	rm -f tests/*.orig tests/bin/*_tests 

runtest: tests
	./tests/bin/packet_tests
	./tests/bin/receiver_tests

tests: tests/bin tests/bin/receiver_tests tests/bin/packet_tests

tests/bin/packet_tests: tests/packet_tests.cpp packet.h 
	cppcheck $(CHKFLAGS) tests/packet_tests.cpp
	astyle $(STYLE) tests/packet_tests.cpp
	$(CC) $(TI) tests/packet_tests.cpp -o tests/bin/packet_tests $(CFLAGS)

tests/bin/receiver_tests: tests/receiver_tests.cpp packet.h rdt/rdt.h
	cppcheck $(CHKFLAGS) tests/receiver_tests.cpp
	astyle $(STYLE) tests/receiver_tests.cpp
	$(CC) $(TI) tests/receiver_tests.cpp -o tests/bin/receiver_tests $(CFLAGS)

tests/bin: 
	mkdir tests/bin

packet.h: rdt/packet.h
	cppcheck $(CHKFLAGS) rdt/packet.h
	astyle $(STYLE) rdt/packet.h

# DO NOT DELETE