CC=g++
CFLAGS= -Wall -Wextra
XMLPREFIX=./XML_process
OBJS=proxy-L3.o $(XMLPREFIX)/LwSaxParser.o $(XMLPREFIX)/FromXMLToGoogleMapHTTPRequest.o $(XMLPREFIX)/FromGoogleMapXMLToDistanceTable.o $(XMLPREFIX)/SortVisits.o $(XMLPREFIX)/Adresse.o 
INCLUDEFLAGS= -I$(XMLPREFIX) $(shell pkg-config --cflags libxml++-2.6) -I/opt/local/include/curl
LDLIBS=-L/opt/local/lib $(shell pkg-config --libs libxml++-2.6) -lcurl -lxslt

all: proxy-L3

proxy-L3: $(OBJS)
	$(CC) $(LDLIBS) $(OBJS) -o proxy-L3 

proxy-L3.o : $(XMLPREFIX)/FromXMLToGoogleMapHTTPRequest.o $(XMLPREFIX)/LwSaxParser.o $(XMLPREFIX)/SortVisits.o $(XMLPREFIX)/Adresse.o proxy-L3.c
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) proxy-L3.c -o proxy-L3.o

clean:
	rm -rf *o *~ TMP.xml TMP2.xml res.txt

realclean: clean
	rm -rf proxy-L3
