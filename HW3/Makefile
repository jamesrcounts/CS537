UNLOCK = chmod 666
FORMAT = astyle -YfDHUbps4yj -k3

SRCS = src/dijkstra.cpp
PROG = bin/dijkstra

all: $(PROG) 

unlock: format
	$(UNLOCK) Makefile $(SRCS)

format:
	$(FORMAT) $(SRCS)	

$(PROG): $(SRCS)
	$(CXX) $(SRCS) -o $(PROG)