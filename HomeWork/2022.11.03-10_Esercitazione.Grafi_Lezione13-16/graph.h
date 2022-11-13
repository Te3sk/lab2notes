typedef struct e{
	int dest;
	float w;
	struct e * next;
} Edge;

typedef struct {
	int val;
	Edge* adjList;
} Node;

typedef struct {
	int N;
	int E;
	Node* nodes;
} Graph;

typedef struct {
	int n1, n2;
	float w;
} CompleteEdge;

Graph load(char* );
void save(Graph, char*);

void insert(Edge** adjList,int dest,float weight);

int* dfs(Graph, int,int*,int*);

int hasCycles(Graph);

int countConnectedComponents(Graph);

int isTree(Graph);

Graph Kruskal(Graph);


