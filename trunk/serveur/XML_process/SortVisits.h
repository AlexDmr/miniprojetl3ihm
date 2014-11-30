#include <string>
#include <vector>
#include <map>
/*
 * Classe qui contient un parser DOM pour modifier le fichier d'entrée
 *  en y ajoutant l'ordre des adresses à visiter
 */
namespace xmlpp {
    class Node;
    class Element;
}

class SortVisits {
    
public:
    /// Constructeur
    SortVisits();
    
    /// Desctructeur
    ~SortVisits();
    
    /// Méthode qui fait tout
    void processDistanceMatrix(char * inputFileName, char * outputFileName, int id);
    
    /// Faire le job...
    void modifyFile(const char * inputFilename, std::vector<std::string> * adresses, const char * outputFilename);
    
    /// Faire du xslt
    void saveXHTMLFile(char * inputXMLFile, char * outputXHTMLFile, int id);
    
protected:
    std::string getPatientNodeAdresse(xmlpp::Node * adresseNode);
    xmlpp::Element * findAdresseInMap(std::string sortedAdresse, std::map<std::string, xmlpp::Element *> map);
};