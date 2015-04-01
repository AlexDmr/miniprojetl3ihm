#include <iostream>
#include "FromXMLToGoogleMapHTTPRequest.h"
#include "FromGoogleMapXMLToDistanceTable.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Veuillez entrer le nom du fichier a analyser ainsi que l'identifiant de l'infirmiare en paramètre de la commande : testParsers filename.xml id" << std::endl;
        std::cout << "Exemple: " << std::endl;
        std::cout << "> ./testParsers ../data/cabinetInfirmier.xml 1" << std::endl << std::endl;
        return 1;
    }
    char * filename = argv[1];
    char * id = argv[2];
    int idInt = atoi(id);

    LwSaxParser simpleParser;
    FromXMLToGoogleMapHTTPRequest dataBaseParser;
    FromGoogleMapXMLToDistanceTable googleMapParser;
//    SortVisits sorter;

    std::vector<std::string> * adresses;
    std::vector< std::vector<int> > * distances;
    std::string inputStd(filename);
    std::string tmpFileName = inputStd.substr(0, inputStd.find_last_of("."));
    tmpFileName += "-sorted.xml";


    int option = -1;
    while (option != 0) {
        std::cout << "-------------------------------------------" << std::endl;
        std::cout << "Que voulez-vous faire avec ce fichier ?" << std::endl;
        std::cout << "0 quitter l'application" << std::endl;
        std::cout << "1 tester le parseur LwSaxParser" << std::endl;
        std::cout << "2 obtenir la requête HTTP à envoyer à GoogleMap pour récupérer les matrices de distances entre les adresses " << std::endl;
        std::cout << "3 créer un tableau c++ à partir du fichier XML renvoyé par GoogleMap " << std::endl;
        std::cout << "4 Modifier la base de données XML donnée avec une liste d'adresses ordonnées pour une infirmière" << std::endl;
        std::cout << "5 Créer un document HTML à partir du doc XML de base de données et du résultat de l'ordonnancement des adresses " << std::endl;
        std::cout << "Veuillez taper 0, 1, 2, 3, 4 ou 5" << std::endl;
        std::cout << "-------------------------------------------" << std::endl;

        std::cin >> option;

        if (std::cin.fail()) {
            std::cin.clear();
            //std::cin.ignore(std::numeric_limits<std::streamsize>::max(), std::cin.widen ( '\n' ));
            option = -1;
        }

        switch (option) {
            case 0: // On ne fait rien et l'on s'en va...
                break;
                
            case 1:
                std::cout << std::endl;
                simpleParser.parseDocument(filename);
                std::cout << std::endl;            
                break;

            case 2: // obtenir la requête HTTP à envoyer à GoogleMap pour récupérer les matrices de distances entre les adresses
                std::cout << std::endl;
                std::cout << std::endl;
                std::cout << "La requête HTTP est: " << dataBaseParser.getGoogleHttpRequest(filename, idInt);
                break;

            case 3: // créer un tableau c++ à partir du fichier XML renvoyé par GoogleMap
                googleMapParser.parseDocument(filename);
                std::cout << std::endl;
                std::cout << std::endl;
                adresses = googleMapParser.getAdresses();
                distances = googleMapParser.getDistances();

                // écrire la liste des adresses:
                for (unsigned int i = 0; i < adresses->size(); i++)
                {
                    std::cout << "| " << i << " | " << adresses->at(i) << " | " << std::endl;
                }
                std::cout << std::endl;
                std::cout << std::endl;

                // écrire le tableau des distances avec les index
                std::cout << "     | ";
                for(unsigned int c = 0; c < adresses->size(); c++) {
                    std::string spacesBefore = "";
                    if (c < 10)
                        spacesBefore = "   ";
                    else if (c < 100)
                        spacesBefore = "  ";
                    else if (c < 1000)
                        spacesBefore = " ";
                    else
                        spacesBefore = "";

                    std::cout << spacesBefore << c <<  " | ";
                }
                std::cout << std::endl << "  --------------------------------" << std::endl;

                for (unsigned int ligne = 0; ligne < adresses->size(); ligne++)
                {
                    std::cout << "   " << ligne << " | ";
                    for (unsigned int colonne = 0; colonne < adresses->size(); colonne++)
                    {
                        int value = (distances->at(ligne)).at(colonne);
                        std::string spacesBefore = "";
                        if (value < 10)
                            spacesBefore = "   ";
                        else if (value < 100)
                            spacesBefore = "  ";
                        else if (value < 1000)
                            spacesBefore = " ";
                        else
                            spacesBefore = "";

                        std::cout << spacesBefore << value << " | ";
                    }
                    std::cout << std::endl << "  --------------------------------" << std::endl;
                }

                break;

    //             case 4: // Modifier la base de données XML donnée avec une liste d'adresses ordonnées pour une infirmière
    //             googleMapParser.parseDocument("../data/reponseGoogle.xml");
    //             std::cout << std::endl;
    //             std::cout << std::endl;
    //             adresses = googleMapParser.getAdresses();
    //
    //             // On échange 2 adresses pour le test...
    //             if (adresses->size() > 2) {
    //                 std::string tmp = adresses->at(2);
    //                 adresses->at(2) = adresses->at(1);
    //                 adresses->at(1) = tmp;
    //             }
    //             // écrire la liste des adresses:
    //             for (unsigned int i = 0; i < adresses->size(); i++)
    //             {
    //                 std::cout << "| " << i << " | " << adresses->at(i) << " | " << std::endl;
    //             }
    //             std::cout << std::endl;
    //
    //                 sorter.modifyFile(filename, adresses, tmpFileName.c_str());
    //                 break;
    //
    //             case 5: // Display the HTML file
    //                 sorter.saveXHTMLFile(filename, "../data/test.html", idInt);
    //                 break;
    //
            default:
                option = -1;
                break;
        }
        std::cout << std::endl << "-------------------------------------------" << std::endl << std::endl;
    }

	return 0;
}
