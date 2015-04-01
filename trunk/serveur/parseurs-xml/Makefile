CC=g++
CFLAGS= -Wall -Wextra
INCLUDEFLAGS= $(shell pkg-config --cflags libxml++-2.6) -I/usr/include/curl -I/opt/local/include/libxml++-2.6 -I/opt/local/lib/libxml++-2.6/include -I/opt/local/include/glibmm-2.4 -I/opt/local/lib/glibmm-2.4/include -I/opt/local/include/glib-2.0 -I/opt/local/lib/glib-2.0/include -I/opt/local/include/libxml2
LDLIBS=-L/opt/local/lib

all: testParsers

testParsers: testParsers.o LwSaxParser.o FromXMLToGoogleMapHTTPRequest.o FromGoogleMapXMLToDistanceTable.o SortVisits.o Address.o
	$(CC) Address.o SortVisits.o FromGoogleMapXMLToDistanceTable.o FromXMLToGoogleMapHTTPRequest.o LwSaxParser.o testParsers.o -o testParsers $(LDLIBS)  -lcurl -lxml++-2.6 -lglib-2.0 -lglibmm-2.4 -lxslt -lxml2

testParsers.o : FromXMLToGoogleMapHTTPRequest.o LwSaxParser.o testParsers.c
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) testParsers.c -o testParsers.o

LwSaxParser.o : LwSaxParser.h LwSaxParser.cpp
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) LwSaxParser.cpp -o LwSaxParser.o

FromXMLToGoogleMapHTTPRequest.o :  Address.h FromXMLToGoogleMapHTTPRequest.h FromXMLToGoogleMapHTTPRequest.cpp LwSaxParser.h LwSaxParser.cpp
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) FromXMLToGoogleMapHTTPRequest.cpp -o FromXMLToGoogleMapHTTPRequest.o

FromGoogleMapXMLToDistanceTable.o : FromGoogleMapXMLToDistanceTable.h FromGoogleMapXMLToDistanceTable.cpp LwSaxParser.h LwSaxParser.cpp
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) FromGoogleMapXMLToDistanceTable.cpp -o FromGoogleMapXMLToDistanceTable.o

SortVisits.o : SortVisits.h SortVisits.cpp Address.h
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) SortVisits.cpp -o SortVisits.o
	
Address.o : Address.h Address.cpp
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) Address.cpp -o Address.o
	
clean:
	rm -rf *o 

realclean: clean
	rm -rf testParsers