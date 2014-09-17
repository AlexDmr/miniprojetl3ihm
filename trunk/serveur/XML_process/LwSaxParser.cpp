#include <iostream>
#include "LwSaxParser.h"

#include <iostream>
#include <fstream>

/*
 * L'implémentation des méthodes suivantes est formtement inspirée du site 
 * http://libxmlplusplus.sourceforge.net/docs/manual/html/ar01s02.html
 *
 * Ne pas hésiter à se référer à l'API documentation de libxml++:
 * https://developer.gnome.org/libxml++/stable/annotated.html
 */


LwSaxParser::LwSaxParser() : xmlpp::SaxParser() {
	std::cout << "Building my own sax parser..." << std::endl;
}

LwSaxParser::~LwSaxParser() {
	
}

void LwSaxParser::parseDocument(char * filename) {
 try 
    {
        std::cout << "Lecture du fichier " << filename << " ligne par ligne..." << std::endl;
        std::string line;
        std::ifstream inputStream(filename);

        while (std::getline(inputStream, line)) {
            parse_chunk(line);
        }
        finish_chunk_parsing();
        std::cout << "Fin de la lecture du fichier " << filename << "." << std::endl;
      }
      catch(const xmlpp::exception& ex)
       {
           std::cout << "libxml++ exception: " << ex.what() << std::endl;
       }
}

void LwSaxParser::on_start_document() {
	std::cout << "Appel de la méthode: on_start_document()" << std::endl;
}

void LwSaxParser::on_end_document() {
	std::cout << "Appel de la méthode: on_end_document()" << std::endl;
}


void LwSaxParser::on_start_element(const Glib::ustring& name, const AttributeList& attributes)
{
  std::cout << "Appel de la méthode: on_start_element(const Glib::ustring& name, const AttributeList& attributes)" << std::endl;

  std::cout << "    avec les arguments suivants: " << std::endl;
  std::cout << "      - name: " << name << std::endl;
  for(xmlpp::SaxParser::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter)
  {
    std::cout << "      - attribute: (" << iter->name << " = " << iter->value << ")" << std::endl;
  }
}

void LwSaxParser::on_end_element(const Glib::ustring& name)
{
  std::cout << "Appel de la méthode: on_end_element(const Glib::ustring& name) " << std::endl;
  std::cout << "    avec les arguments suivants: " << std::endl;
  std::cout << "      - name: " << name << std::endl;
}

void LwSaxParser::on_characters(const Glib::ustring& text)
{
    std::cout << "Appel de la méthode: on_characters(const Glib::ustring& text) " << std::endl;
    std::cout << "    avec les arguments suivants: " << std::endl;
    std::cout << "      - text: " << text << std::endl;
}


void LwSaxParser::on_warning(const Glib::ustring& text)
{
  std::cout << "on_warning(): " << text << std::endl;
}

void LwSaxParser::on_error(const Glib::ustring& text)
{
  std::cout << "on_error(): " << text << std::endl;
}

void LwSaxParser::on_fatal_error(const Glib::ustring& text)
{
  std::cout << "on_fatal_error(): " << text << std::endl;
}
