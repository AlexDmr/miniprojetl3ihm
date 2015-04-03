#include <iostream>
#include <curl.h>

#include "Address.h"

#include "FromXMLToGoogleMapHTTPRequest.h"

// Constructeur
FromXMLToGoogleMapHTTPRequest::FromXMLToGoogleMapHTTPRequest() : LwSaxParser()
{
    // On initialise les attributs...
    isCabinet = false;
    nurseId = "";
    currentState = OTHER;
    addressList = "";
    request = "";
    currentAddress = NULL;
}

FromXMLToGoogleMapHTTPRequest::~FromXMLToGoogleMapHTTPRequest() {
    // On libère l'adresse courante
    if (currentAddress != NULL) {
        delete currentAddress;
    }    
}


char * FromXMLToGoogleMapHTTPRequest::getGoogleHttpRequest(char * dataBaseFileName, int nurseNumber) {
    char * result;
    // Convertir l'entier nurseNumber en std::string pour pouvoir le stocker dans l'attribut id
    // Possible en C -> lire l'API doc de la méthode itoa ou sprintf
    // Plus simple en C++ en utilisant la STL
    std::ostringstream nbStr;
    if (nurseNumber < 100) {
        nbStr << "0";
    }
    if (nurseNumber < 10) {
        nbStr << "0";
    }
    nbStr << nurseNumber;
    nurseId = nbStr.str();
    std::cout << "Nurse Id: " << nurseId.c_str() << std::endl;
    
    // Parser le docuemnt
    parseDocument(dataBaseFileName);
    
    // Convertir la requête au format std::string en char *
    //    result = request.c_str(); -> ne fonctionne pas car request.c_str() renvoie quelque chose de const
    result = new char[request.length() + 1];
    strcpy(result, request.c_str());
        
    // Renvoyer le résultat    
    return result;
    
}

// Ce qui se passe lorsqu'on commence le parcours d'un docuemnt
void FromXMLToGoogleMapHTTPRequest::on_start_document() {
    // On ré-initialise les attributs
    // Imaginons que l'on a déjà parsé un fichier et que l'on souhaite en parser un deuxième...
    // Il faut alors remettre à jour les attributs du parseur !
    // Attention, ne pas toucher à nurseId, qui a été renseigné dans en attribut de la méthode getGoogleHttpRequest !
    isCabinet = false;
    addressList = "";
    request = "";
    currentAddress = NULL;
    // On se met dans l'état START pour pouvoir démarrer...
    currentState = START;
}

// Ce qui se passe lorsque l'on termine le parcours du document
void FromXMLToGoogleMapHTTPRequest::on_end_document() {
    // Une fois que l'on a terminé le parsing du document, on converti la liste d'adresse en requête Google
    // et on la stocke dans l'attribut request de type std::string (plus facile à manipuler que char *).
     request = "origins=" + std::string(curl_easy_escape(NULL,addressList.c_str(),0)) + "&destinations=" + std::string(curl_easy_escape(NULL,addressList.c_str(),0));
}

/** 
 * Ce qui se passe lorsque l'on commence le parcours d'un élément
 * @parameter name nom de l'élément
 * @parameter properties Liste des attributs de l'élément
 */
void FromXMLToGoogleMapHTTPRequest::on_start_element(const Glib::ustring& name, const AttributeList& properties) {
    // Lorsque l'on arrive sur un nouveau patient ou à l'adrsse du cabinet, on reset l'adresse...
    switch(currentState) {
    case START:
        if (name == "patient") {
            currentState = PATIENT;
        }
        if (name == "adresse") {
            currentState = ADRESSE;
            isCabinet = true;
            currentAddress = new Address();
        }
        break;
    case PATIENT:
        if (name == "adresse") {
            currentState = ADRESSE;
            isCabinet = false;
            currentAddress = new Address();
        }
        if (name == "visite") {
            currentState = VISITE;
            std::string itervenant = findAttribute(properties, "intervenant");
            // Si l'intervenant est le bon
            if ((itervenant != "") && (itervenant == nurseId)) {
                // On ajoute l'adresse à la liste d'adresse
                addressList += "|" + currentAddress->getGoogleAdresse();
            }
            // Il n'y a pas de Garbage Collector en C++...
            // C'est à nous de détruire les instances avant qu'elles soient déréférencées !
            if (currentAddress != NULL) {
                delete currentAddress;
                currentAddress = NULL;                
            }
        }
        break;
    case ADRESSE:
        if (name == "étage") {
            currentState = ETAGE;
        }
        if (name == "numéro") {
            currentState = NUMERO;
        }
        if (name == "rue") {
            currentState = RUE;
        }
        if (name == "ville") {
            currentState = VILLE;
        }
        if (name == "numéro") {
            currentState = NUMERO;
        }
        if (name == "codePostal") {
            currentState = CODEPOSTAL;
        }
        break;
    default:
        break;
    }
}

/**
 * Ce qui se passe lorsque l'on sort d'un élément
 * @parameter name nom de l'élément
 *
 */
void FromXMLToGoogleMapHTTPRequest::on_end_element(const Glib::ustring& name) {
    switch(currentState) {
    case START:
    break;
    case PATIENT:
    // Attention ! Il y a plusieurs éléments dans PATIENT, il faut vérifier que le
    // nom de l'élément duquel on sort est bien patient !
    if (name == "patient") {
        currentState = START;
    }
    break;
    case ADRESSE:
    if (isCabinet) {
        // Si l'on sort de l'adresse du cabinet, on l'ajoute à la liste d'adresses
        addressList += currentAddress->getGoogleAdresse();
        delete currentAddress;
        currentAddress = NULL;
        currentState = START;
    }
    else {
        currentState = PATIENT;
    }
    break;
    case ETAGE:
    case NUMERO:
    case RUE:
    case VILLE:
    case CODEPOSTAL:
    currentState = ADRESSE;
    break;
    case VISITE:
    currentState = PATIENT;
    break;
    default:
    break;
    }
}

/* 
 * Ce qui se passe lorsque l'on rencontre une chaîne de caractères
 * @parameter characters la liste de caractères en question
 */
void FromXMLToGoogleMapHTTPRequest::on_characters(const Glib::ustring& characters) {
    switch (currentState) {
        // Cas où il faut noter l'adresse courante (on verra plus tard si on la sauvegarde dans la liste d'adresses)
        case NUMERO:
        if (currentAddress != NULL) {
            currentAddress->setNumero(characters);
        }
        break;
        case RUE:
        if (currentAddress != NULL) {
            currentAddress->setRue(characters);
        }
        break;
        case VILLE:
        if (currentAddress != NULL) {
            currentAddress->setVille(characters);
        }
        break;
        case CODEPOSTAL:
        if (currentAddress != NULL) {
            currentAddress->setCodePostal(characters);
        }
        break;                
    }
}

/*
 * Méthode d'aide qui permet de trouver un attribut qui a un certain nom dans une liste d'attribut.
 * Cette méthode existe de base dans l'API Sax de Java, mais pas ici, c'est pourquoi elle est donnée.
 */
std::string FromXMLToGoogleMapHTTPRequest::findAttribute(const AttributeList& attributeList, std::string attributeName) {
    std::string resultat = "";
    xmlpp::SaxParser::AttributeList::const_iterator iter = attributeList.begin();
    while ((iter != attributeList.end()) && (iter->name != attributeName)) {
        iter++;
    }
    if (iter != attributeList.end()) {
        resultat = iter->value;
    }
    return resultat;
}
