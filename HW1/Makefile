###################################
# Makefile
# HW 1
# CS537 - Data Communications - Spring 2013
# Jim Counts
###################################
CC = g++
INCLUDE = -I ../igloo
CFLAGS = -O2 -Wall -lpthread
SRCS = httpClient.cpp

all:  httpClient check

httpClient: httpClient.cpp
	g++ httpClient.cpp -o httpClient

format: 
	astyle -YfDHUbps4yj -k3 $(SRCS)

check: format
	cppcheck -v --inline-suppr  --enable=all $(SRCS)

# DO NOT DELETE