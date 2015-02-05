/**======================================
	Author: Calvin Rodrigues da Costa
=======================================*/

#include <stdio.h>
#include <stdlib.h>
#include <igraph.h>

#include <omp.h>
#include <time.h>
#include "structures.h"
#include "searches.h"


sol* grasp_p(typeNode* graph, igraph_vector_t* m_Group, int iter, int iter_LS, int size_Graph, int threads_GRASP, float alpha, igraph_vector_t * best_Used_Nodes, float** info, float percent)
{
    sol* solution = NULL, *best_Solution = NULL, *aux;
    igraph_vector_t used_Nodes;

    int index;

    float weight = 0, r_Number;

    omp_set_num_threads(threads_GRASP);

	#pragma omp parallel for private(r_Number, solution, weight, used_Nodes) shared(best_Solution, best_Used_Nodes)
	for(index = 0; index<iter; index++)
    {
        r_Number = (time(NULL)*(omp_get_thread_num() + 1 + weight));

        igraph_vector_init(&used_Nodes, 1);

		solution = prim_sg(graph, size_Graph, m_Group, r_Number, alpha, &used_Nodes, percent);

        if(solution!=NULL)
        {
            solution = local_search(graph, m_Group, &used_Nodes, iter_LS, solution);

            weight = solution->total_Weight;

            #pragma omp critical
            {
                if(best_Solution == NULL)
                {
                    best_Solution = solution;
                    igraph_vector_copy(best_Used_Nodes, &used_Nodes);
                }else if(best_Solution->total_Weight > solution->total_Weight)
                {
                    aux = best_Solution;
                    best_Solution = solution;
                    dell_tree(aux, best_Used_Nodes);

                    igraph_vector_destroy(best_Used_Nodes);
					igraph_vector_copy(best_Used_Nodes, &used_Nodes);
                }else{dell_tree(solution, &used_Nodes);}
            }
        }

        igraph_vector_destroy(&used_Nodes);

        info[index][0] = (double)clock();
		info[index][1] = weight;

	}

    return best_Solution;
}
