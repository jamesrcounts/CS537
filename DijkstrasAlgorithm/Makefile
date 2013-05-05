CHECK = cppcheck -v --inline-suppr --enable=all
FORMAT = astyle -YfDHUbps4yj -k3
UNLOCK = chmod 666

CFLAGS = -g -O0
INC = -I ../igloo -I ../approvals -I dijkstra
TEST = test/bin/tests
TESTS = test/vertex.test.h test/edge.test.h test/graph.test.h
EDGE = dijkstra/obj/edge.o
VERTEX = dijkstra/obj/vertex.o
GRAPH = dijkstra/obj/graph.o
OBJS = $(VERTEX) $(EDGE) $(GRAPH)
HDRS = dijkstra/vertex.h dijkstra/edge.h dijkstra/graph.h
SRCS = dijkstra/vertex.cpp dijkstra/edge.cpp dijkstra/graph.cpp dijkstra/dijkstra.cpp
PROG = dijkstra/bin/dijkstra
BAKS = ${SRCS:.cpp=.cpp.orig} ${HDRS:.h=.h.orig}

all: $(TEST) $(OBJS) $(PROG) runtest

runtest:
	$(TEST)

clean:
	rm -f $(TEST) $(OBJS) $(PROG) $(BAKS)

format:
	$(FORMAT) $(TESTS) $(HDRS) $(SRCS)
	$(UNLOCK) $(TESTS) $(HDRS) $(SRCS)

check: format
	$(CHECK) $(HDRS) $(SRCS)

$(PROG): $(OBJS) dijkstra/dijkstra.cpp
	$(CXX) dijkstra/dijkstra.cpp $(OBJS) -o $(PROG) $(CFLAGS)

$(TEST): $(OBJS) $(TESTS) test/tests.cpp
	$(CXX) $(INC) test/tests.cpp $(OBJS) -o $(TEST) $(CFLAGS)

$(VERTEX): $(EDGE) dijkstra/vertex.h dijkstra/vertex.cpp
	$(CXX) -c dijkstra/vertex.cpp -o $(VERTEX) $(CFLAGS)

$(EDGE): dijkstra/edge.h dijkstra/edge.cpp
	$(CXX) -c dijkstra/edge.cpp -o $(EDGE)

$(GRAPH): $(EDGE) $(VERTEX) dijkstra/graph.h dijkstra/graph.cpp
	$(CXX) -c dijkstra/graph.cpp -o $(GRAPH) $(CFLAGS)