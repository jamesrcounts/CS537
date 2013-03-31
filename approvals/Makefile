###################################
# Makefile
###################################
CC = g++
INCLUDE = -I ../igloo -I ~/lib/boost
CFLAGS = -lpthread -Wall
SRCS = test/*.cpp *.h

all: format runtest check 

runtest: NamerTests StringWriterTests
	./test/bin/NamerTests
	./test/bin/StringWriterTests

NamerTests: test/NamerTests.cpp Namer.gch
	g++ $(INCLUDE) test/NamerTests.cpp -o test/bin/NamerTests -g $(CFLAGS)

StringWriterTests: test/StringWriterTests.cpp StringWriter.gch
	g++ $(INCLUDE) test/StringWriterTests.cpp -o test/bin/StringWriterTests -g $(CFLAGS)

Namer.gch: Namer.h
	g++ -c Namer.h -O3 -Wall

StringWriter.gch: StringWriter.h
	g++ -c StringWriter.h -O3 -Wall

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: format
	cppcheck -v --inline-suppr --suppress=unmatchedSuppression --enable=all $(SRCS)

clean:
	rm -f ./test/bin/*

# DO NOT DELETE