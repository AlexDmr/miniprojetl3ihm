#include <iostream>

#include <curl.h>
#include "Adresse.h"
#include "FromXMLToGoogleMapHTTPRequest.h"

// Constructeur
FromXMLToGoogleMapHTTPRequest::FromXMLToGoogleMapHTTPRequest(std::string id) : LwSaxParser()
{
    this->id = "00" + id;
    
    request = "";
    listeAdresses = "";
    state = OTHER;
}

FromXMLToGoogleMapHTTPRequest::~FromXMLToGoogleMapHTTPRequest() {
    
}

std::string FromXMLToGoogleMapHTTPRequest::getGoogleMapHttpRequest() {
    return request;
}

std::string FromXMLToGoogleMapHTTPRequest::getGoogleMapHttpRequest_V2() {
    return "origins=" + std::string(curl_easy_escape(NULL,"Grenoble+38031+46+avenue+Félix+Viallet|La Tronche+38700+Rond-Point+de+la+Croix+de+Vie|Grenoble+38000+rue+Casimir+Brenier|Grenoble+38042+25+rue+des+Martyrs",0)) + "&destinations=" + std::string(curl_easy_escape(NULL,"Grenoble+38031+46+avenue+Félix+Viallet|La Tronche+38700+Rond-Point+de+la+Croix+de+Vie|Grenoble+38000+rue+Casimir+Brenier|Grenoble+38042+25+rue+des+Martyrs",0));
}

/// Ce qui se passe lorsqu'on commence le parcours d'un docuemnt
void FromXMLToGoogleMapHTTPRequest::on_start_document() {
    request = "";
    listeAdresses = "";    
    state = START;
    adresseCourante = NULL;
}
/// Ce qui se passe lorsque l'on termine le parcours du document
void FromXMLToGoogleMapHTTPRequest::on_end_document() {
    request = "origins=" + std::string(curl_easy_escape(NULL,listeAdresses.c_str(),0)) + "&destinations=" + std::string(curl_easy_escape(NULL,listeAdresses.c_str(),0));
}

/** 
 * Ce qui se passe lorsque l'on commence le parcours d'un élément
 * @parameter name nom de l'élément
 * @parameter properties Liste des attributs de l'élément
 */
void FromXMLToGoogleMapHTTPRequest::on_start_element(const Glib::ustring& name, const AttributeList& properties) {
    // Lorsque l'on arrive sur un nouveau patient ou à l'adrsse du cabinet, on reset l'adresse...
    switch(state) {
    case START:
        if (name == "patient") {
            state = PATIENT;
        }
        if (name == "adresse") {
            state = ADRESSE;
            adresseCabinet = true;
            adresseCourante = new Adresse();
        }
        break;
    case PATIENT:
        if (name == "adresse") {
            state = ADRESSE;
            adresseCabinet = false;
            adresseCourante = new Adresse();
        }
        if (name == "visite") {
            state = VISITE;
            std::string itervenant = findAttribute(properties, "intervenant");
            // Si l'intervenant est le bon
            if ((itervenant != "") && (itervenant == id)) {
                //std::cout << "intervenant: " << itervenant << std::endl;
                // On ajoute l'adresse à la liste d'adresse
                listeAdresses += "|" + adresseCourante->getGoogleAdresse();
            }
            // dans tous les cas, détruire l'adresse
            delete adresseCourante;
            adresseCourante = NULL;
        }
        break;
    case ADRESSE:
        if (name == "étage") {
            state = ETAGE;
        }
        if (name == "numéro") {
            state = NUMERO;
        }
        if (name == "rue") {
            state = RUE;
        }
        if (name == "ville") {
            state = VILLE;
        }
        if (name == "numéro") {
            state = NUMERO;
        }
        if (name == "codePostal") {
            state = CODEPOSTAL;
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
    switch(state) {
    case START:
    break;
    case PATIENT:
    // Attention ! Il y a plusieurs éléments dans PATIENT, il faut vérifier que le no
    // est bien le nom pour sortir de l'état patient !
    if (name == "patient") {
        state = START;
    }
    break;
    case ADRESSE:
    if (adresseCabinet) {
        // Si l'on sort de l'adresse du cabinet, on l'ajoute à la liste d'adresses
        listeAdresses += adresseCourante->getGoogleAdresse();
        delete adresseCourante;
        adresseCourante = NULL;
        state = START;
    }
    else {
        state = PATIENT;
    }
    break;
    case ETAGE:
    case NUMERO:
    case RUE:
    case VILLE:
    case CODEPOSTAL:
    state = ADRESSE;
    break;
    case VISITE:
    state = PATIENT;
    break;
    default:
    break;
    }
}

/** 
 * Ce qui se passe lorsque l'on rencontre une chaîne de caractères
 * @parameter characters la liste de caractères en question
 */
void FromXMLToGoogleMapHTTPRequest::on_characters(const Glib::ustring& characters) {
    switch (state) {
        // Cas où il faut noter l'adresse courante (on verra plus tard si on la sauvegarde dans la liste d'adresses)
        case NUMERO:
        if (adresseCourante != NULL) {
            adresseCourante->setNumero(characters);
        }
        break;
        case RUE:
        if (adresseCourante != NULL) {
            adresseCourante->setRue(characters);
        }
        break;
        case VILLE:
        if (adresseCourante != NULL) {
            adresseCourante->setVille(characters);
        }
        break;
        case CODEPOSTAL:
        if (adresseCourante != NULL) {
            adresseCourante->setCodePostal(characters);
        }
        break;                
    }
    // switch (state) {
    //     case START:
    //     std::cout << "START ";
    //     break;
    //     case PATIENT:
    //     std::cout << "PATIENT ";
    //     break;
    //     case ADRESSE:
    //     std::cout << "ADRESSE ";
    //     break;
    //     case ETAGE:
    //     std::cout << "ETAGE ";
    //     break;
    //     case NUMERO:
    //     std::cout << "NUMERO ";
    //     break;
    //     case RUE:
    //     std::cout << "RUE ";
    //     break;
    //     case VILLE:
    //     std::cout << "VILLE ";
    //     break;
    //     case CODEPOSTAL:
    //     std::cout << "CODEPOSTAL ";
    //     break;
    //     case VISITE:
    //     std::cout << "VISITE ";
    //     break;
    //     case OTHER:
    //     std::cout << "OTHER ";
    //     break;
    // }
}

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
