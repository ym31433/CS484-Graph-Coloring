#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include <time.h>
#include <stdint.h>

void checker() {
    for(int i = 0; i != V; ++i) {
        if(colors[i] == -1) {
            printf("Error: %d is not colored!\n", i);
        }
        else {
            for(int j = 0; j != V; ++j) {
                if(graph[IND(i, j, V)] == 1 && colors[i] == colors[j]) {
                    printf("Error: %d and %d have the same color!\n", i, j);
                }
            }
        }
    }
    printf("Finished checking.\n");
}

void ldf() {
    int select;
    int num_uncolored = V; //number of uncolored vertices

    //for(int iteration = 0; iteration != max_degree; ++iteration) {
    while(num_uncolored != 0) {
        //go through all the vertices then color
        for(int i = 0; i < V; ++i) {
            if(colors[i] != -1) continue; //this vertex is already colored
            //neighbor_color is used to keep track of which colors are used,
            //at most V different colors will be used
            int *neighbor_color = (int *)malloc(max_degree*sizeof(int));
            memset(neighbor_color, 0, max_degree*sizeof(int));
            select = 1;
            for(int j = 0; j != V; ++j) {
                if(graph[IND(i, j, V)] == 1) { //neighbor found
                    if(colors[j] != -1) { //neighbor is already colored
                        //update neighbor_color
                        neighbor_color[colors[j]] = 1;
                    }
                    else if(degrees[j] > degrees[i] ||
                      (degrees[j] == degrees[i] && weights[j] > weights[i])) {
                        select = 0;
                        break;
                    }
                }
            }
            if(select == 1) {
                num_uncolored--;
                for(int j = 0; j != max_degree; ++j) {
                    if(neighbor_color[j] == 0) {
                        colors[i] = j;
                        break;
                    }
                }
                //sanity check
                if(colors[i] == -1) {
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
        } //end of loop that goes through each vertex 

    } //end of while(max_num_uncolored != 0)
}

int main(int argc, char** argv) {

    char* input_filename = malloc(1000);
    char* output_filename = malloc(1000);
    struct timespec start_time, end_time;
    uint64_t elapsed_time;
    
    if(argc != 3) {
        printf("Usage: mpirun -np <#processes> ./graphColoring <input_file> <output_file>\n");
        return -1;
    }
    
    //read graph
    strcpy(input_filename, argv[1]);
    read_graph(input_filename);

    //record time
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    //initialize weights, degrees and colors
    degrees = (int *)malloc(V*sizeof(int));
    weights = (int *)malloc(V*sizeof(int));
    colors = (int *)malloc(V*sizeof(int));
    memset(colors, -1, V*sizeof(int));
    int count; //temporal count for degree
    max_degree = 0;
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

    //Largest Degree First algorithm
    ldf();

    //record time
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec)*1000000 + (end_time.tv_nsec - start_time.tv_nsec)/1000;
    printf("Time taken: %llu microseconds.\n", (long long unsigned int)elapsed_time);

#ifdef DEBUG
    printGraph();
#endif
#ifdef CHECKER
    checker();
#endif
    strcpy(output_filename, argv[2]);
    write_graph(output_filename);

    free(graph);
    free(colors);
    free(weights);
    free(degrees);
    free(input_filename);
    free(output_filename);
    return 0;
}
