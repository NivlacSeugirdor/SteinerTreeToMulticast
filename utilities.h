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


///Headers


/** Main functions*/
typeNode* function_to_read(igraph_vector_t* m_Group, char graph_Name[], int* size_Graph); ///This function will get the information in the input file. With the information, returns the structure typeNode as the explained in the page 21 of monograph

void function_to_save(char graph_Name[], float time_in, float time_out, sol* solution, igraph_vector_t* used_Nodes, float alpha, int threads, int iter, float **info); ///Function to save the obtained results.

typeEdge* selection(float alpha, int *size_U_Nodes, typeNode* graph, igraph_vector_t* used_Nodes, float r_Number, float *cost_Nodes, int *bool_Change, igraph_vector_t* changed, int size_Changed, igraph_vector_t* m_Group); /// Function explained in the page 25 of monograph

sol* selects_best(igraph_vector_t nodes, igraph_vector_t weights, igraph_vector_t *used_Nodes, igraph_vector_t *m_Group, res* target_Node, int *boolean_Best, sol* solution, typeNode* graph
                               , int* size_Aux_U_Nodes, igraph_vector_t* aux_U_Nodes); /// Function used by the local_search function

res* apply_changes(res* target_Node, typeEdge* new_Path, res* new_Parent, igraph_vector_t *used_Nodes, igraph_vector_t *m_Group, typeNode* graph, sol* solution, float edge_Weight
                            , int* size_Aux_U_Nodes, igraph_vector_t* aux_U_Nodes, float *cost_Nodes); ///Function explained in the section 3.1.1 of the monograph.

/**Support functions*/

void insert_node(typeNode* graph, int in, int out, float weight); /// Function used by the "function_to_read".

typeEdge* get_edge(typeEdge* restrict_List, int size_List, float random_Number, int err); /// Used by the "selection" function. One edge is taken from a select group of edges.

typeEdge* increase_list(typeEdge* res_List, int q_Edges, int parent, int child, float weight); /// Used by the "selection" function. Will create/enhance a list of edges to be submitted to get_edge function.

void get_min(igraph_vector_t nodes, igraph_vector_t weights, int *index_Min, float *min_Weight, int root); /// Selects a node with minimum weight connected with the current node.  This node won't be the previous node.

typeEdge* get_path(igraph_vector_t* used_Nodes, int parent, int child, typeNode* graph, typeEdge* edge, float *cost_Path, int *use_Check); /// Selects a path to be used by the apply_changes function.

int find_position(igraph_vector_t* target, int value); ///Will get the index of a node in the vector "target". If the node is not in the vector, the function will return the value -1.

void get_edges(sol* solution, igraph_vector_t* n_In, igraph_vector_t* n_Out); ///Function to get the edges in the generic tree. They will be used in function_to_save.

res* prepare_to_dell(res* target_Node); /// Used by the apply_changes function.

typeEdge* dell_edges(typeEdge* res_List); /// Deallocates a list of edges

typeEdge* last_resort(typeEdge* rcl, igraph_vector_t *used_Nodes, igraph_vector_t* m_Group); /// This function will receive the restricted candidate list and will select the minimum weight edge with a node which is not in the tree.

void control_changes(int size_U_nodes, igraph_vector_t* used_Nodes, int *size_Change, igraph_vector_t* changes); /// This function will manipulate the list of changes.

///==============================================================================================================================================================================================
/// Functions

