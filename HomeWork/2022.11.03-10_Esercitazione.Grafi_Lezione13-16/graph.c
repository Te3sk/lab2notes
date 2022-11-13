#include <stdlib.h>
#include <stdio.h>

#include "Stack.h"
#include "graph.h"

Graph load(char* file ){
		FILE* in=fopen(file,"r");

		if(in){
			Graph g;

			fscanf(in,"%d",&g.N);
			g.nodes=(Node*)malloc(g.N*sizeof(Node));


			for(int i=0;i<g.N;i++){
				fscanf(in,"%d",&g.nodes[i].val);
				g.nodes[i].adjList=NULL;
			}
			int n1,n2;
			float w;
			g.E=0;
			while(fscanf(in, "%d %d %f",&n1,&n2,&w)==3){
				insert(&g.nodes[n1].adjList,n2,w);
				insert(&g.nodes[n2].adjList,n1,w);
				g.E++;
			}
			return g;

		} else {
			perror("Open file: ");
			exit(1);
		}
		

}
void save(Graph g, char* file){
	FILE* out=fopen(file,"w");

	if(out){
		fprintf(out,"%d\n",g.N);
		for(int i=0;i<g.N;i++){
			fprintf(out,"%d ",g.nodes[i].val);
		}
		for(int i=0;i<g.N;i++){
			Edge* temp=g.nodes[i].adjList;
			while(temp){
				//stampo questo arco
				if(temp->dest>i)
					fprintf(out,"\n%d %d %f",i,temp->dest, temp->w);
				temp=temp->next;
			}
		}
		fclose(out);	

	} else {
			perror("Open file: ");
			exit(1);
		}
}

void insert(Edge** adjList,int dest,float weight){
	Edge* e=(Edge*)malloc(sizeof(Edge));
	e->dest=dest;
	e->w=weight;
	e->next=*adjList;
	*adjList=e;
}

void visit(int n,int p,int* visited, int* result, int* visitedN){
	visited[n]=p;
	result[*visitedN]=n;
	*visitedN=*visitedN+1;
}

int* dfs(Graph g, int start, int* hasCycles, int* nConComp){
	int prec[g.N];
	
	int* result=(int*)malloc(g.N*sizeof(int));
	int visitedN=0;

	for(int i=0;i<g.N;i++){
		prec[i]=-1;
	}
	
	Stack s=NULL;

	push(&s,start);
	visit(start, -2,prec,result,&visitedN);
	if(nConComp) *nConComp=1;
	
	if(hasCycles)
			*hasCycles=0;//questo if era erroneamente incluso dentro la while che segue

	//visita
	while(visitedN<g.N){
		int next=pop(&s);
		
		Edge* e=g.nodes[next].adjList;
		while(e!=NULL){
			if(prec[e->dest]==-1){//il controllo era con 0 invece di -1
				visit(e->dest,next,prec,result,&visitedN);
				push(&s,e->dest);
			} else{
				if(prec[next]!=e->dest)
					if(hasCycles) *hasCycles=1;
			}
			e=e->next;
		}
		//printf("%p\n", s);
		if(s==NULL && visitedN<g.N){
			int i=0;
			while(prec[i]!=-1)
				i++;
			visit(i,-2,prec,result,&visitedN);
			push(&s,i);
			if(nConComp) (*nConComp)++;
		}
	}
	
	return result;
}

int hasCycles(Graph g){
	int cycles;
	int* temp=dfs(g,0,&cycles,NULL);
	free(temp);
	return cycles;
}

int countConnectedComponents(Graph g){
	int ncc;
	int* temp=dfs(g,0,NULL,&ncc);
	free(temp);
	return ncc;
}

int isTree(Graph g){
	int cycles,ncc;
	int* temp=dfs(g,0,&cycles,&ncc);
	free(temp);
	return (ncc==1 && !cycles);
}


int compareEdge(const void* e1,const void* e2){
	return (int)((CompleteEdge*)e1)->w-((CompleteEdge*)e2)->w;
}

void insertEdge(Graph g, CompleteEdge e){
	insert(&(g.nodes[e.n1].adjList), e.n2,e.w);
	insert(&(g.nodes[e.n2].adjList), e.n1,e.w);
}

void remove1 (Edge** adjList, int n){
	if((*adjList)!=NULL){
			if((*adjList)->dest==n){
				Edge* temp=(*adjList);
				(*adjList)=temp->next;
				free(temp);
			} else {
				remove1(&((*adjList)->next),n);
			}
	}
}

void removeEdge(Graph g, CompleteEdge e){
	remove1(&(g.nodes[e.n1].adjList),e.n2);
	remove1(&(g.nodes[e.n2].adjList),e.n1);	
}

Graph Kruskal(Graph g){

	Graph tree;
	tree.N=g.N;
	tree.E=0;

	//aggiungere nodi a tree
	tree.nodes=(Node*)malloc(g.N*sizeof(Node));
	//controllo puntatore null
	if(tree.nodes==NULL){
		printf("Memory error");
		exit(1);
	}

	for(int i=0;i<tree.N;i++){
		tree.nodes[i].val=g.nodes[i].val;
		tree.nodes[i].adjList=NULL;
	}
	save(tree,"temp.txt");

	//creare array di archi

	CompleteEdge edges[g.E];
	int dim=0;
	
	
	for(int i=0;i<g.N;i++){
		Edge* al=g.nodes[i].adjList;
		while(al!=NULL){
			if(i<al->dest){
				CompleteEdge e;
				e.n1=i;
				e.n2=al->dest;
				e.w=al->w;

				edges[dim]=e;
				dim++;
			}
			al=al->next;
		}
	}


	//ordinare array di archi

	qsort(edges,g.E,sizeof(CompleteEdge),compareEdge);

	//cominciare ad aggiungere archi all'albero

	int e=0;
	while(tree.E<tree.N-1){
		insertEdge(tree,edges[e]);

		if (hasCycles(tree)){

			removeEdge(tree,edges[e]);

		} else {

			tree.E++;

		}
		e++;

	}

	return tree;	

}
