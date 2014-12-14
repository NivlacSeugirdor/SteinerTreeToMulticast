#ifndef utilities_H_INCLUDED
#define utilities_H_INCLUDED

/**======================================
	Author: Calvin Rodrigues da Costa
=======================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <igraph.h>
#include <string.h>
#include "structures.h"
#define INF 99999999 /** To infinity and beyond! */
/**
	This library has functions for:
		- Read archive in SteinLib format;
		- Save the results;
		- Select a edge to add int the tree;
		- 3 sub functions to:
			* Select a random edge;
			* Build an ordered list of edges;
			* Build an list of edges.
*/


/**
=======================================================================================================================
=======================================================================================================================
*/

	typeNode* function_to_read(igraph_vector_t* m_Group, char graph_Name[], int* size_Graph); //done

	void function_to_save(char graphName[], float time_in, float time_out, sol* solution, igraph_vector_t* used_Nodes,
                        float alpha, int threads, int iter, float **info); 

    typeEdge* selection(float alpha, int *size_U_Nodes, typeNode* graph, igraph_vector_t* used_Nodes, float r_Number, float *cost_Nodes, int *bool_Change, igraph_vector_t* changed, int size_Changed);
	//Sub functions
	void insert_node(typeNode* graph, int in, int out, float weight); //done

   	typeEdge* get_edge(typeEdge* restrict_List, int size_List, float random_Number, int err); // Done

	typeEdge* increase_list(typeEdge* res_List, int q_Edges, int parent, int child, float weight);//done

    res* apply_changes(res* target_Node, typeEdge* new_Path, res* new_Parent, igraph_vector_t *used_Nodes, igraph_vector_t *m_Group, typeNode* graph, sol* solution, float edge_Weight
                            , int* size_Aux_U_Nodes, igraph_vector_t* aux_U_Nodes);

	void get_min(igraph_vector_t nodes, igraph_vector_t weights, int *index_Min, float *min_Weight, int root);

	typeEdge* get_path(igraph_vector_t* used_Nodes, int parent, int child, typeNode* graph, typeEdge* edge, float *cost_Path, int *use_Check);

	sol* selects_best(igraph_vector_t nodes, igraph_vector_t weights, igraph_vector_t *used_Nodes, igraph_vector_t *m_Group, res* target_Node, int *boolean_Best, sol* solution, typeNode* graph
                               , int* size_Aux_U_Nodes, igraph_vector_t* aux_U_Nodes);

    int find_position(igraph_vector_t* target, int value);
/**
=======================================================================================================================
=======================================================================================================================
*/

void insert_node(typeNode* graph, int node_I, int node_O, float weight) //O(n) -> n = vector_Size
{ //This function make a ordered insertion
    int index, vector_Size;
//    printf("N.I %d N.O %d, W %f\n", node_I, node_O, weight);
//    graph[node_I-1].value = node_I;

    for(index = 0; index < graph[node_I-1].size_Neighborhood; index++)
    {
        if(VECTOR(graph[node_I-1].neighbors)[index] > node_O)
        {
            igraph_vector_insert(&graph[node_I-1].neighbors, index, node_O);
            igraph_vector_insert(&graph[node_I-1].edge_Weight, index, weight);
            graph[node_I-1].size_Neighborhood++;
            return;
        }
    }
    igraph_vector_insert(&graph[node_I-1].neighbors, index, node_O);
    igraph_vector_insert(&graph[node_I-1].edge_Weight, index, weight);

    graph[node_I-1].size_Neighborhood++;
}


