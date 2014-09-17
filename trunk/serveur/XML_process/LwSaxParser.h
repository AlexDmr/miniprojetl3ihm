#ifndef LWSAXPARSER_H
#define LWSAXPARSER_H

#include <libxml++/libxml++.h>

class LwSaxParser : public xmlpp::SaxParser {

public:	
    /// Constructeur
    LwSaxParser();
    
    /// Destructeur
    virtual ~LwSaxParser();
	
    void parseDocument(char * filename);
    
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

    virtual void on_warning(const Glib::ustring& text);
    virtual void on_error(const Glib::ustring& text);
    virtual void on_fatal_error(const Glib::ustring& text);

    /*
     * @}
     */

};
#endif