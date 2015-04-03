#include "tsp.h"
#include <iostream>
#include <vector>

using namespace std;

int main()
{
	int n = 5; // taille du graphe
	vector<vector<double> > G(n);
	for (int i = 0; i < n; i++)
		G[i].resize(n);

	vector<int> L = travelingSalesmanPerson(G, 0);

	for(int i = 0; i < L.size(); i++)
		cout<<L[i]<<endl;
}