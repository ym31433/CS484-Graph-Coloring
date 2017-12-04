#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void read_graph(char* filename) {
    //debug
    //printf("filename = %s\n", filename);
    printf("filename: %s\n", filename);
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

//write the colors to the file and print the number of color used
void write_graph(char* filename) {
    FILE* file = fopen(filename, "w");
    if(!file) {
        printf("Unable to open file %s\n", filename);
        return;
    }
    int num_colors = 0;
    char color_str[10];
    for(int i = 0; i != V; ++i) {
        //update number of colors
        if(colors[i] > num_colors) num_colors = colors[i];
        //write to file
        sprintf(color_str, "%d\n", colors[i]);
        fputs(color_str, file);
    }
    printf("Number of colors used: %d\n", num_colors+1);

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

