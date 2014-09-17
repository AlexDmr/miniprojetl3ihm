#include "FromGoogleMapXMLToDistanceTable.h"

// Constructeur
FromGoogleMapXMLToDistanceTable::FromGoogleMapXMLToDistanceTable() : LwSaxParser() {
    
}

FromGoogleMapXMLToDistanceTable::~FromGoogleMapXMLToDistanceTable() {
    
}


std::vector<std::string> FromGoogleMapXMLToDistanceTable::getAdresses() {
    std::vector<std::string> adresses;
    adresses.push_back("46 Avenue Félix Viallet, IUT 2, 38031 Grenoble, France");
    adresses.push_back("Rond-point de la Croix de Vie, 38700 La Tronche, France");
    adresses.push_back("Rue Casimir Brenier, 38000 Grenoble, France");
    adresses.push_back("25 Rue des Martyrs, 38000 Grenoble, Francev");
    
    return adresses;
}

std::vector< std::vector<int> > FromGoogleMapXMLToDistanceTable::getDistances() {
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
    
}