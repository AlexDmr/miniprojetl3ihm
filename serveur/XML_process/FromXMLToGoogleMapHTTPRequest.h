#include "LwSaxParser.h"


#include <string>

class Adresse;
class FromXMLToGoogleMapHTTPRequest : public LwSaxParser {

public:
    enum PossibleStates {START, PATIENT, ADRESSE, ETAGE, NUMERO, RUE, VILLE, CODEPOSTAL, VISITE, OTHER};
    
    /// Constructeur
    FromXMLToGoogleMapHTTPRequest(std::string id);
    
    /// Desctructeur
    ~FromXMLToGoogleMapHTTPRequest();
    
    std::string getGoogleMapHttpRequest();

    std::string getGoogleMapHttpRequest_V2();
    
protected:
    bool adresseCabinet;
    
    /// Attributs
    std::string request;
    
    /// id de l'infirmière
    std::string id;
    
    /// Etat courant
    PossibleStates state;
    
    /// Stockage des listes d'adresse
    std::string listeAdresses;
    
    Adresse * adresseCourante;
    
    /// Stockage temporaire de l'adresse courante
    ///  Elle sera ajoutée à listeAdresse après vérification qu'un élément visite contient bien l'id de l'infirmière
//    std::string adresseCourante;
    
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
   
   
protected:
    // Cherche attributeName dans la liste des attributs et renvoie sa valeur
    // renvoie la chaîne de caractères vide si l'attribut n'est pas trouvé.
    std::string findAttribute(const AttributeList& attributeList, std::string attributeName);


};

