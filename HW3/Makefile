UNLOCK = chmod 666
SRCS = src/dijkstra.cpp
PROG = bin/dijkstra

all: $(PROG) unlock

unlock: 
	$(UNLOCK) Makefile $(SRCS)

$(PROG): $(SRCS)
	$(CXX) $(SRCS) -o $(PROG)