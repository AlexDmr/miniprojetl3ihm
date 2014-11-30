#include <string>

class Adresse {

private:
    bool hasNumero;
    std::string numero;
    std::string rue;
    std::string ville;
    std::string codePostal;
    
public:
    Adresse();
    void setNumero(std::string numero);
    void setRue(std::string rue);
    void setVille(std::string ville);
    void setCodePostal(std::string codePostal);
    std::string getGoogleAdresse();
    std::string getGoogleAnswerAdress();    
};