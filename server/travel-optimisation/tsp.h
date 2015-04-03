#include <vector>

// G: graphe pondere non oriente complet verifiant l'inegalite triangulaire
// Retourne les sommets dans l'ordre de visite d'un tour de longueur au plus 2 fois l'optimal
std::vector<int> travelingSalesmanPerson(std::vector<std::vector<double> > &G, int depart);
