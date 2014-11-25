#include "LwSaxParser.h"
#include <vector>

class FromGoogleMapXMLToDistanceTable : public LwSaxParser {
    
public:
    enum PossibleStates {START, ORIGIN_ADDRESS, ROW, ELEMENT, DISTANCE, VALUE, UNKNOWN};
    
    /// Constructeur
    FromGoogleMapXMLToDistanceTable();
    
    /// Desctructeur
    ~FromGoogleMapXMLToDistanceTable();
    
    /// Liste des adresses
    std::vector<std::string> * getAdresses();
    
    /**
     * Distance en mètres entre les adresses
     * Le vecteur général est un vecteur de ligne.
     * Sur chaque ligne numéro i, il y a la distance entre l'adresses numéro i de getAdresses
     *  et chacune des adresses numéro j (à la colonne j).
     */
    std::vector< std::vector<int> > * getDistances();
    
protected:
   /** 
    * Méthodes ré-implemntées (surchargées) de la classe SaxParser
    * @{
    */
   /// Ce qui se passe lorsqu'on commence le parcours d'un docuemnt
   virtual void on_start_document();
   /// Ce qui se passe lorsque l'on termine le parcours du document
   virtual void on_end_document();
   
   /** 
    * Ce qui se passe lorsque l'on commence le parcours d'un élément
    * @parameter name nom de l'élément
    * @parameter properties Liste des attributs de l'élément
    */
   virtual void on_start_element(const Glib::ustring& name, const AttributeList& properties);

   /**
    * Ce qui se passe lorsque l'on sort d'un élément
    * @parameter name nom de l'élément
    *
    */
   virtual void on_end_element(const Glib::ustring& name);
   
   /** 
    * Ce qui se passe lorsque l'on rencontre une chaîne de caractères
    * @parameter characters la liste de caractères en question
    */
   virtual void on_characters(const Glib::ustring& characters);

        
    /// Attributs
    
   /// Etat courant
   int state;
   
   std::string adresseCourante;
   /// Vecteur des adresses à remplir
   std::vector<std::string> * adresses;
   
    /// Matrice des distances à remplir
   int numeroLigne;
   std::vector<int> * ligne;
    std::vector< std::vector<int> > * distanceMatrix;
    
};