typeNode* function_to_read(igraph_vector_t* m_Group, char graph_Name[], int* size_Graph)
{// O(l*m + n) -> l = quantity of edges, m = quantity of neighbors to node, n = size of multicast group.
	FILE *arc;
	char archive[50];
	strcpy(archive, graph_Name);
	strcat(archive, ".stp\0");
	arc = fopen(archive, "r");

	if(arc==NULL)
	{
		printf("Problem with archive %s!\n", graph_Name);
		return NULL;
	}

	char buffer[51];
	int stage, nodes, edges, it, index;
    typeNode* graph;
    int node_I; //in
    int node_O; //out
    float weight=0;


	for(stage = 1, nodes = 0, edges = 0; stage < 5; )
	{
		switch(stage)
		{
			case 1:
				fscanf(arc, "%s", buffer);
				if(strcmp(buffer, "END")==0){stage++;}
			break;

			case 2:
				fscanf(arc, "%s", buffer);

				if(strcmp(buffer, "SECTION")==0){fscanf(arc, "%s", buffer);}
				else if(strcmp(buffer, "Nodes")==0)
				{
					fscanf(arc, "%d %s %d", &nodes, buffer, &edges);
					graph = (typeNode*)malloc(sizeof(typeNode)*nodes);

                    *size_Graph = nodes;
                    for(index = 0; index < nodes; index++)
                    {
                        graph[index].value = index+1;
                        graph[index].size_Neighborhood = 0;
                    }

					stage++;
				}

			break;

			case 3:
			    /**
                    Here the graph is initialized:
                        . The value of the nodes will be set;
                        . Also the neighborhood of each node will be defined.
                    PS: It is an undirected graph.
                */
				for(it = 0; it < edges; it++)
				{
				    fscanf(arc, "%s %d %d %f", buffer, &node_I, &node_O, &weight);

                    if(graph[node_I-1].size_Neighborhood == 0)
				    {
				        graph[node_I-1].size_Neighborhood++;

				        igraph_vector_init(&graph[node_I-1].neighbors, 1);
				        igraph_vector_init(&graph[node_I-1].edge_Weight, 1);
				        VECTOR(graph[node_I-1].neighbors)[0] = node_O;
				        VECTOR(graph[node_I-1].edge_Weight)[0] = weight;
				    }else{insert_node(graph, node_I, node_O, weight);}

				    if(graph[node_O-1].size_Neighborhood == 0)
				    {
				        graph[node_O-1].size_Neighborhood++;

				        igraph_vector_init(&graph[node_O-1].neighbors, 1);
				        igraph_vector_init(&graph[node_O-1].edge_Weight, 1);
				        VECTOR(graph[node_O-1].neighbors)[0] = node_I;
				        VECTOR(graph[node_O-1].edge_Weight)[0] = weight;
				    }else{insert_node(graph, node_O, node_I, weight);}
				}

				fscanf(arc, "%s", buffer);

				stage++;
			break;

			case 4:
				fscanf(arc, "%s", buffer);

				if(strcmp(buffer, "SECTION")==0){fscanf(arc, "%s", buffer);}
				else if(strcmp(buffer, "Terminals")==0)
				{
					int q_Member = 0, member = 0;

                    fscanf(arc, "%d", &q_Member);
					igraph_vector_init(m_Group, 0);
//                    printf("%d \n", q_Member);
					for(it = 0; it < q_Member; it++)
					{
						fscanf(arc, "%s %d", buffer, &member);
//						printf("%s %d ", buffer, member);
						igraph_vector_insert(m_Group, it, member);
					}
//					printf("\n");
					stage++;
				}
			break;
		}
	}

	fclose(arc);
	return graph;
}


