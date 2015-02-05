#ifndef structures_H_INCLUDED
#define structures_H_INCLUDED

/**======================================
	Author: Calvin Rodrigues da Costa
=======================================*/

#include <stdio.h>
#include <stdlib.h>
#include <igraph.h>

///Structure explained in the page 21 of the monograph
typedef struct node
{
    int value;
    int size_Neighborhood;

    igraph_vector_t neighbors;
    igraph_vector_t edge_Weight;
}typeNode;

typedef struct edge
{
	int parent;
	int child;
	float weight;

    struct edge* next;
    struct edge* prev;
}typeEdge;

typedef struct results
{
    struct results *parent;
	int value; //value of the current node
	float cost;
	float weight; //Weight of the edge between the previous and the current node

    struct results* prev_Bros;
	struct results* brothers; //BROS!
	struct results* children;
}res;

///Structure explained in the page 22 of the monograph
typedef struct the_solution
{
	res* root; //The root of tree
	double total_Weight; // Total weight of the tree
}sol;


/// Headers

res* find_node(res* solution, int node); /// Function to get a single node in the generic tree.

sol* new_sol(); /// Returns a new structure for the routing tree.

res* new_res(int node_Value, float weight); /// Returns a new member to the generic tree.

typeEdge* new_edge(int parent, int child, float weight); /// Returns a edge structure with your respective parent, child and weight

/// This function add a new node to the generic tree.
/// The parameter m_Nodes is a reference to the not used multicast nodes.
sol* add_node(sol* solution, typeEdge* chosen_One, igraph_vector_t* used_Nodes, igraph_vector_t* m_Nodes, int *size_M_Nodes);

sol* dell_tree(sol* solution, igraph_vector_t* used_Nodes); /// Deallocates the generic tree.

res* prune_tree(sol* solution, res* node_Root, igraph_vector_t* m_Group, igraph_vector_t* used_Nodes); ///Function explained with an example. Pages 27-35 of the monograph.

void change_cost(sol* solution, res* root, float* cost_Nodes); /// Update cost of nodes of the generic tree.

///==============================================================================================================================================================================================
/// Functions

res* find_node(res* root, int node_Value)
{
    //If the function don't find the node, will return the value NULL. Else, returns the reference  to the node.
    res* current_Node = root;
 	int execute;

    if(root==NULL){return NULL;}

 	if(current_Node->value != node_Value)
    {
        current_Node = current_Node->children;
        while(current_Node != root && current_Node != NULL)
        {
            if(current_Node->value == node_Value){break;}
            if(current_Node->children!=NULL){current_Node = current_Node->children;}
            else if(current_Node->brothers!=NULL){current_Node = current_Node->brothers;}
            else
            {
                execute = 1;
                while(execute)
                {
                    if(current_Node->parent != root && current_Node->parent->brothers != NULL)
                    {
                        current_Node = current_Node->parent->brothers;
                        execute = 0;
                    }else if(current_Node->parent == root)
                    {
                        current_Node = NULL;
                        execute = 0;
                    }else{current_Node = current_Node->parent;}
                }
            }
        }
    }

	return current_Node;
}

sol* new_sol()
{
	sol* solution = (sol*)malloc(sizeof(sol));
	solution->root = NULL;
	solution->total_Weight = 0;

	return solution;
}

res* new_res(int node_Value, float weight)
{
	res* new_Node = (res*)malloc(sizeof(res));

    new_Node->cost = 0;
	new_Node->value = node_Value;
	new_Node->weight = weight;

	new_Node->brothers = NULL;
	new_Node->children = NULL;
    new_Node->prev_Bros = NULL;

	return new_Node;
}

typeEdge* new_edge(int parent, int child, float weight)
{
    typeEdge* new_Edge;

    new_Edge = (typeEdge*)malloc(sizeof(typeEdge));
    new_Edge->parent = parent;
    new_Edge->weight = weight;
    new_Edge->child = child;

    new_Edge->next = NULL;
    new_Edge->prev = NULL;

    return new_Edge;
}

sol* add_node(sol* solution, typeEdge* chosen_One, igraph_vector_t* used_Nodes, igraph_vector_t* m_Nodes, int *size_M_Nodes)
{
    /// parent will be ever != NULL
    res* parent = find_node(solution->root, chosen_One->parent);

    int pos = -1;//variable to be used in the function find_position

    res* children = parent->children;
	res* new_Node = new_res(chosen_One->child, chosen_One->weight);

    new_Node->cost = parent->cost + new_Node->weight;
    new_Node->parent = parent;

    solution->total_Weight += new_Node->weight;

    if(children==NULL){parent->children = new_Node;}
    else //Will go to the most recent child of the parent node.
    {
        for(;children->brothers != NULL; children = children->brothers){}
        children->brothers = new_Node;
        new_Node->prev_Bros = children;
    }

    igraph_vector_insert(used_Nodes, 1, new_Node->value); /// First position will be ever the root.

    pos = find_position(m_Nodes, new_Node->value);

    if(pos>=0)
    {
       igraph_vector_remove(m_Nodes, pos);
       *size_M_Nodes -=1;
    }

    free(chosen_One);

    return solution;
}

