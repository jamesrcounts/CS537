###################################
# Makefile
###################################
CC = g++
INCLUDE = -I ../igloo -I ~/lib/boost/include
LINK = -L ~/lib/boost/lib
CFLAGS = -lpthread -lboost_system -lboost_filesystem -Wall -static
SRCS = test/*.cpp *.h test/reporters/*.cpp

all: format runtest check 

runtest: NamerTests StringWriterTests MeldReporterTests
	./test/bin/NamerTests
	./test/bin/StringWriterTests
	./test/bin/MeldReporterTests

NamerTests: test/NamerTests.cpp Namer.gch
	g++ $(INCLUDE) $(LINK) test/NamerTests.cpp -o test/bin/NamerTests -g $(CFLAGS)

StringWriterTests: test/StringWriterTests.cpp StringWriter.gch
	g++ $(INCLUDE) $(LINK) test/StringWriterTests.cpp -o test/bin/StringWriterTests -g $(CFLAGS)

MeldReporterTests: test/reporters/MeldReporterTests.cpp
	g++ $(INCLUDE) $(LINK) test/reporters/MeldReporterTests.cpp -o test/bin/MeldReporterTests -g $(CFLAGS)
	
Namer.gch: Namer.h
	g++ $(INCLUDE) $(LINK) -c Namer.h -O3 -Wall -lboost_system -static

StringWriter.gch: StringWriter.h
	g++ -c StringWriter.h -O3 -Wall

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: format
	cppcheck -v --inline-suppr --suppress=unmatchedSuppression --enable=all $(SRCS)

clean:
	rm -f ./test/bin/*

# DO NOT DELETE