void function_to_save(char graphName[], float time_in, float time_out, sol* solution, igraph_vector_t* used_Nodes,
                        float alpha, int threads, int iter, float **info) 
{
	FILE* new_Arc;
	int i, s;
    char aux[30], aux2[30];
	char archive[100];
	igraph_vector_t n_In, n_Out;

	igraph_vector_init(&n_In, 0);
	igraph_vector_init(&n_Out, 0);

	strcpy(archive, graphName);
	strcat(archive, "_");
    sprintf(aux, "%f", alpha);
	strcat(archive, aux);
	strcat(archive, "_");
	sprintf(aux2, "%d", threads);
	strcat(archive, aux2);
	strcat(archive, "_result.txt\0");
	new_Arc = fopen(archive, "a");

	fprintf(new_Arc, "SECTION Comment\n");

	fprintf(new_Arc, "Name ");

	fprintf(new_Arc, "%s", graphName);

	fprintf(new_Arc, "\nProblem STPG\n");

    fprintf(new_Arc, "Program GRASP_P\n");

    fprintf(new_Arc, "Version 1.0\n");

    fprintf(new_Arc, "End\n\n\n");

    fprintf(new_Arc, "SECTION Solutions\n");

    for(i = 0; i < iter; i++)
    {
        fprintf(new_Arc, "Solution %f %f\n", (info[i][0]-time_in)/CLOCKS_PER_SEC, info[i][1]);
    }

    fprintf(new_Arc, "End\n\n\n");

    fprintf(new_Arc, "SECTION Run\n");

    fprintf(new_Arc, "Threads %d\n", threads);

    fprintf(new_Arc, "Time %f\n", (time_out-time_in)/CLOCKS_PER_SEC);

    fprintf(new_Arc, "Primal %f\n", solution->total_Weight);

    fprintf(new_Arc, "End\n\n\n");

    fprintf(new_Arc, "SECTION Finalsolution\n");

    s = (int)igraph_vector_size(used_Nodes);

    fprintf(new_Arc, "Vertices %d\n", s);

    igraph_vector_sort(used_Nodes);

    for(i=0; i<s; i++){fprintf(new_Arc, "V %d\n", (int)VECTOR(*used_Nodes)[i]);}
    ///
    get_edges(solution, &n_In, &n_Out);

    s = (int)igraph_vector_size(&n_In);

    fprintf(new_Arc, "Edges %d\n", s);

    for(i=0; i<s; i++){fprintf(new_Arc, "V %d %d\n", (int)VECTOR(n_In)[i], (int)VECTOR(n_Out)[i]);}

    fprintf(new_Arc, "End\n\n\n");

    igraph_vector_destroy(&n_In);
    igraph_vector_destroy(&n_Out);
}


typeEdge* increase_list(typeEdge* res_List, int q_Edges, int parent, int child, float weight)// O(n)
{
	typeEdge* new_Edge, *aux = res_List;
	new_Edge  = new_edge(parent, child, weight);
    int execute;

    if(q_Edges > 0)
	{
	    execute = 1;
        while(execute)
        {
//            printf("Aux %d W %f Prev %p Next %p, New %d W %f :", aux->child, aux->weight, aux->prev, aux->next, new_Edge->child, new_Edge->weight);
            if(aux->weight >= new_Edge->weight)
            {
                if(aux->prev == NULL)
                {
                    aux->prev = new_Edge;
                    new_Edge->next = aux;
                    res_List = new_Edge;
                }else
                {
                    aux->prev->next = new_Edge;
                    new_Edge->next = aux;
                    new_Edge->prev = aux->prev;
                    aux->prev = new_Edge;
                }
                execute = 0;
            }else
            {
                if(aux->next!=NULL){aux = aux->next;}
                else
                {
                    aux->next = new_Edge;
                    new_Edge->prev = aux;
                    execute = 0;
                }
            }
        }


	}else{res_List = new_Edge;}

//    for(aux = res_List; aux!=NULL; aux = aux->next)
//    {
//            printf("C %d W %f ::: ", aux->child, aux->weight);
////            scanf("%*c");
//    }
//    printf("\n");

	return res_List;
}

typeEdge* dell_edges(typeEdge* res_List)// O(n)
{
	typeEdge *aux, *second_Aux;
    for(aux = res_List; aux!=NULL; )
    {
        second_Aux = aux->next;
        free(aux);
        aux = second_Aux;
    }
	return NULL;
}

