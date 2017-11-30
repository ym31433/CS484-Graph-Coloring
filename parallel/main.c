#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define IND(i, j, size) ((i)*(size)+(j))

int *graph; //only allocated and initialized in rank0
            //adjacency matrix, size:V^2
int *weights, *degrees; //weights and degrees of vertices,
                     //initialized in rank0 and broadcasted
                     //size: P
int *colors; //color list, size: V
int *subgraph; //subgraph matrix, initialized from scatter
               //size: subgraph_ranges[rank]
int *subgraph_offsets, *subgraph_ranges; //subgraph offset and range list
                    //initialized in all processes
                    //used for scattering the graph
                    //size: P
int *subcolors; //color list per processor, size: ranges[rank]
int *offsets, *ranges; //vetex ID offset and range list
                    //initialized in all processes
                    //used for gathering color list
                    //size: P
int V, E; //number of vertices and edges
int max_degree;
int rank; //ID in MPI
int P; //number of prcesses in MPI

void checker() {
    int start_id = offsets[rank];
    for(int i = 0; i != ranges[rank]; ++i) {
        if(colors[i+start_id] == -1) {
            printf("Error: %d is not colored!\n", i+start_id);
        }
        else {
            for(int j = 0; j != V; ++j) {
                if(subgraph[IND(i, j, V)] == 1 && colors[i+start_id] == colors[j]) {
                    printf("Error: %d and %d have the same color!\n", i+start_id, j);
                }
            }
        }
    }
    printf("rank%d finished checking.\n", rank);
}

void read_graph(char* filename) {
    //debug
    //printf("filename = %s\n", filename);
    FILE* file = fopen(filename, "r");
    if(!file) {
        printf("Unable to open file %s\n", filename);
        return;
    }

    char buf[100]; //buffer for the line
    E = 0; //counts the number of edges
    char* tmp; //temporary holding the desired string
    int v0, v1;
    
    //start parsing
    //first get the number of vertices and allocate graph
    //fscanf(file, "%s", buf);
    fgets(buf, 100, file);
    tmp = strtok(buf, "\n");
    V = atoi(tmp);
    //debug
    //printf("V=%d\n", V);
    graph = (int *)malloc(V*V*sizeof(int));
    //then parse the whole graph and get the number of vertices
    while(fgets(buf, 100, file) != NULL) {
        E++; //increments the number of edges
        //parse line to graph
        tmp = strtok(buf, " ");
        v0 = atoi(tmp);
        //debug
        //printf("v0=%d\n", v0);
        tmp = strtok(NULL, " ");
        //debug
        //printf("tmp=%s\n", tmp);
        v1 = atoi(tmp);
        //debug
        //printf("v1=%d\n", v1);
        graph[IND(v0, v1, V)] = 1;
        graph[IND(v1, v0, V)] = 1;
    }
    fclose(file);
}

void write_graph(char* filename) {
    FILE* file = fopen(filename, "w");
    if(!file) {
        printf("Unable to open file %s\n", filename);
        return;
    }

    char color_str[10];
    for(int i = 0; i != V; ++i) {
        sprintf(color_str, "%d\n", colors[i]);
        fputs(color_str, file);
    }

    fclose(file);
}

//used to debug
void printGraph() {
    //graph
    printf("graph = \n");
    for(int i = 0; i != V; ++i) {
        for(int j = 0; j != V; ++j) {
            printf("%d, ", graph[IND(i, j, V)]);
        }
        printf("\n");
    }
    //degree
    printf("degree = \n");
    for(int i = 0; i != V; ++i) {
        printf("%d, ", degrees[i]);
    }
    printf("\n");
    //weight
    printf("weight = \n");
    for(int i = 0; i != V; ++i) {
        printf("%d, ", weights[i]);
    }
    printf("\n");
    //color
    printf("color = \n");
    for(int i = 0; i != V; ++i) {
        printf("%d, ", colors[i]);
    }
    printf("\n");
}

void printLists() {
    printf("offsets = \n");
    for(int i = 0; i != P; ++i) {
        printf("%d, ", offsets[i]);
    }
    printf("\n");
    printf("ranges = \n");
    for(int i = 0; i != P; ++i) {
        printf("%d, ", ranges[i]);
    }
    printf("\n");
    printf("subgraph_offsets = \n");
    for(int i = 0; i != P; ++i) {
        printf("%d, ", subgraph_offsets[i]);
    }
    printf("\n");
    printf("subgraph_ranges = \n");
    for(int i = 0; i != P; ++i) {
        printf("%d, ", subgraph_ranges[i]);
    }
    printf("\n");
}

void init_lists(){
    offsets = (int *)malloc(P*sizeof(int));
    ranges = (int *)malloc(P*sizeof(int));
    subgraph_offsets = (int *)malloc(P*sizeof(int));
    subgraph_ranges = (int *)malloc(P*sizeof(int));

    int quotient = V/P;
    int remainder, start_id;
    offsets[0] = 0;
    subgraph_offsets[0] = 0;
    for(int i = 1; i != P; ++i) {
        //calculate start_id
        remainder = (V+i) % P;
        start_id = quotient*i + remainder*(remainder<i);
        //initialize lists
        offsets[i] = start_id;
        subgraph_offsets[i] = start_id*V;
        ranges[i-1] = offsets[i] - offsets[i-1];
        subgraph_ranges[i-1] = subgraph_offsets[i] - subgraph_offsets[i-1];
    }
    ranges[P-1] = V - offsets[P-1];
    subgraph_ranges[P-1] = V*V - subgraph_offsets[P-1];
#ifdef DEBUG
    if(rank == 0)
        printLists();
#endif
    //allocate subcolors & subgraph
    subcolors = (int *)malloc(ranges[rank]*sizeof(int));
    memset(subcolors, -1, ranges[rank]*sizeof(int));
    subgraph = (int *)malloc(subgraph_ranges[rank]*sizeof(int));
}