// Main
typeNode* function_to_read(igraph_vector_t* m_Group, char graph_Name[], int* size_Graph)
{
    FILE *arc;
    char archive[50];

    strcpy(archive, graph_Name);
    strcat(archive, ".stp\0");
    arc = fopen(archive, "r");

    if(arc==NULL)
    {
        printf("Problem with the archive %s!\n", graph_Name);
        return NULL;
    }

    char buffer[51];
    int stage, nodes, edges, index;

    typeNode* graph;
    int node_I, node_O;
    float weight=0;

    for(stage=1, nodes = 0, edges = 0; stage < 5; /** Nothing */)
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

                    for(index=0; index<nodes; index++)
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

                for(index=0; index<edges; index++)
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
                    int q_Members = 0, member = 0; //Quantity of members of the multicast group. The current member selected.

                    fscanf(arc, "%d", &q_Members);
                    igraph_vector_init(m_Group, 0);

                    for(index=0; index<q_Members; index++)
                    {
                        fscanf(arc, "%s %d", buffer, &member);
                        igraph_vector_insert(m_Group, index, member);
                    }

                    stage++;
                }
            break;
        }
    }

    fclose(arc);
    return graph;
}

void function_to_save(char graph_Name[], float time_in, float time_out, sol* solution, igraph_vector_t* used_Nodes, float alpha, int threads, int iter, float **info)
{
    FILE* new_Arc;
    int index;
    char buffer[30], buffer2[30], archive[80];

    igraph_vector_t n_In, n_Out;
    igraph_vector_init(&n_In, 0);
    igraph_vector_init(&n_Out, 0);

    strcpy(archive, graph_Name);
    strcat(archive, "_");
    sprintf(buffer, "%f", alpha);
    strcat(archive, buffer);
    strcat(archive, "_");
    sprintf(buffer2, "%d", threads);
    strcat(archive, buffer2);
    strcat(archive, "_result.txt\0");
    new_Arc = fopen(archive, "a");

    printf("Graph %s with %d threads and alpha equal %.2f - Cost = %.0f -- Time %f.\n", graph_Name, threads, alpha, solution->total_Weight, (time_out-time_in)/CLOCKS_PER_SEC);

    if(new_Arc==NULL)
    {
        printf("Problem with the archive %s!\n", archive);
        return;
    }

    fprintf(new_Arc, "SECTION Comment\n");

    fprintf(new_Arc, "Name ");

    fprintf(new_Arc, "%s", graph_Name);

    fprintf(new_Arc, "\nProblem STPG\n");

    fprintf(new_Arc, "Program GRASP_P\n");

    fprintf(new_Arc, "Version 1.0\n");

    fprintf(new_Arc, "End\n\n\n");

    fprintf(new_Arc, "SECTION Solutions\n");

    for(index=0; index<iter; index++){fprintf(new_Arc, "Solution %f %.0f\n", (info[index][0]-time_in)/CLOCKS_PER_SEC, info[index][1]);}

    fprintf(new_Arc, "End\n\n\n");

    fprintf(new_Arc, "SECTION Run\n");

    fprintf(new_Arc, "Threads %d\n", threads);

    fprintf(new_Arc, "Time %f\n", (time_out-time_in)/CLOCKS_PER_SEC);

    fprintf(new_Arc, "Primal %.0f\n", solution->total_Weight);

    fprintf(new_Arc, "End\n\n\n");

    fprintf(new_Arc, "SECTION Finalsolution\n");

    int sizes = (int)igraph_vector_size(used_Nodes);

    fprintf(new_Arc, "Vertices %d\n", sizes);

    igraph_vector_sort(used_Nodes);

    for(index=0; index<sizes; index++){fprintf(new_Arc, "V %d\n", (int)VECTOR(*used_Nodes)[index]);}

    get_edges(solution, &n_In, &n_Out);

    sizes = (int)igraph_vector_size(&n_In);

    fprintf(new_Arc, "Edges %d\n", sizes);

    for(index=0; index<sizes; index++){fprintf(new_Arc, "V %d %d\n", (int)VECTOR(n_In)[index], (int)VECTOR(n_Out)[index]);}

    fprintf(new_Arc, "End\n\n\n");

    igraph_vector_destroy(&n_In);
    igraph_vector_destroy(&n_Out);

    fclose(new_Arc);
}