///OK
typeEdge* selection(float alpha, int *size_U_Nodes, typeNode* graph, igraph_vector_t* used_Nodes, float r_Number, float *cost_Nodes, int *bool_Change, igraph_vector_t* changed, int size_Changed)//O(n^3)
{
    ///----------------------------
    int first_Index, second_Index;
    int size_Neighborhood, q_Edges;
    int parent, child, q_New, err = 1, pos;
    ///----------------------------
    float weight, maximun = 0;
    float res = 0, minimun = INF;
    ///----------------------------
    typeEdge* res_List = NULL, *chosen_One = NULL, *aux = NULL;
    ///----------------------------
    igraph_bool_t check;
    igraph_vector_t neighbors, exceptions;
    ///----------------------------
    int i;
    //printf("Exceptions %p\n", &exceptions);
    igraph_vector_init(&exceptions, 0);
    //printf("==\n");
    ///Select the neighborhood
//    printf("%d \n", *size_U_Nodes);

//    printf("Used Nodes\n");
//    for(i = 0; i < *size_U_Nodes; i++){printf("%d ", (int)VECTOR(*used_Nodes)[i]);}
//    printf("\n");
    for(first_Index = 0, q_Edges = 0; first_Index < *size_U_Nodes; first_Index++)
    {
        ///Take the parent
        parent = (int)VECTOR(*used_Nodes)[first_Index];

        igraph_vector_copy(&neighbors, &graph[parent-1].neighbors);

        size_Neighborhood = graph[parent-1].size_Neighborhood;

        for(second_Index = 0; second_Index < size_Neighborhood; second_Index++)
        {

            check = igraph_vector_contains(used_Nodes, VECTOR(neighbors)[0]);

            ///Take actual weight and child to analyze
            weight = (float)VECTOR(graph[parent-1].edge_Weight)[second_Index];
            child = (int)VECTOR(neighbors)[0];

            if(!check || (cost_Nodes[parent-1] + weight) < cost_Nodes[child-1])
            {
                if(check && (cost_Nodes[parent-1] + weight) < cost_Nodes[child-1]){igraph_vector_insert(&exceptions, 0, child);}

                res_List = increase_list(res_List, q_Edges, parent, child, weight);

                q_Edges++;
//                printf("%p\n", res_List);
//                printf("Max %f, Min %f, Wei %f\n, Max? %d, Min? %d\n", maximun, minimun, weight, (weight > maximun), (weight < minimun));
                //if(weight >=12976958){scanf("%*c");}
                //if(weight <=10){scanf("%*c");}
                
                if(weight > maximun){maximun = weight;}
                if(weight < minimun){minimun = weight;}
            }
            igraph_vector_remove(&neighbors, 0);
        }

    }

    ///Selects some neighbors.
    while(alpha <= 1 && chosen_One == NULL)
    {
	    res = minimun + alpha*(maximun-minimun);
	  //  printf("Alpha %f Rest %f Max %f Min %f\n", alpha, res, maximun, minimun);

	//    printf("Candidatos\n");
		for(aux = res_List, q_New = 0; aux!=NULL; aux = aux->next)
	    {
	//            printf("C %d W %f ::: ", aux->child, aux->weight);
		    if(aux->weight <= res){q_New++;}
	    }
	//    printf("\n");
		if(size_Changed == 0) chosen_One = get_edge(res_List, q_New, r_Number, err); //O(1)
		else
		{
//			printf("Q %d\n", q_New);
			do
			{
				chosen_One = get_edge(res_List, q_New, r_Number, err);
				err++;
				pos = find_position(changed, chosen_One->child);
				if(pos!=-1)
				{
					free(chosen_One);
					chosen_One = NULL;
				}
				//printf("%d - ", (err <= q_New));
			}while(pos != -1 && err <= q_New);
		}

	    if(chosen_One != NULL)
	    {
		    if(find_position(&exceptions, chosen_One->child) >= 0){*bool_Change = 1;}
		    else
		    {
                *bool_Change = 0;
                *size_U_Nodes +=1;
		    }
	    }else
	    {
            q_New = 0;
            alpha+=0.01;
	    }

    }
//	printf("Alpha %f, Chosen %p\n", alpha, chosen_One);
//    printf("BC %d\n", *bool_Change);

    igraph_vector_destroy(&exceptions);

    dell_edges(res_List);

    return chosen_One;
}


///OK
typeEdge* get_edge(typeEdge* restrict_List, int size_List, float random_Number, int err) //O(n)
{
	typeEdge *chosen_One, *aux = restrict_List;

    srand(random_Number*(err));
//
//    while(aux->prev!=NULL){aux = aux->next;}
//    aux2
    int value = (int)random()%size_List;
    int index;
//    printf("Get\n");
    for(index = 0; index < value && aux->next!=NULL; index++, aux = aux->next){/*printf("%d ", aux->child);*/}
	chosen_One = new_edge(aux->parent, aux->child, aux->weight);
//    printf("Parent %d Child %d\n", aux->parent, aux->child);
	return chosen_One;
}

