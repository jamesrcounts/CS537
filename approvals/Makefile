###################################
# Makefile
###################################
CC = g++
INCLUDE = -I ../igloo -I ~/lib/boost/include
LINK = -L ~/lib/boost/lib
CFLAGS = -lpthread -lboost_system -lboost_filesystem -Wall -static
CHECK = cppcheck -v --inline-suppr --suppress=unmatchedSuppression --enable=all
FORMAT = astyle -YfDHUbps4yj -k3 

all: runtest 

runtest: test/NamerTests.cpp test/StringWriterTests.cpp test/reporters/ReporterTests.cpp FileApproverTests
	./test/bin/NamerTests
	./test/bin/StringWriterTests
	./test/bin/ReporterTests
	./test/bin/FileApproverTests

NamerTests.cpp: test/NamerTests.cpp Namer.h
	$(FORMAT) test/NamerTests.cpp
	g++ $(INCLUDE) $(LINK) test/NamerTests.cpp -o test/bin/NamerTests -g $(CFLAGS)

StringWriterTests.cpp: test/StringWriterTests.cpp StringWriter.h
	$(FORMAT) test/StringWriterTests.cpp
	g++ $(INCLUDE) $(LINK) test/StringWriterTests.cpp -o test/bin/StringWriterTests -g $(CFLAGS)

ReporterTests.cpp: test/reporters/ReporterTests.cpp reporters/Reporter.h
	$(FORMAT) test/reporters/ReporterTests.cpp
	g++ $(INCLUDE) $(LINK) test/reporters/ReporterTests.cpp -o test/bin/ReporterTests -g $(CFLAGS)

FileApproverTests: test/FileApproverTests.cpp reporters/Reporter.h
	$(FORMAT) test/FileApproverTests.cpp
	g++ $(INCLUDE) $(LINK) test/FileApproverTests.cpp -o test/bin/FileApproverTests -g $(CFLAGS)

CommandLauncher.h: reporters/CommandLauncher.h
	$(FORMAT) reporters/CommandLauncher.h
	g++ -c reporters/CommandLauncher.h -O3 -Wall

Reporter.h: reporters/Reporter.h reporters/CommandLauncher.h
	$(FORMAT) reporters/Reporter.h
	g++ -c reporters/Reporter.h -O3 -Wall

Namer.h:
	$(FORMAT) Namer.h
	g++ $(INCLUDE) $(LINK) -c Namer.h -O3 -Wall -lboost_system -static

StringWriter.h:
	$(FORMAT) StringWriter.h
	g++ -c StringWriter.h -O3 -Wall

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: format
	cppcheck -v --inline-suppr --suppress=unmatchedSuppression --enable=all $(SRCS)

clean:
	rm -f ./test/bin/*Tests

# DO NOT DELETE