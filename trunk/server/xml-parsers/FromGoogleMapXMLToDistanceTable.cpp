#include <iostream>
#include "FromGoogleMapXMLToDistanceTable.h"

// Constructeur
FromGoogleMapXMLToDistanceTable::FromGoogleMapXMLToDistanceTable() : LwSaxParser() {
    state = UNKNOWN;
    adresseCourante = "";
    adresses = NULL;
    numeroLigne = -1;
    ligne = NULL;
    distanceMatrix = NULL;
}


FromGoogleMapXMLToDistanceTable::~FromGoogleMapXMLToDistanceTable() {
    
}


std::vector<std::string> * FromGoogleMapXMLToDistanceTable::getAdresses() {
    /*
    std::vector<std::string> adresses;
    adresses.push_back("46 Avenue Félix Viallet, IUT 2, 38031 Grenoble, France");
    adresses.push_back("Rond-point de la Croix de Vie, 38700 La Tronche, France");
    adresses.push_back("Rue Casimir Brenier, 38000 Grenoble, France");
    adresses.push_back("25 Rue des Martyrs, 38000 Grenoble, Francev");
    */
    return adresses;
}

std::vector< std::vector<int> > * FromGoogleMapXMLToDistanceTable::getDistances() {
    /*
    std::vector< std::vector<int> > distances;
    std::vector<int> ligne0;
    ligne0.push_back(0);
    ligne0.push_back(3430);
    ligne0.push_back(224);
    ligne0.push_back(3000);
    distances.push_back(ligne0);
    
    std::vector<int> ligne1;
    ligne1.push_back(3345);
    ligne1.push_back(0);
    ligne1.push_back(3569);
    ligne1.push_back(5625);
    distances.push_back(ligne1);
    
    std::vector<int> ligne2;
    ligne2.push_back(310);
    ligne2.push_back(3337);
    ligne2.push_back(0);
    ligne2.push_back(2907);
    distances.push_back(ligne2);
    
    std::vector<int> ligne3;
    ligne3.push_back(2543);
    ligne3.push_back(5569);
    ligne3.push_back(2326);
    ligne3.push_back(0);
    distances.push_back(ligne3);
    
    return distances;
    */
    
    return distanceMatrix;
    
}

void FromGoogleMapXMLToDistanceTable::on_start_document() {
    state = START;
    adresses = new std::vector<std::string>();
    distanceMatrix = new std::vector< std::vector<int> >();
}

void FromGoogleMapXMLToDistanceTable::on_end_document() {
    
}

void FromGoogleMapXMLToDistanceTable::on_start_element(const Glib::ustring& name, const AttributeList& properties) {
    switch (state) {
    case START:
        if (name == "origin_address") {
            state = ORIGIN_ADDRESS;   
            adresseCourante = "";         
        }
        else if (name == "row") {
            state = ROW;
            ligne = new std::vector<int>();            
        }
        break;
    
    case ROW:
        if (name == "element") {            
            state = ELEMENT;
            adresseCourante = "";
        }        
        break;   
    case ELEMENT:
        if (name == "distance") {
            state = DISTANCE;
        }            
        break;
    case DISTANCE:
        if (name == "value") {
            state = VALUE;
        }
        break;
    default:
        break;
    }
}

void FromGoogleMapXMLToDistanceTable::on_end_element(const Glib::ustring& name) {
    switch (state) 
    {
    case VALUE:
        state = DISTANCE;
        break;
    case DISTANCE:
        if (name == "distance") {
            state = ELEMENT;
        }
        break;
    case ELEMENT:
        if (name == "element") {
            state = ROW;
        }
        break;
    case ROW:
        if (name == "row") {           
            std::vector<int> ligneTmp;
            for (int i = 0; i < ligne->size(); i++) {
                ligneTmp.push_back(ligne->at(i));
            }
            distanceMatrix->push_back(ligneTmp);
            state = START;
        }
        break;
    case ORIGIN_ADDRESS:
        if (name == "origin_address") {
            adresses->push_back(adresseCourante);
            state = START;
        }
        break;
        
    default:
        break;
        
    }
    
}

void FromGoogleMapXMLToDistanceTable::on_characters(const Glib::ustring& characters) {
    int val;
    switch(state) 
    {
    case ORIGIN_ADDRESS:
        adresseCourante += std::string(characters);        
        break;
    case VALUE:
        sscanf(characters.c_str(), "%d", &val);
        std::cout << "Dans VALUE, val: " << val << std::endl;
        ligne->push_back(val);
        break;
    default:
        break;
    }
}