///OK
res* prepare_to_dell(res* target_Node)
{
    res* aux;
    aux = target_Node->parent;

    if(target_Node->prev_Bros == NULL && target_Node->brothers != NULL)
    {
        aux->children = target_Node->brothers;
        target_Node->brothers->prev_Bros = NULL;
        target_Node->brothers = NULL;
    }else if(target_Node->prev_Bros != NULL && target_Node->brothers != NULL)
    {
        target_Node->prev_Bros->brothers = target_Node->brothers;
        target_Node->brothers->prev_Bros = target_Node->prev_Bros;
        target_Node->brothers = NULL;
        target_Node->prev_Bros = NULL;
    }else if(target_Node->prev_Bros != NULL && target_Node->brothers == NULL)
    {
        target_Node->prev_Bros->brothers = NULL;
        target_Node->prev_Bros = NULL;
    }else{aux->children = NULL;}

    target_Node->parent = NULL;

    return aux;
}

///Looks OK
res* apply_changes(res* target_Node, typeEdge* new_Path, res* new_Parent, igraph_vector_t *used_Nodes, igraph_vector_t *m_Group, typeNode* graph, sol* solution, float edge_Weight, int* size_Aux_U_Nodes, igraph_vector_t* aux_U_Nodes)
{
    res* aux, *second_Aux;
    int execute, pos;

    aux = prepare_to_dell(target_Node);

    typeEdge* run_The_Path;
	
	
//	if(new_Parent->value > 500 || new_Parent->value < 1)
//    {
//	    printf("Child %d\n", target_Node->value);
//	    printf("New Parent %d\n", new_Parent->value);
	    printf("Old Parent %d\n", aux->value);
//    }
//    printf("\n============================================\n");
    if(new_Path == NULL)
    {
//        printf("\n============================================\n");
        
        solution->total_Weight -= target_Node->weight;
        target_Node->parent = new_Parent;
		
//		if(target_Node->parent->value > 500 || target_Node->parent->value < 1)
//        {
//        	printf("Perda Total\n");
//		    printf("Child %d\n", target_Node->value);
//		    printf("New Parent %d\n", new_Parent->value);
//		    printf("Old Parent %d\n", aux->value);
//       }
        target_Node->cost = new_Parent->cost+edge_Weight;

        target_Node->weight = edge_Weight;
        solution->total_Weight += target_Node->weight;

        if(new_Parent->children==NULL){new_Parent->children=target_Node;}
        else
        {
            for(second_Aux = new_Parent->children; second_Aux->brothers!=NULL ; second_Aux = second_Aux->brothers){}
            second_Aux->brothers = target_Node;
            target_Node->prev_Bros = second_Aux;
        }
    }else
    {

        run_The_Path = new_Path;

        second_Aux = new_Parent;

        while(run_The_Path->child != target_Node->value)
        {
            igraph_vector_insert(used_Nodes, 1, run_The_Path->child);

            if(second_Aux->children==NULL)
            {
                second_Aux->children = new_res(run_The_Path->child, run_The_Path->weight);
                second_Aux->children->cost = second_Aux->cost + second_Aux->children->weight;
                second_Aux->children->parent = second_Aux;
                solution->total_Weight += second_Aux->children->weight;

                second_Aux = second_Aux->children;
            }else
            {
                second_Aux = second_Aux->children;
//                printf("P %d C %d\n", run_The_Path->parent, run_The_Path->child);
                for(;second_Aux->brothers!=NULL; second_Aux = second_Aux->brothers){/*printf(":Aux %d :", second_Aux->value);*/}
//                printf(":Aux %d :", second_Aux->value);
//                printf("\n");

                second_Aux->brothers = new_res(run_The_Path->child, run_The_Path->weight);
//                printf("%d recebe %d como irmão;\n", second_Aux->value, second_Aux->brothers->value);
                second_Aux->brothers->cost = second_Aux->parent->cost + second_Aux->brothers->weight;
//                printf("%d recebe custo %f;\n", second_Aux->brothers->value, second_Aux->brothers->cost);
                second_Aux->brothers->parent = second_Aux->parent;
                second_Aux->brothers->prev_Bros = second_Aux;
                solution->total_Weight += second_Aux->brothers->weight;
//                printf("Novo peso da solução %f\n", solution->total_Weight);
                second_Aux = second_Aux->brothers;
            }
            run_The_Path = run_The_Path->next;
        }
//        printf("P %d C %d\n", run_The_Path->parent, run_The_Path->child);
        second_Aux->children = target_Node;
        target_Node->parent = second_Aux;
//        printf("%d recebe %d como filho;\n", second_Aux->value, target_Node->value);

        solution->total_Weight-=target_Node->weight;
//        printf("Antigo peso da solução %f\n", solution->total_Weight);
        target_Node->weight = edge_Weight;
        solution->total_Weight+=target_Node->weight;
//        printf("Novo peso da solução %f\n", solution->total_Weight);

//        printf("Antigo custo de %d: %f\n", target_Node->value, target_Node->cost);
        target_Node->cost = second_Aux->cost + target_Node->weight;
//        printf("Novo custo de %d: %f\n", target_Node->value, target_Node->cost);
/**		if(target_Node->parent->value > 500 || target_Node->parent->value < 1)
        {
        	printf("Perda Total\n");
		    printf("Child %d\n", target_Node->value);
		    printf("New Parent %d\n", new_Parent->value);
		    printf("Old Parent %d\n", aux->value);
        }
*/
    }

    if(new_Path!=NULL){dell_edges(new_Path);}

    change_cost(solution, target_Node, NULL);//OK
    execute = 1;

    while(execute)
    {
//            printf("Node %d\n", aux->value);
            if(aux->children!=NULL || igraph_vector_contains(m_Group, aux->value))
            {
//                printf("Salvo! Child %p\n", aux->children);
                execute = 0;
            }else
            {
                    pos = find_position(used_Nodes, aux->value);///!!!!!
                    igraph_vector_remove(used_Nodes, pos);
//                    printf("Node %d != children (%p) pos 1: %d\n", aux->value, aux->children, pos);
                    pos = find_position(aux_U_Nodes, aux->value);

                    if(pos > -1)
                    {
                        igraph_vector_remove(aux_U_Nodes, pos);
                        *size_Aux_U_Nodes -= 1;
                    }
                    second_Aux = prepare_to_dell(aux);
                    solution->total_Weight-=aux->weight;

                    free(aux);

                    aux = second_Aux;
            }
    }
//    printf("Novo peso da solução %f\n", solution->total_Weight);
//    printf("============================================\n");
    return new_Parent;
}

