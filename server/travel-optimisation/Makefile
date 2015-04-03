CC=g++
CFLAGS= -Wall -Wextra 
INCLUDEFLAGS= -I/opt/local/include
LDLIBS=-L/opt/local/lib

all: testTSP

testTSP: testTSP.o tsp.o kruskal.o  Arete.o
	$(CC) testTSP.o tsp.o kruskal.o Arete.o -o testTSP 

testTSP.o : kruskal.o tsp.o testTSP.cpp
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) testTSP.cpp -o testTSP.o

tsp.o : kruskal.h tsp.h tsp.cpp
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) tsp.cpp -o tsp.o

kruskal.o :  Arete.h kruskal.h kruskal.cpp
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) kruskal.cpp -o kruskal.o

Arete.o : Arete.h Arete.cpp
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) Arete.cpp -o Arete.o
clean:
	rm -rf *o 

realclean: clean
	rm -rf testTSP