#include "kruskal.h"

using namespace std;

void tri(vector<Arete*> &tab)
{
	for(int i = 0; i < tab.size(); i++) // chercher l'element en position i
	{
		int max = i;
		for(int j = i+1; j < tab.size(); j++)
			if(tab[j]->poids > tab[max]->poids)
				max = j;
		Arete *tmp = tab[max];
		tab[max] = tab[i];
		tab[i] = tmp;
	}
}

void parcours(vector<vector<double> > &G, vector<bool> &marque, int source)
{
	marque[source] = 1;
	for(int i = 0; i < G.size(); i++)
		if(G[source][i] != -1 && marque[i] == 0)
			parcours(G, marque, i);
}

bool kruskal(vector<vector<double> > &G, vector<vector<double> > &T)
{
	vector<Arete*> aretes;
	for(int i = 0; i < G.size(); i++)
		for(int j = 0; j < G.size(); j++)
		{
			T[i][j] = G[i][j];
			if(j > i && G[i][j] != -1)
				aretes.push_back(new Arete(i, j, G[i][j]));
		}
	tri(aretes);
	int nAretesT = aretes.size();
	int nextArete = 0;
	while(nAretesT > G.size()-1)
	{
		if(nextArete >= aretes.size())
			return false;
		Arete *a = aretes[nextArete];
		T[a->u][a->v] = T[a->v][a->u] = -1;
		nAretesT--;
		vector<bool> marque(G.size(), 0);
		parcours(T, marque, 0);
		bool connected = true;
		for(int i = 0; i < G.size(); i++)
			if(marque[i] == 0)
				connected = false;
		if(connected == false)
		{
			nAretesT++;
			T[a->u][a->v] = T[a->v][a->u] = a->poids;
		}
		nextArete++;
	}
	return true;
}