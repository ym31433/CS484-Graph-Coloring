//
//  main.c
//  cs484project
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define IND(i, j, size) ((i)*(size)+(j))


int V;//number of vertices
int E;//number of edges
int *degrees;//the degrees of vertices
int *degrees_sorted;//sorted degrees
int *orders;//orders of the degrees

int *graph;//the original adjacency graph
int *new_graph;//the adjacency graph for sorted vertices by degree numbers
int *colors;//colors of each vertices
int *new_colors;//colors of each vertices

int color_count;//the colors used,start from 0

int coloring_done;//finishing flag
int *safe;//

void read_graph(char* filename) {
    //open the filr
    //printf("THIIIIIIIIIS STEP");
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
    fgets(buf, 100, file);
    tmp = strtok(buf, "\n");
    V = atoi(tmp);
    
    graph = (int *)malloc(V*V*sizeof(int));
    new_graph=(int *)malloc(V*V*sizeof(int));
    for(int i = 0; i != V; ++i){
        for(int j = 0; j!= V; ++j){
            graph[IND(i,j, V)]=0;
        }
    }
    //then parse the whole graph and get the number of vertices
    while(fgets(buf, 100, file) != NULL) {
        E++; //increments the number of edges
        //parse line to graph
        tmp = strtok(buf, " ");
        v0 = atoi(tmp);
        tmp = strtok(NULL, " ");
        v1 = atoi(tmp);
        graph[IND(v0, v1, V)] = 1;
        graph[IND(v1, v0, V)] = 1;
    }
    fclose(file);
}

void printGraph() {
    //graph
    printf("graph = \n");
    for(int i = 0; i != V; ++i) {
        for(int j = 0; j != V; ++j) {
            printf("%d, ", graph[IND(i, j, V)]);
        }
        printf("\n");
    }
    
    //new graph
    printf("new graph = \n");
    for(int i = 0; i != V; ++i) {
        for(int j = 0; j != V; ++j) {
            printf("%d, ", new_graph[IND(i, j, V)]);
        }
        printf("\n");
    }

    //degrees
    printf("degree = \n");
    for(int i = 0; i != V; ++i) {
        printf("%d, ", degrees[i]);
    }
    printf("\n");
    
    printf("sorted_degree = \n");
    for(int i = 0; i != V; ++i) {
        printf("%d, ", degrees_sorted[i]);
    }
    printf("\n");

    //colors
    printf("color = \n");
    for(int i = 0; i != V; ++i) {
        printf("%d, ", colors[i]);
    }
    printf("\n");
    
    //colors
    printf("new color = \n");
    for(int i = 0; i != V; ++i) {
        printf("%d, ", new_colors[i]);
    }
    printf("\n");
    
    //orders
    printf("order = \n");
    for(int i = 0; i != V; ++i) {
        printf("%d, ", orders[i]);
    }
    printf("\n");

}

void initialize(){
    //initialize colors vector
    colors = (int *)malloc(V*sizeof(int));
    memset(colors, -1, V*sizeof(int));
    
    new_colors = (int *)malloc(V*sizeof(int));
    memset(new_colors, -1, V*sizeof(int));
    
    //initialize degrees vector
    degrees = (int *)malloc(V*sizeof(int));
    degrees_sorted=(int *)malloc(V*sizeof(int));
    for(int i=0;i!=V;++i){
        int count = 0;
        for(int j = 0; j != V; ++j) {
            if(graph[IND(i, j, V)]==1)
                count++;
            //count += graph[IND(i, j, V)];
        }
            degrees[i] = count;
            degrees_sorted[i]=count;
    }
    color_count=0;
    
    orders = (int *)malloc(V*sizeof(int));
    
    for(int i = 0; i != V; ++i){
        orders[i]=-1;
       
    }
    

}


int cmpfunc (const void * a, const void * b) {
    return ( *(int*)b- *(int*)a );
}

void sort_degrees(){
    //sort the degrees from largest to smallest
    qsort(degrees_sorted,V,sizeof(int),cmpfunc);
    int *labeled;
    labeled = (int *)malloc(V*sizeof(int));
    for(int i = 0; i != V; ++i) {
        labeled[i]=-2;
    }
    
    //label the order of degrees
    
       for(int i = 0; i != V; ++i){
        int j=0;
        while(j<V){
            if(labeled[j]==-2){
               if(degrees[j]==degrees_sorted[i]){
                    orders[i]=j;
                    labeled[j]=1;
                    break;
               }
            }
            //}
        j++;
        }
    }
    
    for(int i = 0; i != V; ++i){
        for(int j = 0; j!= V; ++j){
            new_graph[IND(i,j, V)]=graph[IND(orders[i],orders[j], V)];
        }
    }

    
    //labels
    /*printf("labeled= \n");
    for(int i = 0; i != V; ++i) {
        printf("%d, ", labeled[i]);
    }
    printf("\n");*/
}

void adjacency_check(int initial)
{
    
    for(int i = initial+1; i < V; ++i){
        if(new_colors[i]==-1){
        //printf("LOOP NUMVER %d \n",i);
            if(new_graph[IND(initial,i,V)]==1){
                //printf("gonna change %d safe %d \n",initial,i);
                safe[i]++;
            }
            if(safe[i]==0){
                //printf("find next %d \n",i);
                adjacency_check(i);
            }

        }
    }
    
    
}


void Welsh_Powell(){
    
    
    safe=(int *)malloc(V*sizeof(int));
    for(int i = 0; i != V; ++i){
        safe[i]=0;
    }
    
    

    coloring_done=1;//assume coloring done initially
    int first;//the vertex we are gonna start this round
    
    //do coloring following the order
    for(int i = 0; i != V; ++i){
        if(new_colors[i]==-1){
            new_colors[i]=color_count;
           
            first=i;
            break;
        }
    }
     //printf("this round starts from %d\n",first);
    
    adjacency_check(first);
    
    //printf("this round color is %d\n",color_count);
    
    /*
    printf("safe= \n");
    for(int i = 0; i != V; ++i) {
        printf("%d, ", safe[i]);
    }
    printf("\n");
    */
    
    for(int j=first+1;j<V;j++){
        if(new_colors[j]==-1 && safe[j]==0){
            new_colors[j]=color_count;
        }
    }
    
    //check whether the coloring proceess
    for(int j=0;j<V;j++){
        if(new_colors[j]==-1){
        coloring_done=0;
        break;
        }
    }
    
    if(coloring_done)
        return;
    else{
        color_count++;
        Welsh_Powell();
    }
   
}

void revert_color(){
    for(int j=0;j<V;j++)
        colors[orders[j]]=new_colors[j];
}

void checker(){
    for(int i = 0; i != V; ++i){
        for(int j = 0; j!= V; ++j){
            if(graph[IND(i,j,V)]==1 && colors[i]==colors[j])
                printf("Duplicate coloring ");
        }
     }
}

int main(int argc, const char * argv[]) {
    
    char *input_file="5_10.txt";
    read_graph(input_file);
    
    
    initialize();
    //printGraph();
    sort_degrees();
    
    Welsh_Powell();
    revert_color();
    printGraph();
    checker();
    
    printf("The maximum color we are using is %d \n",color_count+1);
    
    free(graph);
    free(colors);
    free(degrees);
    free(degrees_sorted);
    //free(input_file);
    
    return 0;
}