///OK
void get_min(igraph_vector_t nodes, igraph_vector_t weights, int *index_Min, float *min_Weight, int root)//O(n)
{
	int size_Vector = (int)igraph_vector_size(&nodes), index;
	*index_Min = 0;
    *min_Weight = INF;

	for(index = 0; index < size_Vector; index++)
	{
//	    printf("index %d MW %f W %f, N %d R %d\n", index, *min_Weight, (float)VECTOR(weights)[index], (int)VECTOR(nodes)[index], root);
		if(*min_Weight >= (float)VECTOR(weights)[index] && (int)VECTOR(nodes)[index] != root)
		{
			*min_Weight = (float)VECTOR(weights)[index];
			*index_Min = index;
        }
	}
}

///Looks OK
typeEdge* get_path(igraph_vector_t* used_Nodes, int parent, int child, typeNode* graph, typeEdge* edge, float *cost_Path, int *use_Check)//OK
{
	int check_Used = 0, execute = 1, index = -1, chosen;
	int current_Node = parent, prev_Node = child, pos_U = -1;
    *use_Check = 1;
	float weight = edge->weight, mini = INF;

	igraph_vector_t cpy_Neighbors, cpy_Weights;
	igraph_vector_t tabu_List;

	typeEdge* aux;

    igraph_vector_init(&tabu_List, 1);

    VECTOR(tabu_List)[0] = parent;
//    printf("\n\n=========================================================\n\n");
//    printf("Parent %d Child %d\n", edge->parent, edge->child);
	while(execute*execute) //Will be always >= 0
	{
	    if(execute > 0)
		{

			igraph_vector_copy(&cpy_Neighbors, &graph[current_Node-1].neighbors);

			igraph_vector_copy(&cpy_Weights, &graph[current_Node-1].edge_Weight);
		}

		get_min(cpy_Neighbors, cpy_Weights, &index, &mini, prev_Node);

		chosen = (int)VECTOR(cpy_Neighbors)[index];
//		printf("CHOSEN ONE: %d\n", chosen);

        if(chosen==0)
        {
            execute = 0;
            *use_Check = 0;
        }if(igraph_vector_contains(used_Nodes, chosen)){execute = 0;}
		else if(igraph_vector_contains(&tabu_List, chosen))
		{

//            printf("POS: %d Index %d\n", find_position(&cpy_Neighbors, chosen), index);
			igraph_vector_remove(&cpy_Neighbors, index);

			igraph_vector_remove(&cpy_Weights, index);
			execute = -1;
		}else
		{

			igraph_vector_insert(&tabu_List, 0, chosen);
		    execute = 1;
        }

		if(execute >= 0)
		{
            if(*use_Check == 1)
            {
                weight += mini;

                aux = new_edge(chosen, current_Node, mini);

				//			printf("Aux %p Edge %p\n", aux, edge);
                aux->next = edge;
                edge->prev = aux;
                edge = aux;

                current_Node = edge->parent;
                prev_Node = edge->child;
//                printf("Parent %d Child %d\n", current_Node, prev_Node);
            }

			igraph_vector_destroy(&cpy_Neighbors);

			igraph_vector_destroy(&cpy_Weights);
		}

	}
    *cost_Path = weight;

    igraph_vector_destroy(&tabu_List);

    return edge;
}

