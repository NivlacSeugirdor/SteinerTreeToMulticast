/**======================================
	Author: Calvin Rodrigues da Costa
=======================================*/

#include <stdio.h>
#include <stdlib.h>
#include <igraph.h>
//#include <omp.h>
#include "utilities.h"
#include "structures.h"

/**
	This library has functions for:
		- Prim semi-greedy; --toDo
		- Local Search.  --toDo
*/

/**
=======================================================================================================================
=======================================================================================================================
*/
	/** */
	sol* PRIM_SG(typeNode* graph, int size_Graph, igraph_vector_t* m_Group, float n_Random, float alpha, igraph_vector_t* used_Nodes);

	sol* local_search(typeNode* graph, igraph_vector_t* m_Group, igraph_vector_t* used_Nodes, int max_Iter, sol* solution);

/**
=======================================================================================================================
=======================================================================================================================
*/

sol* PRIM_SG(typeNode* graph, int size_Graph, igraph_vector_t* m_Group, float n_Random, float alpha, igraph_vector_t* used_Nodes)
{
	sol* solution = new_sol();
	int size_M_Group, size_U_Nodes;
	int i, bool_Change = 0, size_Changed = 0;
    float *cost_Nodes;
    res* target_Node, *parent;

	igraph_vector_t changed;
	igraph_vector_init(&changed, 0);

    cost_Nodes = (float*)malloc(sizeof(float)*size_Graph);

    for(i = 0; i<size_Graph; i++){cost_Nodes[i] = INF;}

	igraph_vector_t m_Nodes;
	typeEdge* chosen_One = NULL;

    igraph_vector_copy(&m_Nodes, m_Group);

    igraph_vector_init(used_Nodes, 1);
    VECTOR(*used_Nodes)[0] = VECTOR(m_Nodes)[0];
    solution->root = new_res((int)VECTOR(m_Nodes)[0], 0);
    cost_Nodes[0] = 0;

    igraph_vector_remove(&m_Nodes, 0);

    size_U_Nodes=1;
    size_M_Group = (int)igraph_vector_size(&m_Nodes);

	while(size_M_Group > 0)
	{
        chosen_One = selection(alpha, &size_U_Nodes, graph, used_Nodes, n_Random, cost_Nodes, &bool_Change, &changed, size_Changed);

//        if(size_M_Group<100){printf("Size U Nodes %d Size m group %d restricao %d, size Graph %d bool %d\n", size_U_Nodes, size_M_Group, (int)(size_Graph*0.3), size_Graph, bool_Change);}

		if(chosen_One == NULL){}
		else if(bool_Change)
		{
	
			target_Node = find_node(solution->root, chosen_One->child);
			parent = find_node(solution->root, chosen_One->parent);
			cost_Nodes[chosen_One->child-1] = parent->cost + chosen_One->weight;
			apply_changes(target_Node, NULL, parent, used_Nodes, m_Group, graph, solution, chosen_One->weight, &size_U_Nodes, used_Nodes);
			change_cost(solution, target_Node, cost_Nodes);
	
			size_U_Nodes = igraph_vector_size(used_Nodes);
	
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
		}
	}

    solution->root = prune_tree(solution, solution->root, m_Group, used_Nodes);

    size_U_Nodes = (int)igraph_vector_size(used_Nodes);


	igraph_vector_destroy(&changed);
    igraph_vector_destroy(&m_Nodes);
    free(cost_Nodes);
	return solution;
}

/**
    procedimento busca_local
        01- Recebe: Grafo, grupo_Multicast, nós_Na_Árvore, max_Iterações, árvore;
        02- Vars: índice, houve_Melhoria, cópia_Nós_Na_Arv;
        03- Vars: movimento, nó_Selecionado;
        04- índice = 0, houve_Melhoria = 0;
        05- Enquanto índice-houve_Melhoria < max_Iterações faça
        06-         cópia_Nós_Na_Arv <- nós_Na_Árvore;
        07-         Enquanto cópia_Nós_Na_Arv != vazio faça
        08-                 nó_Atual = cópia_Nós_Na_Arv[0];
        09-                Se nó_Atual != grupo_Multicast[0] faça
        10-                      nó_Selecionado = pega_Nó(árvore, nó_Atual);
        11-                      vizinhos <-  Grafo[nó_Atual]->vizinhos;
        12-                      pesos <-  Grafo[nó_Atual]->pesos;
        13-                      movimento = seleciona_movimento(vizinhos, peso, nós_Na_Arvore, grupo_Multicast, nó_Selecionado);
        14-                      Se movimento != NULL faça
        15-                            houve_Melhoria = índice;
        16-                            aplica_mudança(árvore, movimento, nós_Na_Árvore, grupo_Multicast);
        17-                            remove(cópia_Nós_Arv, nó_Atual);
        18-                       fim Se
        19-                fim Se
        20-         fim Enquanto
        21-fim Enquanto
        22-retorna árvore;
    fim procedimento
*/

sol* local_search(typeNode* graph, igraph_vector_t* m_Group, igraph_vector_t* used_Nodes, int max_Iter, sol* solution)
{
	int index, index_Best, boolean_Best = 0;
	int size_Copy, current_Node, i, size_U_Nodes, teste;

	igraph_vector_t copy_U_Nodes, neighbors, neighbors_Weight;

    typeEdge* movement;
    res* target;

	for(index = 0, index_Best = 0, size_Copy = 0; index - index_Best < max_Iter; index++)
	{
		igraph_vector_copy(&copy_U_Nodes, used_Nodes);
        size_Copy = (int)igraph_vector_size(&copy_U_Nodes);
//        printf("\n========================================================================\n========================================================================\n");
//        printf("Index %d Index Best %d dif %d\n\n", index, index_Best, index-index_Best);
//        teste = (int)igraph_vector_size(used_Nodes);
//        printf("%d\n", teste);
//        printf("Vértices utilizados:\n");
//        for(i = 0; i < teste; i++){printf("%d - ", (int)VECTOR(*used_Nodes)[i] );}
//        printf("\n");

		while(size_Copy > 0)
        {
            current_Node = (int)VECTOR(copy_U_Nodes)[0];
//            printf("Current Node %d - ", current_Node);
            teste = (int)igraph_vector_size(used_Nodes);

            if(current_Node != (int)VECTOR(*m_Group)[0])
            {
                target = find_node(solution->root, current_Node);

                igraph_vector_copy(&neighbors, &graph[current_Node-1].neighbors);
                igraph_vector_copy(&neighbors_Weight, &graph[current_Node-1].edge_Weight);

                solution = selects_best(neighbors, neighbors_Weight, used_Nodes, m_Group, target, &boolean_Best, solution, graph,
                                        &size_Copy, &copy_U_Nodes);
            }

            if(boolean_Best){index_Best = index;}
            igraph_vector_remove(&copy_U_Nodes, 0);
            size_Copy--;
        }
	}

	size_U_Nodes = (int)igraph_vector_size(used_Nodes);
//    printf("Vértices utilizados:\n");
//    for(i = 0; i < size_U_Nodes; i++){printf("%d - ", (int)VECTOR(*used_Nodes)[i] );}
//    printf("\n");

    return solution;
}
