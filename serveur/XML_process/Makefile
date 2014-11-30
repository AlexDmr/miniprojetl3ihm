CC=g++
CFLAGS= -Wall -Wextra
INCLUDEFLAGS= $(shell pkg-config --cflags libxml++-2.6) -I/opt/local/include/curl
LDLIBS=$(shell pkg-config --libs libxml++-2.6) -lcurl -lxslt

all: testParsers

testParsers: testParsers.o LwSaxParser.o FromXMLToGoogleMapHTTPRequest.o FromGoogleMapXMLToDistanceTable.o SortVisits.o Adresse.o
	$(CC) $(LDLIBS) Adresse.o SortVisits.o FromGoogleMapXMLToDistanceTable.o FromXMLToGoogleMapHTTPRequest.o LwSaxParser.o testParsers.o -o testParsers 

testParsers.o : FromXMLToGoogleMapHTTPRequest.o LwSaxParser.o Adresse.o testParsers.c
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) testParsers.c -o testParsers.o

SortVisits.o : FromGoogleMapXMLToDistanceTable.o SortVisits.h SortVisits.cpp 
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) SortVisits.cpp -o SortVisits.o

LwSaxParser.o : LwSaxParser.h LwSaxParser.cpp
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) LwSaxParser.cpp -o LwSaxParser.o

FromXMLToGoogleMapHTTPRequest.o : FromXMLToGoogleMapHTTPRequest.h FromXMLToGoogleMapHTTPRequest.cpp LwSaxParser.h LwSaxParser.cpp Adresse.o
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) FromXMLToGoogleMapHTTPRequest.cpp -o FromXMLToGoogleMapHTTPRequest.o

FromGoogleMapXMLToDistanceTable.o : FromGoogleMapXMLToDistanceTable.h FromGoogleMapXMLToDistanceTable.cpp LwSaxParser.h LwSaxParser.cpp Adresse.h Adresse.cpp
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) FromGoogleMapXMLToDistanceTable.cpp -o FromGoogleMapXMLToDistanceTable.o

dresse.o : Adresse.h Adresse.cpp
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) Adresse.cpp -o Adresse.o
	
clean:
	rm -rf *o 

realclean: clean
	rm -rf testParsers