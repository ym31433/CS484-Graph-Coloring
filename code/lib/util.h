
#define IND(i, j, size) ((i)*(size)+(j))
int *graph; //only allocated and initialized in rank0
            //adjacency matrix, size:V^2
int *weights, *degrees; //weights and degrees of vertices,
                     //initialized in rank0 and broadcasted
                     //size: P
int *colors; //color list, size: V
int V, E; //number of vertices and edges
int max_degree;

void read_graph(char *filename);
void write_graph(char *filename);
void printGraph();
