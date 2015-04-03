#include <vector>
#include "Arete.h"

void tri(std::vector<Arete*> &tab);
void parcours(std::vector<std::vector<double> > &G, std::vector<bool> &marque, int source);
bool kruskal(std::vector<std::vector<double> > &G, std::vector<std::vector<double> > &T);