typeEdge* selection(float alpha, int *size_U_Nodes, typeNode* graph, igraph_vector_t* used_Nodes, float r_Number, float *cost_Nodes, int *bool_Change, igraph_vector_t* changed, int size_Changed, igraph_vector_t* m_Group)
{
    int members_Index, neighbors_Index;
    int q_Edges;
    int parent, child, q_New, err = 1, pos;
    ///--------------------------------------------------------
    float weight, maximum = 0;
    float res = 0, minimum = INF;
    ///--------------------------------------------------------
    typeEdge* res_List = NULL, *chosen_One = NULL, *aux = NULL;
    ///--------------------------------------------------------
    igraph_bool_t check;
    igraph_vector_t exceptions;
    ///--------------------------------------------------------
    *bool_Change = 0;
    igraph_vector_init(&exceptions, 0); /// Will contain nodes belonging to the generic tree whose parent change enables a reduction in cost of the tree.

    for(members_Index=0, q_Edges = 0; members_Index< *size_U_Nodes; members_Index++)
    {
        parent = (int)VECTOR(*used_Nodes)[members_Index];

        for(neighbors_Index=0; neighbors_Index < graph[parent-1].size_Neighborhood; neighbors_Index++)
        {
            weight = (float)VECTOR(graph[parent-1].edge_Weight)[neighbors_Index];
            child = (int)VECTOR(graph[parent-1].neighbors)[neighbors_Index];
            check = igraph_vector_contains(used_Nodes, child);

            if((cost_Nodes[parent-1] + weight) <= cost_Nodes[child-1] || !check)
            {
                if(check){igraph_vector_insert(&exceptions, 0, child);}

                res_List = increase_list(res_List, q_Edges, parent, child, weight);

                q_Edges++;

                if(weight > maximum){maximum = weight;}
                if(weight < minimum){minimum = weight;}
            }
        }
    }

    res = minimum + alpha*(maximum-minimum);

    for(aux=res_List, q_New=0; aux!=NULL; aux=aux->next){if(aux->weight <= res){q_New++;}}

    if(size_Changed==0){chosen_One = get_edge(res_List, q_New, r_Number, err);}
    else
    {
        do
        {
            chosen_One = get_edge(res_List, q_New, r_Number, err);
            err++;
            pos = find_position(changed, chosen_One->child);

            if(pos!=-1) /// The node may experience just one direct reallocation in the construction phase
            {
                free(chosen_One);
                chosen_One = NULL;
            }
        }while(pos!=-1 && err <= q_New);
    }

    if(chosen_One!=NULL)
    {
        pos = find_position(&exceptions, chosen_One->child);

        if(pos!=-1){*bool_Change = 1;}

    }else{chosen_One = last_resort(res_List, used_Nodes, m_Group);}

    igraph_vector_destroy(&exceptions);
    dell_edges(res_List);

    return chosen_One;
}

sol* selects_best(igraph_vector_t nodes, igraph_vector_t weights, igraph_vector_t *used_Nodes, igraph_vector_t *m_Group, res* target_Node, int *boolean_Best, sol* solution, typeNode* graph, int* size_Aux_U_Nodes, igraph_vector_t* aux_U_Nodes)
{

    igraph_vector_t aux_Nodes;
    int size_Vector = (int)igraph_vector_size(&nodes), index_Min = -1, index;

    int use_Check, check, chosen;

    float min_Weight = INF, cost_Path = 0;
    res *new_Parent, *child;
    typeEdge *new_Path= NULL;
    *boolean_Best = 0; /// If an improvement is found, the value of this variable will be changed to 1.

    get_min(nodes, weights, &index_Min, &min_Weight, -1);
    chosen = (int)VECTOR(nodes)[index_Min];


    if(min_Weight<=target_Node->weight && chosen != target_Node->parent->value)
    {

        check = igraph_vector_contains(used_Nodes, chosen);

        if(check)
        {

            new_Parent = find_node(solution->root, chosen);

            if((new_Parent->cost + min_Weight) <= target_Node->cost)
            {

                child = new_Parent->children;

                while(child!=NULL)
                {
                    if(child->value==target_Node->parent->value){return solution;} /// If the chosen new parent is the old
                    child = child->brothers;
                }

                apply_changes(target_Node, new_Path, new_Parent, used_Nodes, m_Group, graph, solution, min_Weight, size_Aux_U_Nodes, aux_U_Nodes, NULL);
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
                    apply_changes(target_Node, new_Path, new_Parent, used_Nodes, m_Group, graph, solution, min_Weight, size_Aux_U_Nodes, aux_U_Nodes, NULL);
                    *boolean_Best = 1;
                }
            }

            dell_edges(new_Path);
        }
    }

    return solution;
}

