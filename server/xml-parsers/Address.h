#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>

class Address {

private:
    bool hasNumero;
    std::string numero;
    std::string rue;
    std::string ville;
    std::string codePostal;
    
public:
    Address();
    void setNumero(std::string numero);
    void setRue(std::string rue);
    void setVille(std::string ville);
    void setCodePostal(std::string codePostal);
    std::string getGoogleAdresse();
    std::string getGoogleAnswerAdress();    
};

#endif