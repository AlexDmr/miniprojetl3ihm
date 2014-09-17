#include "LwSaxParser.h"
#include <string>

class FromXMLToGoogleMapHTTPRequest : public LwSaxParser {

public:
    /// Constructeur
    FromXMLToGoogleMapHTTPRequest(std::string id);
    
    /// Desctructeur
    ~FromXMLToGoogleMapHTTPRequest();
    
    std::string getGoogleMapHttpRequest();

    std::string getGoogleMapHttpRequest_V2();
    
protected:
    /// Méthodes à réimplémenter de la classe LWSaxParser...

};

