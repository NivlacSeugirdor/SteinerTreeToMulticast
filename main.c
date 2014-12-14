#include <stdio.h>
#include <stdlib.h>
#include "GRASP.h"
#include <igraph.h>
#define IT_GRASP 8
#define IT_LS 1
//gcc -fopenmp structures.h utilities.h searches.h GRASP.h main.c  -I/usr/include/igraph -L/usr/local/lib -ligraph -o TCC2


//void main()
//{
//    ///Test cases
//    char names[1][50] = {{"teste"}};
//
//    ///To solve
//    sol* solution;
//
//    ///Graph
//    typeNode* graph;
//
//    ///Used Nodes && Multicast Group
//    igraph_vector_t used_Nodes, m_Group;
//
//    ///Clocks
//    clock_t start_Work, end_Work;
//
//    ///Integers
//    int size_Graph, index_Aux;
//    int index_Cases, threads;
//
//    ///Float
//    float alpha;
//    float **info;
//
//
//    ///Start the igraph vectors
//    igraph_vector_init(&used_Nodes, 0);
//    igraph_vector_init(&m_Group, 0);
//
//    for(index_Cases = 0; index_Cases < 4; index_Cases++)
//    {
//        graph = function_to_read(&m_Group, names[index_Cases], &size_Graph);
//        info = (float**)malloc(sizeof(float)*80);
//
//        for(index_Aux = 0; index_Aux<80; index_Aux++){info[index_Aux] = (float*)malloc(sizeof(float)*2);}
//
//        for(threads = 1; threads <= 4; threads*=2)
//        {
//            for(alpha = 0; alpha < 1; alpha+=0.1)
//            {
//                solution = new_sol();
//                start_Work = clock();
//
//                solution = GRASP_P(graph, &m_Group, 80, 10, size_Graph, threads, alpha, &used_Nodes, info);
//
//
//                end_Work = (double)clock();
//
//                function_to_save(names[index_Cases], start_Work, end_Work, solution, &used_Nodes, alpha, threads, 80, info);
//
//                dell_tree(solution, &used_Nodes);
//            }
//        }
//
//        for(index_Aux = 0; index_Aux<80; index_Aux++){free(info[index_Aux]);}
//        free(info);
//        for(index_Aux = 0; index_Aux<size_Graph; index_Aux++)
//        {
//            igraph_vector_destroy(&graph[index_Aux].neighbors);
//            igraph_vector_destroy(&graph[index_Aux].edge_Weight);
//        }
//
//        free(graph);
//    }
//}

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


//void main()
//{
//    sol* solution_Test;
//    int size_Graph;
//
//    solution_Test = new_sol();
//    igraph_vector_t used_Nodes, multicast_Group;
//    typeNode* graph = (typeNode*)malloc(sizeof(typeNode)*20);
//    clock_t start, end_Work;
//    float info[20][2];
//
//    char graph_Name[] = "teste";
//    int i, maxI, j;
//    /**
//        Preparando as arestas.
//        Montando o grafo.
//    */
//    printf("Lendo arquivo\n");
//    graph = function_to_read(&multicast_Group, graph_Name, &size_Graph);
//
//    maxI = (int)igraph_vector_size(&multicast_Group);
//    printf("Grupo Multicast:\n");
//    printf("Size MG %d\n", maxI);
//    for(i = 0; i < maxI; i++){printf("%d  ", (int)VECTOR(multicast_Group)[i]);}
//
//    printf("\n\nGrafo\n\n");
//    for(i = 0; i<size_Graph; i++)
//    {
//        printf("No : %d Numero vizinhos: %d.\nVizinhos: \n", graph[i].value, graph[i].size_Neighborhood);
//        for(j = 0; j < graph[i].size_Neighborhood; j++){printf(": Vizinho %d Peso Aresta %f :", (int)VECTOR(graph[i].neighbors)[j], (float)VECTOR(graph[i].edge_Weight)[j]);}
//        printf("\n\n");
//    }
//
//    start = clock();
//    printf("\nExecutando GRASP.\n");
//    igraph_vector_init(&used_Nodes, 0);
//    solution_Test = GRASP_P(graph, &multicast_Group, 20, 10, size_Graph, 4, 0.6, &used_Nodes, info);
//
//
//    printf("Peso total da solução: %f\n", solution_Test->total_Weight);
//    end_Work = (double)clock();
//
//    function_to_save(graph_Name, start, end_Work, solution_Test, &used_Nodes, 0.6, 4, 20, info);
//
//    /**
//        Teste dell_tree
//    */
//    printf("Verificando desalocação de memória:\n");
//    solution_Test = dell_tree(solution_Test, &used_Nodes);
//    printf("Solution: %p\n", solution_Test);
//    printf("DONE!\n");
//}
