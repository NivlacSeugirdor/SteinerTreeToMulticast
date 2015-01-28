/**======================================
	Author: Calvin Rodrigues da Costa
=======================================*/


#include <stdio.h>
#include <stdlib.h>
#include "grasp.h"
#include <igraph.h>

#define IT_GRASP 20
#define IT_LS 1

/**
    gcc -fopenmp structures.h utilities.h searches.h GRASP.h main.c  -I/usr/include/igraph -L/usr/local/lib -ligraph -o TCC2
*/


void uso_grasp(typeNode *graph, int size_Graph, igraph_vector_t multicast_Group, char graph_Name[], int threads, float alpha)
{
    sol* solution;
    igraph_vector_t used_Nodes;

    clock_t start_Work, end_Work;

    float **info;
    int index, index2, match=0;
    ///Performs the allocation of matrix "info" with size (IT_GRASP x 2 )

    info = (float**)malloc(sizeof(float*)*IT_GRASP);

    for(index = 0; index < IT_GRASP; index++){info[index] = (float*)malloc(sizeof(float)*2);}


    /**
    // If be necessary, to print the graph.

    printf("Graph of size %d :\n", size_Graph);
    for(index = 0; index<size_Graph; index++)
    {
        printf("Node %d\n", (index+1) );
        printf("Neighbors: ");
        for(index2=0; index2<igraph_vector_size(&graph[index].neighbors); index2++)
        {
            printf("%d ", (int)VECTOR(graph[index].neighbors)[index2]);
        }
        printf("\n");

        printf("Weights: ");
        for(index2=0; index2<igraph_vector_size(&graph[index].neighbors); index2++)
        {
            printf("%.0f ", (float)VECTOR(graph[index].edge_Weight)[index2]);
        }
        printf("\n-----\n");
    }
    */
//    printf("Multicast group of size %d:\n", (int)igraph_vector_size(&multicast_Group));
//    for(index=0; index<igraph_vector_size(&multicast_Group); index++){printf("%d ", (int)VECTOR(multicast_Group)[index]);}
//    printf("\n");

    start_Work = clock();
    ///====================================
    ///====================================

    igraph_vector_init(&used_Nodes, 0);

    solution = grasp_p(graph, &multicast_Group, IT_GRASP, IT_LS, size_Graph, threads, alpha, &used_Nodes, info);


    end_Work = (double)clock();



//    printf("%d used nodes:\n", (int)igraph_vector_size(&used_Nodes));
//    for(index=0; index<igraph_vector_size(&used_Nodes); index++){printf("%d ", (int)VECTOR(used_Nodes)[index]);}
//    printf("\n");
//
//    printf("Covered nodes:\n");
//    for(index=0; index<igraph_vector_size(&multicast_Group); index++)
//    {
//        if(igraph_vector_contains(&used_Nodes, (int)VECTOR(multicast_Group)[index])){match++;printf("%d ", (int)VECTOR(multicast_Group)[index]);}
//        else{printf("II %d II ", (int)VECTOR(multicast_Group)[index]);}
//    }
//    printf("\nThe tree cover %d of %d members of the Multicast group.\n", match, (int)igraph_vector_size(&multicast_Group));





    function_to_save(graph_Name, start_Work, end_Work, solution, &used_Nodes, alpha, threads, IT_GRASP, info);

    solution = dell_tree(solution, &used_Nodes);
    ///====================================
    ///====================================

    for(index = 0; index < IT_GRASP; index++){free(info[index]);}

    free(info);
}


void main(int argc, char *argv[])
{
    int i, index, size_Graph = 0;
    float j;
    char graph_Name[50];
    typeNode *graph;
    igraph_vector_t multicast_Group;

    strcpy(graph_Name, argv[1]);

    graph = function_to_read(&multicast_Group, graph_Name, &size_Graph);
    ///....................................................
    for(i=1; i<=4; i*=2)
        for(j=0; j<=0.5; j+=0.1)
            uso_grasp(graph, size_Graph, multicast_Group, graph_Name, i, j);

    ///....................................................

    for(i = 0; i<size_Graph; i++)
    {
        igraph_vector_destroy(&graph[i].neighbors);
        igraph_vector_destroy(&graph[i].edge_Weight);
    }

    free(graph);
    igraph_vector_destroy(&multicast_Group);
}
