CC=g++
CFLAGS= -Wall -Wextra
XMLPREFIX=./XML_process
OBJS=proxy-metier.o $(XMLPREFIX)/LwSaxParser.o $(XMLPREFIX)/FromXMLToGoogleMapHTTPRequest.o $(XMLPREFIX)/FromGoogleMapXMLToDistanceTable.o $(XMLPREFIX)/SortVisits.o $(XMLPREFIX)/Adresse.o 
INCLUDEFLAGS= -I$(XMLPREFIX) $(shell pkg-config --cflags libxml++-2.6) -I/opt/local/include/curl
LDLIBS=-L/opt/local/lib $(shell pkg-config --libs libxml++-2.6) -lcurl -lxslt

all: proxy-metier

proxy-metier: $(OBJS)
	$(CC) $(LDLIBS) $(OBJS) -o proxy-metier

proxy-metier.o : $(XMLPREFIX)/FromXMLToGoogleMapHTTPRequest.o $(XMLPREFIX)/LwSaxParser.o $(XMLPREFIX)/SortVisits.o $(XMLPREFIX)/Adresse.o proxy-metier.c
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) proxy-metier.c -o proxy-metier.o

clean:
	rm -rf *o *~ FichierDataBase.xml FichierMatrice.xml FichierRes.html

realclean: clean
	rm -rf proxy-metier
