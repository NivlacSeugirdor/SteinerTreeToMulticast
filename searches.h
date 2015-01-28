/**======================================
	Author: Calvin Rodrigues da Costa
=======================================*/

#include <stdio.h>
#include <stdlib.h>
#include <igraph.h>
#include "utilities.h"
#include "structures.h"


///Headers
sol* prim_sg(typeNode* graph, int size_Graph, igraph_vector_t* m_Group, float n_Random, float alpha, igraph_vector_t* used_Nodes);

sol* local_search(typeNode* graph, igraph_vector_t* m_Group, igraph_vector_t* used_Nodes, int max_Iter, sol* solution);


///Functions
sol* prim_sg(typeNode* graph, int size_Graph, igraph_vector_t* m_Group, float n_Random, float alpha, igraph_vector_t* used_Nodes)
{
    sol* solution = new_sol();

    int size_M_Group, size_U_Nodes;
    int index, bool_Change, size_Changed;

    float *cost_Nodes;

    res *target_Node, *parent;

    igraph_vector_t changed;
	igraph_vector_t m_Nodes;

	igraph_vector_init(&changed, 0);

	cost_Nodes = (float*)malloc(sizeof(float)*(size_Graph+1));
	for(index = 0; index<size_Graph; index++){cost_Nodes[index] = INF;}


	typeEdge* chosen_One = NULL;

	igraph_vector_copy(&m_Nodes, m_Group);

	igraph_vector_init(used_Nodes, 1);
    VECTOR(*used_Nodes)[0] = VECTOR(m_Nodes)[0];

	solution->root = new_res((int)VECTOR(m_Nodes)[0], 0);
    cost_Nodes[(int)VECTOR(m_Nodes)[0]-1] = 0; /// root cost = 0, ever.
    igraph_vector_remove(&m_Nodes, 0);



    size_U_Nodes = 1;
    bool_Change = 0;
    size_Changed = 0;
    size_M_Group = (int)igraph_vector_size(&m_Nodes);



    while(size_M_Group > 0)
    {
        chosen_One = selection(alpha, &size_U_Nodes, graph, used_Nodes, n_Random, cost_Nodes, &bool_Change, &changed, size_Changed, m_Group);

        if(chosen_One==NULL){break;}
        else if(bool_Change)
        {
            target_Node = find_node(solution->root, chosen_One->child);
            parent = find_node(solution->root, chosen_One->parent);

            cost_Nodes[chosen_One->child-1] = parent->cost + chosen_One->weight;

            apply_changes(target_Node, NULL, parent, used_Nodes, m_Group, graph, solution, chosen_One->weight, &size_U_Nodes, used_Nodes, cost_Nodes);

            change_cost(solution, target_Node, cost_Nodes);

            size_U_Nodes = igraph_vector_size(used_Nodes);

            control_changes(size_U_Nodes, used_Nodes, &size_Changed, &changed);

            if(size_Changed == (int)(size_Graph*0.3))
			{
				igraph_vector_remove(&changed, 0);
				size_Changed--;
			}

         	igraph_vector_insert(&changed, size_Changed, chosen_One->child);
			size_Changed++;

            free(chosen_One);
        }else
        {
            parent = find_node(solution->root, chosen_One->parent);

            cost_Nodes[chosen_One->child-1] = parent->cost + chosen_One->weight;

            solution = add_node(solution, chosen_One, used_Nodes, &m_Nodes, &size_M_Group);

            size_U_Nodes += 1;
        }
    }

//    /**
        //to debug
        int match = 0;
        printf("==============\nCovered nodes:\n");
        for(index=0; index<igraph_vector_size(m_Group); index++)
        {
            if(igraph_vector_contains(used_Nodes, (int)VECTOR(*m_Group)[index])){match++;printf("%d ", (int)VECTOR(*m_Group)[index]);}
            else{printf("II %d II ", (int)VECTOR(*m_Group)[index]);}
        }
        printf("\nThe tree cover %d of %d members of the Multicast group.\n", match, (int)igraph_vector_size(m_Group));

        if(match<(int)igraph_vector_size(m_Group)){scanf("%*c");}
//       */
    solution->root = prune_tree(solution, solution->root, m_Group, used_Nodes);
    /**
        //to debug
        match = 0;

        printf("Covered nodes:\n");
        for(index=0; index<igraph_vector_size(m_Group); index++)
        {
            if(igraph_vector_contains(used_Nodes, (int)VECTOR(*m_Group)[index])){match++;printf("%d ", (int)VECTOR(*m_Group)[index]);}
            else{printf("II %d II ", (int)VECTOR(*m_Group)[index]);}
        }
        printf("\nThe tree cover %d of %d members of the Multicast group.\n==============\n", match, (int)igraph_vector_size(m_Group));
    */
    igraph_vector_destroy(&changed);
    igraph_vector_destroy(&m_Nodes);

    free(cost_Nodes);

	return solution;
}

sol* local_search(typeNode* graph, igraph_vector_t* m_Group, igraph_vector_t* used_Nodes, int max_Iter, sol* solution)
{
    int index, index_Best, boolean_Best = 0, root;
	int size_Copy, current_Node, i, size_U_Nodes;

	igraph_vector_t copy_U_Nodes, neighbors, neighbors_Weight;

    typeEdge* movement;
    res* target;

    root = (int)VECTOR(*m_Group)[0];

    for(index = 0, index_Best = 0, size_Copy = 0; index - index_Best < max_Iter; index++)
	{

	    igraph_vector_copy(&copy_U_Nodes, used_Nodes);
        size_Copy = (int)igraph_vector_size(&copy_U_Nodes);

	    while(size_Copy > 0)
        {

            current_Node = (int)VECTOR(copy_U_Nodes)[0];

            if(current_Node!=root && current_Node>0)
            {
                target = find_node(solution->root, current_Node);

                if(target!=NULL)
                {
                    igraph_vector_copy(&neighbors, &graph[current_Node-1].neighbors);
                    igraph_vector_copy(&neighbors_Weight, &graph[current_Node-1].edge_Weight);
                    solution = selects_best(neighbors, neighbors_Weight, used_Nodes, m_Group, target, &boolean_Best, solution, graph, &size_Copy, &copy_U_Nodes);
                }
            }

            if(boolean_Best){index_Best = index;}
            igraph_vector_remove(&copy_U_Nodes, 0);
            size_Copy--;
        }
    }

    return solution;
}
