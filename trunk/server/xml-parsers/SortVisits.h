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
    void processDistanceMatrix(char * inputFileName, char * googleAnswer, int id, char * xslFilename, char * outputFileName);
    
    /// Ré-ordonne la liste des patients contenus dans inputFilename et correspondant à
    void modifyFile(const char * inputFilename, std::vector<std::string> * adresses, const char * outputFilename);
    
    /// Ouvre le fichier inputXMLFile, lui applique la transformation XSLT contenue dans le
    /// fichier xslTransformationFile avec le paramètre nurseId et sauvegarde le résultat dans le
    /// fichier outputXHTMLFile.
    void saveXHTMLFile(char * inputXMLFile, char * xslTransformationFile, char * outputXHTMLFile, int nurseId);
    
protected:
    std::string getPatientNodeAdresse(xmlpp::Node * adresseNode);
    xmlpp::Element * findAdresseInMap(std::string sortedAdresse, std::map<std::string, xmlpp::Element *> map);
};