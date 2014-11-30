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

#include "Adresse.h"
#include "FromGoogleMapXMLToDistanceTable.h"

// Constructeur
SortVisits::SortVisits() {
    
}

SortVisits::~SortVisits() {
    
}

void SortVisits::processDistanceMatrix(char * inputFileName, char * outputFileName, int id) {
    FromGoogleMapXMLToDistanceTable googleMapParser;
    std::vector<std::string> * adresses;
    std::vector< std::vector<int> > * distances;

    adresses = googleMapParser.getAdresses();
    distances = googleMapParser.getDistances();
    
    // Ici, il faut appeler la fonction développée en RO

    std::string inputStd(inputFileName);
    std::string tmpFileName = inputStd.substr(0, inputStd.find_last_of("."));
    tmpFileName += "-sorted.xml";
    
    modifyFile(inputFileName, adresses, tmpFileName.c_str());
    saveXHTMLFile(inputFileName, outputFileName, id);
    
}

std::string SortVisits::getPatientNodeAdresse(xmlpp::Node * adresseNode) {
    Adresse adresse;
    xmlpp::Node * node;
    xmlpp::Element * element;
    
    xmlpp::Node::PrefixNsMap nsMap;
    nsMap["cab"] = "http://www.ujf-grenoble.fr/l3miage/cabinet";
    
    // On récupère le numéro s'il existe
    if (adresseNode->find("cab:numéro", nsMap).size() > 0) {
        node = adresseNode->find("cab:numéro", nsMap).at(0);
        element = dynamic_cast <xmlpp::Element *> (node);
        if (element != NULL) {
            std::string numero = element->get_child_text()->get_content();
            adresse.setNumero(numero);
        }
    }
    
    // On récupère la rue
    node = adresseNode->find("cab:rue", nsMap).at(0);
    element = dynamic_cast <xmlpp::Element *> (node);
    if (element != NULL) {
        std::string rue = element->get_child_text()->get_content();
        adresse.setRue(rue);
    }
    
    // On récupère la ville
    node = adresseNode->find("cab:ville", nsMap).at(0);
    element = dynamic_cast <xmlpp::Element *> (node);
    if (element != NULL) {
        std::string rue = element->get_child_text()->get_content();
        adresse.setVille(rue);
    }
    
    // On récupère le code postal
    node = adresseNode->find("cab:codePostal", nsMap).at(0);
    element = dynamic_cast <xmlpp::Element *> (node);
    if (element != NULL) {
        std::string code = element->get_child_text()->get_content();
        adresse.setCodePostal(code);
    }
    
    return adresse.getGoogleAnswerAdress();
}
/// Faire le job...
void SortVisits::modifyFile(const char * inputFilename, std::vector<std::string> * adresses, const char * outputFilename) {
    std::cout << " Modifying xml file..." << std::endl;
    xmlpp::DomParser parser;
    parser.parse_file(inputFilename);
    if(parser)
    {
      //Walk the tree:
      xmlpp::Document * doc = parser.get_document();
      const xmlpp::Node* node = doc->get_root_node(); //deleted by DomParser.

      // Lorsque l'on a un namespace par défaut, il faut impérativement dans libxml (et libxml++)
      //   déclarer un préfixe pour ce namespace.
      xmlpp::Node::PrefixNsMap nsMap;
      nsMap["cab"] = "http://www.ujf-grenoble.fr/l3miage/cabinet";

      // --------------- On ordonne les patients selon leur adresse
      // On recherche et stocke tous les patients dans une map avec pour
      // clé leur adresse
      std::map<std::string, xmlpp::Element *> patientsAdresses;
      // On doit construire les adresse de la même manière que lors du parsing
      xmlpp::Node * patientsNode = node->find("//cab:patients", nsMap).at(0);
      xmlpp::NodeSet allPatients = patientsNode->find("cab:patient", nsMap);
      for(xmlpp::NodeSet::iterator iter = allPatients.begin(); iter != allPatients.end(); ++iter)
          {
              xmlpp::Element * patient = dynamic_cast<xmlpp::Element *>(*iter);
              // Récupérer l'adresse du patient en question
              xmlpp::Node * adresseNode = patient->find("cab:adresse", nsMap).at(0);
              std::string adresse = getPatientNodeAdresse(adresseNode);
              std::cout << adresse  << std::endl;
              xmlpp::Document * newDoc = new xmlpp::Document();
              patientsAdresses[adresse] = newDoc->create_root_node_by_import(patient);
              patientsNode->remove_child(patient);              
          }
          
          // Maintenant que tous les patients ont été supprimés du document, il faut les
          //  remettre dans le bon ordre
          
          // On parcourt les adresses d'entrées, on les cherche dans la map, si on les trouve, on les remets
          std::vector<std::string>::iterator it;
          for (it = adresses->begin(); it != adresses->end(); it++) {
              std::string sortedAdresse = (*it);
              xmlpp::Element * element = findAdresseInMap(sortedAdresse, patientsAdresses);
              if (element != NULL) {
                  std::cout << "Child found !!, element name: " << std::string(element->get_name()) << std::endl;
                  patientsNode->import_node(element);
              }
          }
          // Serialize the output file
          doc->write_to_file(outputFilename, "UTF-8");
    }
        
}


xmlpp::Element * SortVisits::findAdresseInMap(std::string sortedAdresse, std::map<std::string, xmlpp::Element *> map) {
    std::string sortedLower = sortedAdresse;
    std::transform(sortedLower.begin(), sortedLower.end(), sortedLower.begin(), ::tolower);
    std::cout << std::endl << "Searching for adresse: " << sortedLower << std::endl;

    xmlpp::Element * element = NULL;
    std::map<std::string, xmlpp::Element *>::iterator it = map.begin();
    while ((it != map.end()) && (element == NULL)) {
        std::string mapLower = (it->first);
        std::transform(mapLower.begin(), mapLower.end(), mapLower.begin(), ::tolower);
    
        std::cout << "it adresse:     " << mapLower << std::endl;
        
        if (sortedLower.find(mapLower) != std::string::npos) {
            element = it->second;
            std::cout << "Found !!" << std::endl;
        }
        it++;
    }    

    return element;
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