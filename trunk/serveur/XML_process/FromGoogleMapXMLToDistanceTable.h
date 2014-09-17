#include "LwSaxParser.h"
#include <vector>

class FromGoogleMapXMLToDistanceTable : public LwSaxParser {
    
public:
    /// Constructeur
    FromGoogleMapXMLToDistanceTable();
    
    /// Desctructeur
    ~FromGoogleMapXMLToDistanceTable();
    
    /// Liste des adresses
    std::vector<std::string> getAdresses();
    
    /**
     * Distance en mètres entre les adresses
     * Le vecteur général est un vecteur de ligne.
     * Sur chaque ligne numéro i, il y a la distance entre l'adresses numéro i de getAdresses
     *  et chacune des adresses numéro j (à la colonne j).
     */
    std::vector< std::vector<int> > getDistances();
    
protected:
    /// Méthodes à réimplémenter de la classe LWSaxParser...
    
};