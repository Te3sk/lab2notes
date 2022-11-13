#include "graph.h"
#include <stdio.h>

int main(){

		Graph g = load("g1.txt");
		
		//printf("load completo\n");
		//processing

		//save(g,"outputG1.txt");

		//printf("save completo\n");

		/*int* dfsRes= dfs(g,5,NULL,NULL);
		
		printf("Visita: ");
		for(int i=0;i<g.N;i++)
			printf("%d ",g.nodes[dfsRes[i]].val);
		printf("\n");

		printf("Grafo con %d componenti connesse.\n", countConnectedComponents(g));
		
		if(hasCycles(g))
			printf("Grafo con cicli.\n");
		else
			printf("Grafo senza cicli.\n");

		if(isTree(g))
			printf("Grafo e' un albero.\n");
		else
			printf("Grafo non e' un albero.\n");
		*/
		Graph tree=Kruskal(g);
	
		save(tree, "alberoG1.txt");

		return 0;

}