res* apply_changes(res* target_Node, typeEdge* new_Path, res* new_Parent, igraph_vector_t *used_Nodes, igraph_vector_t *m_Group, typeNode* graph, sol* solution, float edge_Weight, int* size_Aux_U_Nodes, igraph_vector_t* aux_U_Nodes, float *cost_Nodes)
{
    res *old_Parent, *aux;
    int execute, pos = -1;


    typeEdge *run_The_Path;

    /// --------------- First step
    old_Parent = prepare_to_dell(target_Node);

    if(new_Path==NULL)
    {

        solution->total_Weight -= target_Node->weight;

        target_Node->parent = new_Parent;

        target_Node->cost = new_Parent->cost + edge_Weight;

        target_Node->weight = edge_Weight;
        solution->total_Weight += edge_Weight;

        aux = new_Parent->children;

        if(aux != NULL)
        {
            for(/**Nothing*/; aux->brothers!=NULL; aux = aux->brothers){/** Empty */}

            aux->brothers = target_Node;

            target_Node->prev_Bros = aux;

        }else{new_Parent->children = target_Node;}

    }else
    {
        run_The_Path = new_Path;
        aux = new_Parent;

        while(run_The_Path->child!=target_Node->value)
        {
            igraph_vector_insert(used_Nodes, 1, run_The_Path->child);

            if(aux->children==NULL)
            {
                aux->children = new_res(run_The_Path->child, run_The_Path->weight);
                aux->children->cost = aux->cost + aux->children->weight;
                aux->children->parent = aux;
                solution->total_Weight += aux->children->weight;

                aux = aux->children;
            }else
            {
                aux = aux->children;

                for(/**Nothing*/; aux->brothers!=NULL; aux = aux->brothers){/** Empty */}

                aux->brothers = new_res(run_The_Path->child, run_The_Path->weight);
                aux->brothers->cost = aux->parent->cost + aux->brothers->weight;
                aux->brothers->parent = aux->parent;

                solution->total_Weight += aux->brothers->weight;
                aux = aux->brothers;
            }

            run_The_Path = run_The_Path->next;
        }

        aux->children = target_Node;
        target_Node->parent = aux;

        solution->total_Weight-=target_Node->weight;
        target_Node->weight = edge_Weight;
        solution->total_Weight+=target_Node->weight;

        target_Node->cost = aux->cost + target_Node->weight;
    }


    /// --------------- Second step
    if(new_Path!=NULL){dell_edges(new_Path);}

    change_cost(solution, target_Node, cost_Nodes); /// Will update the costs of nodes descendants of target_node

    /// --------------- Third step
    execute = 1;

    while(execute)
    {
        if(old_Parent->children!=NULL || igraph_vector_contains(m_Group, old_Parent->value)){execute = 0;}
        else
        {

            /// Remove in the original vector
            pos = find_position(used_Nodes, old_Parent->value);

            igraph_vector_remove(used_Nodes, pos);

            /// Remove in the copy
            pos = find_position(aux_U_Nodes, old_Parent->value);

            if(pos>=0)
            {
                igraph_vector_remove(aux_U_Nodes, pos);
                *size_Aux_U_Nodes -= 1;
            }

            /// Update the references and the solution weight
            aux = prepare_to_dell(old_Parent);
            solution->total_Weight-=old_Parent->weight;

            if(cost_Nodes!=NULL){cost_Nodes[old_Parent->value-1] = INF;}

            /// Release the pointer (Release the Kraken!)
            free(old_Parent);
            old_Parent = aux;
        }
    }

    return new_Parent;
}


