#include <stdio.h>
#include <stdlib.h>
#include "GRASP.h"
#include <igraph.h>
#define IT_GRASP 8
#define IT_LS 1
//gcc -fopenmp structures.h utilities.h searches.h GRASP.h main.c  -I/usr/include/igraph -L/usr/local/lib -ligraph -o TCC2


void uso_Grasp(char graph_Name[], int threads, float alpha)
{
    sol* solution;
    igraph_vector_t used_Nodes, multicast_Group;
    typeNode *graph;
    clock_t start, end_Work;

    float **info;
    int index, size_Graph = 0;
    printf("%s %d %f\n", graph_Name, threads, alpha);
    info = (float**)malloc(sizeof(float*)*IT_GRASP);

    for(index = 0; index < IT_GRASP; index++){info[index] = (float*)malloc(sizeof(float)*2);}
//    printf("yup");
    graph = function_to_read(&multicast_Group, graph_Name, &size_Graph);
//    printf("yup");
    start = clock();

    igraph_vector_init(&used_Nodes, 0);
    solution = GRASP_P(graph, &multicast_Group, IT_GRASP, IT_LS, size_Graph, threads, alpha, &used_Nodes, info);

    end_Work = (double)clock();
	
    function_to_save(graph_Name, start, end_Work, solution, &used_Nodes, alpha, threads, IT_GRASP, info);
	
    solution = dell_tree(solution, &used_Nodes);

    for(index = 0; index < IT_GRASP; index++){free(info[index]);}

    for(index = 0; index<size_Graph; index++)
    {
        igraph_vector_destroy(&graph[index].neighbors);
        igraph_vector_destroy(&graph[index].edge_Weight);
    }
    free(graph);

    free(info);
}

void main(int argc, char *argv[])
{
    int i;
    float j;
    char aux[50];
//    printf("%c", argv[1]);
    strcpy(aux, argv[1]);
    for(i=1; i<=2; i*=2)
        for(j = 0; j <= 1; j+=0.1)
            uso_Grasp(aux, i, j);

}