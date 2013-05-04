CC = g++
FLAGS = -ldijkstra -g -Wall
LI = -Ldijkstra/lib/
TI = -I ../igloo -I ../approvals -I dijkstra
CHECK = cppcheck -v --inline-suppr --enable=all
FORMAT = astyle -YfDHUbps4yj -k3
UNLOCK = chmod 666
TESTBIN = tests/bin/vertex_tests 
DIRS = tests/bin dijkstra/lib dijkstra/obj
LIB = dijkstra/lib/libdijkstra.a
OBJS = dijkstra/obj/Vertex.o dijkstra/obj/Graph.o
VERTEX = dijkstra/Vertex.h dijkstra/Vertex.cpp
GRAPH = dijkstra/Graph.h dijkstra/Graph.cpp
TESTS = tests/graphs.h tests/nodes.h tests/dalgorithm.h

all: runtests

runtests: $(TESTBIN)
	tests/bin/vertex_tests

$(TESTBIN): $(DIRS) $(LIB) $(VERTEX) $(GRAPH) tests/vertex_tests.cpp $(TESTS)
	$(CHECK) tests/vertex_tests.cpp $(TESTS)
	$(FORMAT) tests/vertex_tests.cpp $(TESTS)
	$(UNLOCK) tests/vertex_tests.cpp $(TESTS)
	$(CC) $(TI) $(LI) tests/vertex_tests.cpp $(TESTS) -o tests/bin/vertex_tests $(FLAGS)

$(LIB): $(DIRS) $(OBJS)
	ar rcs $(LIB) $(OBJS)

dijkstra/obj/Vertex.o: $(DIRS) $(VERTEX)
	$(CHECK) $(VERTEX)
	$(FORMAT) $(VERTEX)
	$(UNLOCK) $(VERTEX)
	$(CC) -c dijkstra/Vertex.cpp -o dijkstra/obj/Vertex.o -g -Wall

dijkstra/obj/Graph.o: $(DIRS) $(GRAPH)
	$(CHECK) $(GRAPH)
	$(FORMAT) $(GRAPH)
	$(UNLOCK) $(GRAPH)
	$(CC) -c dijkstra/Graph.cpp -o dijkstra/obj/Graph.o -g -Wall

tests/bin:
	mkdir tests/bin

dijkstra/lib:
	mkdir dijkstra/lib

dijkstra/obj:
	mkdir dijkstra/obj