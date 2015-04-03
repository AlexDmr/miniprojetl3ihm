#include <stack>
#include "kruskal.h"
#include "tsp.h"

using namespace std;

// G: graphe pondere non oriente complet verifiant l'inegalite triangulaire
// Retourne les sommets dans l'ordre de visite d'un tour de longueur au plus 2 fois l'optimal
vector<int> travelingSalesmanPerson(vector<vector<double> > &G, int depart)
{
	int n = G.size();
	vector<vector<double> > T(n);
	for (int i = 0; i < n; i++)
		T[i].resize(n, -1);
	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			T[i][j] = G[i][j];
	kruskal(G, T);
	std::vector<bool> marque(G.size(), false);
	std::stack<int> a_visiter;
	a_visiter.push(depart); // on commence a un sommet arbitraire
	marque[depart] = true;
	vector<int> L;

	while (a_visiter.empty() == false) // j'ai choisi de faire mon parcours en profondeur en iteratif, mais on peut le faire en recursif aussi facilement
	{
		int s = a_visiter.top();
		a_visiter.pop();
		L.push_back(s);
		for (int i = 0; i < G.size(); i++)
			if (T[s][i] != -1. && !marque[i])
			{
				a_visiter.push(i);
				marque[i] = true;
			}
	}
	return L;
}