void ldf() {
    int start_id = offsets[rank];
    int select;
    int num_uncolored = ranges[rank]; //number of uncolored vertices
    int max_num_uncolored = ranges[rank]; //max number of uncolored vertices across all processes

    //for(int iteration = 0; iteration != max_degree; ++iteration) {
    while(max_num_uncolored != 0) {
        //go through all the vertices then color
#pragma omp parallel for private(select)
        for(int i = 0; i < ranges[rank]; ++i) {
            if(subcolors[i] != -1) continue; //this vertex is already colored
            //neighbor_color is used to keep track of which colors are used,
            //at most V different colors will be used
            int *neighbor_color = (int *)malloc(max_degree*sizeof(int));
            memset(neighbor_color, 0, max_degree*sizeof(int));
            select = 1;
            for(int j = 0; j != V; ++j) {
                if(subgraph[IND(i, j, V)] == 1) { //neighbor found
                    if(colors[j] != -1) { //neighbor is already colored
                        //update neighbor_color
                        neighbor_color[colors[j]] = 1;
                    }
                    else if(degrees[j] > degrees[i+start_id] ||
                      (degrees[j] == degrees[i+start_id] && weights[j] > weights[i+start_id])) {
                        select = 0;
                        break;
                    }
                }
            }
            if(select == 1) {
#pragma omp atomic
                num_uncolored--;
                for(int j = 0; j != max_degree; ++j) {
                    if(neighbor_color[j] == 0) {
                        subcolors[i] = j;
                        break;
                    }
                }
                //sanity check
                if(subcolors[i] == -1) {
                    printf("Error: failed to color vertex %d!\n", i);
#ifdef DEBUG_LDF
                    printf("neighbor_color = \n");
                    for(int color_id = 0; color_id != max_degree; color_id++) {
                        printf("%d, ", neighbor_color[color_id]);
                    }
                    printf("\n");
#endif
                }
            } //end of if(select == 1)
            free(neighbor_color);
        } //end of loop that goes through each vertex in subgraph

        //gather color
        MPI_Gatherv(subcolors, ranges[rank], MPI_INT,
        colors, ranges, offsets, MPI_INT, 0, MPI_COMM_WORLD);

        //check whether all the vertices are colored
        MPI_Allreduce(&num_uncolored, &max_num_uncolored, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        //if(max_num_uncolored == 0) break;

        //broadcast color
        MPI_Bcast(colors, V, MPI_INT, 0, MPI_COMM_WORLD);
    } //end of while(max_num_uncolored != 0)
}

int main(int argc, char** argv) {

    char* input_filename = malloc(1000);
    char* output_filename = malloc(1000);
    
    int provided; // check the supported MPI_THREAD function
    //initialize MPI
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if(provided != MPI_THREAD_FUNNELED) {
        printf("Error: cannot support MPI_THREAD_FUNNELED!\n");
        return -1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);
#ifdef DEBUG
    if(rank == 0)
        printf("P = %d\n", P);
#endif

    if(argc != 3) {
        if(rank == 0) printf("Usage: mpirun -np <#processes> ./graphColoring <input_file> <output_file>\n");
        MPI_Finalize();
        return -1;
    }
    
    //read graph
    if(rank == 0) {
        strcpy(input_filename, argv[1]);
        read_graph(input_filename);
    } 
    MPI_Bcast(&V, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Bcast(&E, 1, MPI_INT, 0, MPI_COMM_WORLD); //do we need this?

    //initialize weights, degrees and colors
    degrees = (int *)malloc(V*sizeof(int));
    weights = (int *)malloc(V*sizeof(int));
    colors = (int *)malloc(V*sizeof(int));
    memset(colors, -1, V*sizeof(int));
    if(rank == 0) {
        int count; //temporal count for degree
        max_degree = 0;
        //TODO: omp parallel, reduction for max_degree
        for(int i = 0; i != V; ++i) {
            //degree
            count = 0;
            for(int j = 0; j != V; ++j) {
                count += graph[IND(i, j, V)];
            }
            degrees[i] = count;
            if(count > max_degree) max_degree = count;
            //weight the weight is vertex ID for now
            weights[i] = i;
        }
#ifdef DEBUG
        printGraph();
#endif
    }

    //broadcast weights and degrees
    MPI_Bcast(&max_degree, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(degrees, V, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(weights, V, MPI_INT, 0, MPI_COMM_WORLD);

    //initialize lists for scatter&gather
    init_lists();

    //scatter graph into subgraphs in processers
    MPI_Scatterv(graph, subgraph_ranges, subgraph_offsets, MPI_INT,
    subgraph, subgraph_ranges[rank], MPI_INT, 0, MPI_COMM_WORLD);

    //Largest Degree First algorithm
    ldf();

#ifdef DEBUG
    if(rank == 0)
        printGraph();
#endif
#ifdef CHECKER
    checker();
#endif
    //TODO: write graph
    if(rank == 0) {
        strcpy(output_filename, argv[2]);
        write_graph(output_filename);
    }

    free(graph);
    free(colors);
    free(weights);
    free(degrees);
    free(subgraph);
    free(subgraph_offsets);
    free(subgraph_ranges);
    free(subcolors);
    free(offsets);
    free(ranges);
    free(input_filename);
    free(output_filename);
    MPI_Finalize();
    return 0;
}
