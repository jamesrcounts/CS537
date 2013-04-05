###################################
# Makefile
###################################
CC = g++
INCLUDE = -I ../igloo -I ~/lib/boost/include
LINK = -L ~/lib/boost/lib
CFLAGS = -lpthread -lboost_system -lboost_filesystem -Wall -static
CHECK = cppcheck -v --inline-suppr --enable=all
FORMAT = astyle -YfDHUbps4yj -k3 

all: runtest 

runtest: NamerTests StringWriterTests ReporterTests FileApproverTests ApprovalExceptionTests
	./test/bin/NamerTests
	./test/bin/StringWriterTests
	./test/bin/ReporterTests
	./test/bin/FileApproverTests
	./test/bin/ApprovalExceptionTests

ApprovalExceptionTests: test/ApprovalExceptionTests.cpp ApprovalException.h
	$(FORMAT) test/ApprovalExceptionTests.cpp
	$(CHECK) --suppress=unmatchedSuppression test/ApprovalExceptionTests.cpp
	g++ $(INCLUDE) $(LINK) test/ApprovalExceptionTests.cpp -o test/bin/ApprovalExceptionTests -g $(CFLAGS)

NamerTests: test/NamerTests.cpp Namer.h
	$(FORMAT) test/NamerTests.cpp
	$(CHECK) --suppress=unmatchedSuppression test/NamerTests.cpp
	g++ $(INCLUDE) $(LINK) test/NamerTests.cpp -o test/bin/NamerTests -g $(CFLAGS)

StringWriterTests: test/StringWriterTests.cpp StringWriter.h
	$(FORMAT) test/StringWriterTests.cpp
	$(CHECK) test/StringWriterTests.cpp
	g++ $(INCLUDE) $(LINK) test/StringWriterTests.cpp -o test/bin/StringWriterTests -g $(CFLAGS)

ReporterTests: test/reporters/ReporterTests.cpp reporters/Reporter.h
	$(FORMAT) test/reporters/ReporterTests.cpp
	$(CHECK) test/reporters/ReporterTests.cpp
	g++ $(INCLUDE) $(LINK) test/reporters/ReporterTests.cpp -o test/bin/ReporterTests -g $(CFLAGS)

FileApproverTests: test/FileApproverTests.cpp reporters/Reporter.h FileApprover.h
	$(FORMAT) test/FileApproverTests.cpp
	$(CHECK) test/FileApproverTests.cpp
	g++ $(INCLUDE) $(LINK) test/FileApproverTests.cpp -o test/bin/FileApproverTests -g $(CFLAGS)

ApprovalException.h: ApprovalException.gch

ApprovalException.gch:
	$(FORMAT) ApprovalException.h
	$(CHECK) ApprovalException.h
	g++ -c ApprovalException.h -O3 -Wall

CommandLauncher.h: reporters/CommandLauncher.h
	$(FORMAT) reporters/CommandLauncher.h
	$(CHECK) reporters/CommandLauncher.h
	g++ -c reporters/CommandLauncher.h -O3 -Wall

FileApprover.h: FileApprover.gch ApprovalException.h

FileApprover.gch: 
	$(FORMAT) FileApprover.h
	$(CHECK) FileApprover.h
	g++ $(INCLUDE) -c FileApprover.h -O3 -Wall

Reporter.h: reporters/Reporter.h reporters/CommandLauncher.h
	$(FORMAT) reporters/Reporter.h
	$(CHECK) reporters/Reporter.h
	g++ -c reporters/Reporter.h -O3 -Wall

Namer.h:
	$(FORMAT) Namer.h
	$(CHECK) Namer.h
	g++ $(INCLUDE) $(LINK) -c Namer.h -O3 -Wall -lboost_system -static

StringWriter.h:
	$(FORMAT) StringWriter.h
	$(CHECK) StringWriter.h
	g++ -c StringWriter.h -O3 -Wall

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: format
	cppcheck -v --inline-suppr --suppress=unmatchedSuppression --enable=all $(SRCS)

clean:
	rm -f ./test/bin/*Tests

# DO NOT DELETE