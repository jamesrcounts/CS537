UNLOCK = chmod 666
FORMAT = astyle -YfDHUbps4yj -k3

HDRS = src/Graph.h src/Parser.h
SRCS = src/dijkstra.cpp src/Graph.cpp src/Parser.cpp
PROG = bin/dijkstra

all: $(PROG) 

unlock: format
	$(UNLOCK) Makefile $(SRCS) $(HDRS)

format:
	$(FORMAT) $(SRCS) $(HDRS)

$(PROG): $(SRCS) $(HDRS)
	$(CXX) $(SRCS) -o $(PROG)