// Support
void insert_node(typeNode* graph, int node_I, int node_O, float weight)
{
    int index;

    for(index=0; index<graph[node_I-1].size_Neighborhood; index++)
    {
        if( VECTOR( graph[node_I-1].neighbors )[index] > node_O)
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

typeEdge* get_edge(typeEdge* restrict_List, int size_List, float random_Number, int err)
{
    typeEdge *chosen_One = NULL;
    typeEdge *rcl = restrict_List;
    unsigned int seed = (int)random_Number*(err);

    if(rcl!=NULL)
    {
        int value = (int)rand_r(&seed)%size_List;
        int index;

        for(index=0; index<value; index++, rcl = rcl->next){/** Empty */}
        chosen_One = new_edge(rcl->parent, rcl->child, rcl->weight);
    }

    return chosen_One;
}

typeEdge* increase_list(typeEdge* res_List, int q_Edges, int parent, int child, float weight)
{
    typeEdge *new_Edge, *rcl = res_List;
    new_Edge = new_edge(parent, child, weight);
    int execute = 1;


    if(q_Edges>0)
    {
        while(execute)
        {
            if(rcl->weight >= new_Edge->weight)
            {
                if(rcl->prev==NULL)
                {
                    rcl->prev = new_Edge;
                    new_Edge->next = rcl;
                    res_List = new_Edge;
                }else
                {
                    rcl->prev->next = new_Edge;
                    new_Edge->next = rcl;
                    new_Edge->prev = rcl->prev;
                    rcl->prev = new_Edge;
                }
                execute = 0;
            }else
            {
                if(rcl->next!=NULL){rcl = rcl->next;}
                else
                {
                    rcl->next = new_Edge;
                    new_Edge->prev = rcl;
                    execute = 0;
                }
            }
        }
    }else{res_List = new_Edge;}

    return res_List;
}

void get_min(igraph_vector_t nodes, igraph_vector_t weights, int *index_Min, float *min_Weight, int root)
{
    int size_Vector = (int)igraph_vector_size(&nodes);
    int index;

    *index_Min = 0;
    *min_Weight = INF;

    for(index=0; index<size_Vector; index++)
    {
        if(*min_Weight >= (float)VECTOR(weights)[index] && (int)VECTOR(nodes)[index] != root)
        {
            *min_Weight = (float)VECTOR(weights)[index];
            *index_Min = index;
        }
    }
}

typeEdge* get_path(igraph_vector_t* used_Nodes, int parent, int child, typeNode* graph, typeEdge* edge, float *cost_Path, int *use_Check)
{
    int execute = 1, index = -1, chosen, size_Neighbor;
    int current_Node = parent, prev_Node = child, pos = -1;
    float weight = edge->weight, mini;

    *use_Check = 1;

    igraph_vector_t cpy_Neighbors, cpy_Weights;
    igraph_vector_t tabu_List;

    typeEdge *aux;

    igraph_vector_init(&tabu_List, 1);
    VECTOR(tabu_List)[0] = parent;

    while(execute*execute)
    {
        if(execute>0)
        {
            igraph_vector_copy(&cpy_Neighbors, &graph[current_Node-1].neighbors);
            igraph_vector_copy(&cpy_Weights, &graph[current_Node-1].edge_Weight);
        }

        mini = INF;
        get_min(cpy_Neighbors, cpy_Weights, &index, &mini, prev_Node);

        chosen = (int)VECTOR(cpy_Neighbors)[index];

        size_Neighbor = (int)igraph_vector_size(&cpy_Neighbors);

        if(chosen==0 || size_Neighbor==0) /// To avoid errors.
        {
            execute=0;
            *use_Check=0;
        }else if(igraph_vector_contains(used_Nodes, chosen)){execute = 0;}
        else if(igraph_vector_contains(&tabu_List, chosen))
        {
            igraph_vector_remove(&cpy_Neighbors, index);
            igraph_vector_remove(&cpy_Weights, index);

            execute = -1;
        }else
        {
            igraph_vector_insert(&tabu_List, 0, chosen);
            execute = 1;
        }

        if(execute>=0)
        {
            if(*use_Check)
            {
                weight +=mini;

                aux = new_edge(chosen, current_Node, mini);
                aux->next = edge;
                edge->prev = aux;
                edge = aux;

                current_Node = edge->parent;
                prev_Node = edge->child;
            }

            igraph_vector_destroy(&cpy_Neighbors);
            igraph_vector_destroy(&cpy_Weights);
        }


    }

    *cost_Path = weight;
    igraph_vector_destroy(&tabu_List);

    return edge;
}

int find_position(igraph_vector_t* target, int value)
{
    int index, size_Target = (int)igraph_vector_size(target);

    for(index=0; index<size_Target; index++)
        if((int)VECTOR(*target)[index] == value){return index;}

    return -1;
}

void get_edges(sol* solution, igraph_vector_t* n_In, igraph_vector_t* n_Out)
{
    if(solution->root==NULL){return;}

    int execute;
    res* current_Node;

    current_Node = solution->root->children;

//    printf("\n==========================================\n Tree Structure\n==========================================\n");

    while(current_Node!=solution->root)
    {
//        printf("Parent %d Child %d Edge weight %.0f\n", current_Node->parent->value, current_Node->value, current_Node->weight);
        igraph_vector_insert(n_In, 0, current_Node->parent->value);
        igraph_vector_insert(n_Out, 0, current_Node->value);

        if(current_Node->children!=NULL)
        {

            current_Node = current_Node->children;
        }else if(current_Node->brothers!=NULL)
        {
            current_Node = current_Node->brothers;
        }else
        {
            execute = 1;
            while(execute)
            {
                if(current_Node->parent->brothers!=NULL)
                {
                    current_Node = current_Node->parent->brothers;
                    execute = 0;
                }else if(current_Node->parent==solution->root)
                {
                    current_Node = current_Node->parent;
                    execute = 0;
                }else{current_Node = current_Node->parent;}
            }
        }
    }

//    printf("\n==========================================\n\n\n\n");
}

res* prepare_to_dell(res* target_Node)
{
    res* old_Parent;
    old_Parent = target_Node->parent;

    if(target_Node->prev_Bros == NULL && target_Node->brothers != NULL)
    {
        old_Parent->children = target_Node->brothers;
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
    }else{old_Parent->children = NULL;}

    target_Node->parent = NULL;

    return old_Parent;
}

typeEdge* dell_edges(typeEdge* res_List)
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

typeEdge* last_resort(typeEdge* rcl, igraph_vector_t *used_Nodes, igraph_vector_t* m_Group)
{
    typeEdge* chosen_One = new_edge(-1, -1, INF);
    typeEdge* aux;
    int check, check_MG;

    for(aux = rcl; aux != NULL; aux = aux->next)
    {
        check = igraph_vector_contains(used_Nodes, aux->child);
        check_MG = igraph_vector_contains(m_Group, aux->child);

        if( !check && (check_MG || (aux->weight <= chosen_One->weight))
        {
            chosen_One->parent = aux->parent;
            chosen_One->child = aux->child;
            chosen_One->weight = aux->weight;
        }
        if(check_MG && !check){break;}
    }

    if(chosen_One->parent == -1)
    {
        free(chosen_One);
        chosen_One = NULL;
    }

    return chosen_One;
}

void control_changes(int size_U_nodes, igraph_vector_t* used_Nodes, int *size_Change, igraph_vector_t* changes)
{
    int index, check = -1, value;

    for(index = 0; index < *size_Change; /** Nothing */)
    {
        value = (int)VECTOR(*changes)[index];
        check = find_position(used_Nodes, value);

        if(check==-1)
        {
            igraph_vector_remove(changes, index);
            *size_Change = *size_Change-1;
        }else{index++;}
    }
}

#endif