sol* selects_best(igraph_vector_t nodes, igraph_vector_t weights, igraph_vector_t *used_Nodes, igraph_vector_t *m_Group, res* target_Node,
                  int *boolean_Best, sol* solution, typeNode* graph, int* size_Aux_U_Nodes, igraph_vector_t* aux_U_Nodes)
{
	int size_Vector = (int)igraph_vector_size(&nodes), index_Min = -1, index;
	int use_Check, check, chosen;

	float min_Weight  = INF, cost_Path = 0;
	res* new_Parent, *child;
	typeEdge* new_Path = NULL;
    *boolean_Best = 0;

    get_min(nodes,weights, &index_Min, &min_Weight, -1);

	if(min_Weight != target_Node->weight)
	{
		chosen = (int)VECTOR(nodes)[index_Min];
		check = igraph_vector_contains(used_Nodes, chosen);
//		printf("\n-------------------------------------------------------------------------------------\n");
//	    printf("\n===\nEscolhido %d, Está na árvore %d\n===\n", chosen, check);

		if(check)
		{
			new_Parent = find_node(solution->root, chosen);
            child = new_Parent->children;

            while(child!=NULL)
            {
                if(child->value==target_Node->parent->value){return solution;}
                child = child->brothers;
            }

            if( (new_Parent->cost + min_Weight) < target_Node->cost)
			{
				/**new_Parent = */apply_changes(target_Node, new_Path, new_Parent, used_Nodes, m_Group, graph, solution, min_Weight,
                                        size_Aux_U_Nodes, aux_U_Nodes);
				*boolean_Best = 1;
			}
		}else
		{
			new_Path = new_edge(chosen, target_Node->value, min_Weight);

			new_Path = get_path(used_Nodes, chosen, target_Node->value, graph, new_Path, &cost_Path, &use_Check);

            if(use_Check)
            {

				new_Parent = find_node(solution->root, new_Path->parent);

                if((cost_Path + new_Parent->cost) < target_Node->cost)
                {

				/**new_Parent = */apply_changes(target_Node, new_Path, new_Parent, used_Nodes, m_Group, graph, solution, min_Weight,
                                            size_Aux_U_Nodes, aux_U_Nodes);
                    *boolean_Best = 1;
                }else
                {
					dell_edges(new_Path);
                }
            }else
            {
				dell_edges(new_Path);
            }
		}
//		printf("\n-------------------------------------------------------------------------------------\n");
	}
//    printf("BB %d\n", *boolean_Best);
	return solution;
}

///OK
int find_position(igraph_vector_t* target, int value)
{
        int index, size_Target = (int)igraph_vector_size(target);

        for(index=0; index<size_Target; index++)
        {
            if((int)VECTOR(*target)[index] == value){return index;}
        }
        return -1;
}

#endif
