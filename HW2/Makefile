###################################
# Makefile
# HW 1
# CS537 - Data Communications - Spring 2013
# Jim Counts
###################################
CC = g++
INCLUDE = -I ../igloo -I ../approvals
CFLAGS = -lpthread -Wall
SRCS = *.h ./tests/*.cpp

all: format runtest check InternetSocket.gch SocketConnection.gch TcpListeningSocket.gch TcpSocket.gch

runtest: listening_test client_test http_test
	./tests/listening_test
	./tests/client_test
	./tests/bin/http_test

listening_test: ./tests/listening_test.cpp *.h
	g++ $(INCLUDE) ./tests/listening_test.cpp -o ./tests/listening_test -g $(CFLAGS)

client_test: ./tests/client_test.cpp *.h
	$(CC) $(INCLUDE) ./tests/client_test.cpp -o ./tests/client_test -g $(CFLAGS)

http_test: ./tests/http_test.cpp *.h
	$(CC) $(INCLUDE) ./tests/http_test.cpp -o ./tests/bin/http_test -g $(CFLAGS)

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: format
	cppcheck -v --inline-suppr --enable=all $(SRCS)

clean:
	rm -f ./tests/listening_test ./tests/client_test ./tests/test *.orig *.gch

InternetSocket.gch: InternetSocket.h
	$(CC) -c InternetSocket.h $(CFLAGS)

SocketConnection.gch: SocketConnection.h
	$(CC) -c SocketConnection.h $(CFLAGS)

TcpListeningSocket.gch: TcpListeningSocket.h
	$(CC) -c TcpListeningSocket.h $(CFLAGS)

TcpSocket.gch: TcpSocket.h
	$(CC) -c TcpSocket.h $(CFLAGS)

# DO NOT DELETE