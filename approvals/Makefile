###################################
# Makefile
###################################
CC = g++
INCLUDE = -I ../igloo -I ~/lib/boost/include
LINK = -L ~/lib/boost/lib
CFLAGS = -lpthread -lboost_system -lboost_filesystem -Wall -static
SRCS = test/*.cpp *.h test/reporters/*.cpp reporters/*.h

all: format runtest check 

runtest: NamerTests StringWriterTests ReporterTests FileApproverTests
	./test/bin/NamerTests
	./test/bin/StringWriterTests
	./test/bin/ReporterTests
	./test/bin/FileApproverTests

NamerTests: test/NamerTests.cpp Namer.gch
	g++ $(INCLUDE) $(LINK) test/NamerTests.cpp -o test/bin/NamerTests -g $(CFLAGS)

StringWriterTests: test/StringWriterTests.cpp StringWriter.gch
	g++ $(INCLUDE) $(LINK) test/StringWriterTests.cpp -o test/bin/StringWriterTests -g $(CFLAGS)

ReporterTests: test/reporters/ReporterTests.cpp Reporter.gch
	g++ $(INCLUDE) $(LINK) test/reporters/ReporterTests.cpp -o test/bin/ReporterTests -g $(CFLAGS)

FileApproverTests: test/FileApproverTests.cpp Reporter.gch
	g++ $(INCLUDE) $(LINK) test/FileApproverTests.cpp -o test/bin/FileApproverTests -g $(CFLAGS)

CommandLauncher.gch: reporters/CommandLauncher.h
	g++ -c reporters/CommandLauncher.h -O3 -Wall

Reporter.gch: reporters/Reporter.h CommandLauncher.gch
	g++ -c reporters/Reporter.h -O3 -Wall

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