sol* dell_tree(sol* solution, igraph_vector_t* used_Nodes)
{
    igraph_vector_t slayer;

    igraph_vector_init(&slayer, 1);
    VECTOR(slayer)[0] = solution->root->value;

    solution->root = prune_tree(solution, solution->root, &slayer, used_Nodes);

    free(solution->root);
    free(solution);
    igraph_vector_destroy(&slayer);

    return NULL;
}

res* prune_tree(sol* solution, res* node_Root, igraph_vector_t* m_Group, igraph_vector_t* used_Nodes)
{
    res* current_Node = node_Root->children, *aux = NULL;
    int bool_Check, execute, pos = -1;

//    if(node_Root->value == 32) printf("node: %d, check %d\n", node_Root->value, bool_Check);

    while(current_Node!=node_Root && current_Node!=NULL)
    {
        bool_Check = igraph_vector_contains(m_Group, current_Node->value); //Check if the current node are in the multicast group

        if(current_Node->children!=NULL){current_Node = current_Node->children;}
        else if(current_Node->brothers!=NULL)
        {
            if(bool_Check){current_Node = current_Node->brothers;}
            else
            {

               if(current_Node->prev_Bros!=NULL)
                {
                    current_Node->prev_Bros->brothers = current_Node->brothers;
                    current_Node->brothers->prev_Bros = current_Node->prev_Bros;
                }else
                {
                    current_Node->parent->children = current_Node->brothers;
                    current_Node->brothers->prev_Bros = NULL;
                }

                aux = current_Node;
                current_Node = aux->brothers;

                solution->total_Weight -= aux->weight;
                pos = find_position(used_Nodes, aux->value);

                igraph_vector_remove(used_Nodes, pos);

                free(aux);
                aux = NULL;
            }
        }else if(!bool_Check)
        {
            if(current_Node->prev_Bros == NULL)
            {
                current_Node->parent->children = NULL;
                aux = current_Node;
                current_Node = current_Node->parent;
            }else
            {
                current_Node->prev_Bros->brothers = NULL;
                aux = current_Node;
                execute = 1;

                while(execute)
                {
                    if(current_Node->parent != node_Root && current_Node->parent->brothers!=NULL)
                    {
                        current_Node = current_Node->parent->brothers;
                        execute = 0;
                    }else if(current_Node->parent == node_Root)
                    {
                        current_Node = current_Node->parent;
                        execute = 0;
                    }else{current_Node = current_Node->parent;}
                }
            }


            solution->total_Weight -= aux->weight;

            pos = find_position(used_Nodes, aux->value);

            igraph_vector_remove(used_Nodes, pos);
            free(aux);

            aux = NULL;
        }else
        {
            execute = 1;

            while(execute)
            {
                if(current_Node->parent!=node_Root && current_Node->parent->brothers!=NULL)
                {
                    current_Node = current_Node->parent->brothers;
                    execute = 0;
                }else if(current_Node->parent==node_Root)
                {
                    current_Node = current_Node->parent;
                    execute = 0;
                }else{current_Node = current_Node->parent;}
            }
        }
    }

    return node_Root;
}

void change_cost(sol* solution, res* root, float* cost_Nodes)
{
    int execute;
    res* current_Node = root->children;

    while(current_Node!=NULL && current_Node!=root)
    {
        current_Node->cost = current_Node->parent->cost + current_Node->weight;
        if(cost_Nodes!=NULL){cost_Nodes[current_Node->value-1] = current_Node->cost;}//Only happens when change_cost is applied by the prim_sg function

        if(current_Node->children!=NULL){current_Node = current_Node->children;}
        else if(current_Node->brothers!=NULL){current_Node = current_Node->brothers;}
        else
        {
            execute = 1;

            while(execute)
            {
                if(current_Node->parent!=root && current_Node->parent->brothers!=NULL)
                {
                    current_Node = current_Node->parent->brothers;
                    execute = 0;
                }else if(current_Node->parent==root)
                {
                    current_Node = current_Node->parent;
                    execute = 0;
                }else{current_Node = current_Node->parent;}
            }
        }
    }
}

#endif
