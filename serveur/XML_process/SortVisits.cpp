#include "SortVisits.h"
#include <iostream>
#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/DOCBparser.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

// Constructeur
SortVisits::SortVisits() {
    
}

SortVisits::~SortVisits() {
    
}


/// Faire le job...
void SortVisits::modifyFile(char * filename, std::vector<std::string> * adresses) {
    std::cout << " Modifying xml file..." << std::endl;
}

//See http://libxmlplusplus.sourceforge.net/docs/manual/html/ar01s02.html
void SortVisits::saveXHTMLFile(char * inputXMLFile, char * outputXHTMLFile, int id)
{
    FILE *outFile = NULL;
//    const xmlChar * xsltFile = (const xmlChar *) ("data/ex.xsl");
    const xmlChar * xmlFile  = (const xmlChar *) (inputXMLFile);
    const char *params[16 + 1];
    	
    int nbparams = 0;
    xsltStylesheetPtr cur = NULL;
    xmlDocPtr doc, res;
        
    char * nurseId = new char[4];
    sprintf(nurseId, "00%d", id);
        
    std::cout <<"Save XHTML File: input " << inputXMLFile << ", output: " << outputXHTMLFile << " with nurseId: " << nurseId << std::endl;

	params[0] = "destinedId";
    params[1] = nurseId;
    params[2] = NULL;
    
	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;
	cur = xsltParseStylesheetFile((const xmlChar *) ("data/cabinetToInfirmier.xsl"));

	doc = xmlParseFile(inputXMLFile);
    
	res = xsltApplyStylesheet(cur, doc, params);
    outFile=(fopen(outputXHTMLFile,"w"));
       if(outFile==NULL){
           printf("Error!");
           exit(1);
       }
    xsltSaveResultToFile(outFile, res, cur);

    fclose(outFile);
    
	xsltFreeStylesheet(cur);
	xmlFreeDoc(res);
	xmlFreeDoc(doc);

    xsltCleanupGlobals();
    xmlCleanupParser();

    
}