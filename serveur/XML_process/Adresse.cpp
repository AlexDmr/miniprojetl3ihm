#include "Adresse.h"

Adresse::Adresse() {
    hasNumero = false;
    numero = "";
    rue = "";
    ville = "";
    codePostal = "";
}

void Adresse::setNumero(std::string numero) {
    this->numero = numero;
    this->hasNumero = true;
}
void Adresse::setRue(std::string rue) {
    this->rue = rue;
}
void Adresse::setVille(std::string ville) {
    this->ville = ville;
}
void Adresse::setCodePostal(std::string codePostal) {
    this->codePostal = codePostal;
}

std::string Adresse::getGoogleAdresse() {
    std::string resultat = ville;
    resultat += "+" + codePostal;
    if (hasNumero) {
        resultat += "+" + numero;
    }
    resultat += "+" + rue;
    
    return resultat;
}

std::string Adresse::getGoogleAnswerAdress() {
    std::string adresse = "";
    if (! numero.empty()) {
        adresse += numero + " ";
    }
    adresse += rue + ", ";
    adresse += codePostal + " ";
    adresse += ville;
    
    return adresse;
}