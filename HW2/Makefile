###################################
# Makefile
# HW 1
# CS537 - Data Communications - Spring 2013
# Jim Counts
###################################
CC = g++
INCLUDE = -I ../igloo
CFLAGS = -lpthread -Wall
SRCS = test.cpp *.h

all: format runtest check InternetSocket.gch SocketConnection.gch TcpListeningSocket.gch TcpSocket.gch

runtest: test
	./test
	
test: test.cpp *.h
	g++ $(INCLUDE) test.cpp -o test -g $(CFLAGS)

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: format
	cppcheck -v --inline-suppr --enable=all $(SRCS)

clean:
	rm -f test *.orig *.gch

InternetSocket.gch: InternetSocket.h
	$(CC) -c InternetSocket.h $(CFLAGS)

SocketConnection.gch: SocketConnection.h
	$(CC) -c SocketConnection.h $(CFLAGS)

TcpListeningSocket.gch: TcpListeningSocket.h
	$(CC) -c TcpListeningSocket.h $(CFLAGS)

TcpSocket.gch: TcpSocket.h
	$(CC) -c TcpSocket.h $(CFLAGS)

# DO NOT DELETE