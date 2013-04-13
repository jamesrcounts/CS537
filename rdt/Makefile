###################################
# Makefile
# rdt
# CS537 - Spring 2013
# Jim Counts && Michael Ngyuen
###################################
CC = g++
TI = -I ../igloo -I ../approvals
CFLAGS = -Wall -lpthread
CHKFLAGS = -v --inline-suppr --enable=all
STYLE = -YfDHUbps4yj -k3

all: runtest 

clean: 
	rm -f tests/*.orig tests/bin/*_tests 

runtest: tests
	./tests/bin/receiver_tests

tests: tests/bin tests/bin/receiver_tests

tests/bin/receiver_tests: tests/receiver_tests.cpp 
	cppcheck $(CHKFLAGS) tests/receiver_tests.cpp
	astyle $(STYLE) tests/receiver_tests.cpp
	$(CC) $(TI) tests/receiver_tests.cpp -o tests/bin/receiver_tests $(CFLAGS)

tests/bin: 
	mkdir tests/bin

# DO NOT DELETE