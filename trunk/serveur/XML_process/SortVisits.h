#include <string>
#include <vector>
/*
 * Classe qui contient un parser DOM pour modifier le fichier d'entrée
 *  en y ajoutant l'ordre des adresses à visiter
 */
class SortVisits {
    
public:
    /// Constructeur
    SortVisits();
    
    /// Desctructeur
    ~SortVisits();
    
    /// Faire le job...
    void modifyFile(char * filename, std::vector<std::string> adresses);
    
    /// Faire du xslt
    void saveXHTMLFile(char * inputXMLFile, char * outputXHTMLFile, int id);
    
protected:
    
};