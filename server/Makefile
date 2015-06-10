CC=g++
CFLAGS= -Wall -Wextra
XMLPREFIX=./xml-parsers
OBJS=serveur-metier.o $(XMLPREFIX)/LwSaxParser.o $(XMLPREFIX)/FromXMLToGoogleMapHTTPRequest.o $(XMLPREFIX)/FromGoogleMapXMLToDistanceTable.o $(XMLPREFIX)/SortVisits.o $(XMLPREFIX)/Address.o 
INCLUDEFLAGS= -I$(XMLPREFIX) $(shell pkg-config --cflags libxml++-2.6) -I/opt/local/include/curl
LDLIBS=-L/opt/local/lib $(shell pkg-config --libs libxml++-2.6) -lcurl -lxslt

all: serveur-metier

serveur-metier: $(OBJS)
	$(CC) $(LDLIBS) $(OBJS) -o serveur-metier

serveur-metier.o : $(XMLPREFIX)/FromXMLToGoogleMapHTTPRequest.o $(XMLPREFIX)/LwSaxParser.o $(XMLPREFIX)/SortVisits.o $(XMLPREFIX)/Address.o serveur-metier.c
	$(CC) -c $(CFLAGS) $(INCLUDEFLAGS) serveur-metier.c -o serveur-metier.o

clean:
	rm -rf *o *~ FichierDataBase.xml FichierMatrice.xml FichierRes.html

realclean: clean
	rm -rf serveur-metier
