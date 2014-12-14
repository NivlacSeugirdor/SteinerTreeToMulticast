#ifndef structures_H_INCLUDED
#define structures_H_INCLUDED

/**======================================
	Author: Calvin Rodrigues da Costa
=======================================*/

#include <stdio.h>
#include <stdlib.h>
#include <igraph.h>
/**
	This library has functions for:
		- Create a new generic tree; |-> Done  |-> toCheck
		- Add nodes to the generic tree; |-> Done |-> toCheck
		- Find nodes in the generic tree; |-> Done |-> toCheck
		- To prune the tree;  |-> Done |-> toCheck
		- Deallocates the generic tree. |-> Done |-> toCheck
*/


typedef struct node
{
    int value;
    int size_Neighborhood;

    igraph_vector_t neighbors; // All adjacent nodes;
    igraph_vector_t edge_Weight; // Weight of the edge connecting the two nodes;
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

typedef struct the_solution
{
	res* root; //The root of tree --- http://sd.keepcalm-o-matic.co.uk/i/keep-calm-cause-i-m-groot.png
	float total_Weight; // Total weight of the tree
}sol;


/**
=======================================================================================================================
=======================================================================================================================
*/

	/** Function to get a single node in the generic tree */
	res* find_node(res* solution, int node);

	/** Returns a new structure for the routing tree. */
	sol* new_sol();

	/** Returns a new node. */
	res* new_res(int node_Value, float weight);

    /** Returns a new edge. */
    typeEdge* new_edge(int parent, int child, float weight);

	/**  The parameters are:
	         - The current tree;
	         - The edge;
		 - A integer random value;
		 - The used nodes;
		 - The not used multicast nodes.
	*/
	sol* add_node(sol* solution, typeEdge* chosen_One, igraph_vector_t* used_Nodes, igraph_vector_t* m_Nodes, int *size_M_Nodes);

	/** Deallocates the generic tree */
	sol* dell_tree(sol* solution, igraph_vector_t* used_Nodes);

    /** Deallocates the leaves that do not belong to the multicast group*/
    res* prune_tree(sol* solution, res* node_Root, igraph_vector_t* m_Group, igraph_vector_t* used_Nodes);

    /** Update cost of nodes */
    void change_cost(sol* solution, res* root, float* cost_Nodes);

    void get_edges(sol* solution, igraph_vector_t* n_In, igraph_vector_t* n_Out);
/**
=======================================================================================================================
=======================================================================================================================
*/


sol* new_sol() //O(1)
{
	sol* solution = (sol*)malloc(sizeof(sol));
	solution->root = NULL;
	solution->total_Weight = 0;

	return solution;
}

res* new_res(int node_Value, float weight) //O(1)
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

sol* add_node(sol* solution, typeEdge* chosen_One, igraph_vector_t* used_Nodes, igraph_vector_t* m_Nodes, int *size_M_Nodes) //O(n)
{
	/**
		Obligatorily a node must be found. This is because of that, if the edge is selected, one of its ends must
	be in the tree.
	*/
	int pos = -1;

    res* parent = find_node(solution->root, chosen_One->parent);
	res* children = parent->children;
	res* new_Node = new_res(chosen_One->child, chosen_One->weight);

    new_Node->cost = parent->cost + new_Node->weight; /** Add day 23/10/2014 */
    new_Node->parent = parent; /** Add day 23/10/2014 */

	solution->total_Weight += chosen_One->weight;

    if(children == NULL){parent->children = new_Node;}
	else
	{
		for(;children->brothers != NULL; children = children->brothers){} //O(n)
		children->brothers = new_Node;
		new_Node->prev_Bros = children; /** Add day 30/10/2014*/
	}

    //First position will be, ever, the root.
    igraph_vector_insert(used_Nodes, 1, new_Node->value); //O(?) ~ I guess O(n)
    pos = find_position(m_Nodes, new_Node->value);
	//printf("Pos : %d\n", pos);
   	if(pos!=-1)
    {
        igraph_vector_remove(m_Nodes, pos);/**O(n)*/
        *size_M_Nodes -=1;
    } //O(2n)
//    printf("Desaloca aresta\n");
    free(chosen_One);

	return solution;
}

/**
-I -> Go to children;
-III -> Go to brother;
-VIII -> Go up

PROCEDURE find_node
	Input Node, Target;

	IF Node == Target do
		return Node;
	END IF

	aux = Node->children;

	WHILE aux!=Node do
	    IF aux == Target Do
		return aux;
            END IF

	    IF aux->children != NULL DO
		I: . aux = aux->children;
	    ELSE IF aux->brothers != NULL DO
		III: . aux = aux->brothers;
	    ELSE
		VIII: . execute = 1;
		      . WHILE execute && aux!=Node DO
		            IF aux->parent->brothers != NULL && aux->parent != Node DO
		                aux = aux->parent->brothers;
		                execute = 0;
		            ELSE IF aux->parent == Node DO
		                aux = NULL;
		                execute = 0;
		            ELSE
		                aux = aux->parent;
		            END IF
		        END WHILE
	    END IF
	END WHILE
END PROCEDURE

*/

res* find_node(res* root, int node_Value)
{
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

sol* dell_tree(sol* solution, igraph_vector_t* used_Nodes)
{
    igraph_vector_t slayer;
    igraph_vector_init(&slayer, 1);
    VECTOR(slayer)[0] = solution->root->value;
    int i;

	solution->root = prune_tree(solution, solution->root, &slayer, used_Nodes);

	free(solution->root);
	free(solution);
    igraph_vector_destroy(&slayer);

	return NULL;
}

/**
I - Go to children;
III - Is leaf; Is not in MG; Has brother; Prune and go to brother;
VII - Is leaf; Is in MG; Has brother; Go to brother;
VIII - Is leaf; Is not in MG; Has not brother; Prune and go to parent;
IX - Has children yet; Has brother; Go to brother;
XII - Is leaf; Is in MG; Has not brother; Go to parent;
XIX - Has children yet; Has not brother; Go to parent;

PROCEDURE prune_tree
	Input: tree, node

	aux = node->children;

	WHILE aux!=node DO
		IF aux!=NULL
			IF aux->children != NULL DO
				I: . aux = aux->children;
			ELSE IF aux->brothers != NULL
				IF aux in M.G. DO
					VII: .aux = aux->brothers;
				ELSE IF aux not in M.G. DO
					IF aux->prev_Brothers != NULL DO
						III: .aux->prev_Brothers->brothers = aux->brothers;
						      aux->brothers->prev_Brothers = aux->prev_Brothers;
						      aux2 = aux->brothers;
				                      tree->total_Weight -= aux->weight;
						      delete(aux);
						      aux = aux2->brothers;
						      aux2 = NULL;
					ELSE
						III: .aux->parent->children = aux->brothers;
					   	      tree->total_Weight-= aux->weight;
						      aux2 = aux->parent->children;
						      delete(aux);
						      aux2->prev_Brothers = NULL;
						      aux = aux2;
						      aux2 = NULL;
					END IF
				END IF
			ELSE IF aux not in M.G. DO
				IF aux->prev_Brothers == NULL DO
				       VIII:  .aux->parent->children = NULL;
					       aux2 = aux->parent;
					       tree->total_Weight-= aux->weight;
					       delete(aux);
					       aux = aux2;
	                                       aux2 = NULL;
				ELSE
				       VIII:  .aux->prev_Brothers->brothers = NULL;
					       tree->total_Weight -= aux->weight;
					       aux2 = aux->parent;
					       delete(aux);
					       aux = aux2;
					       execute = 1;
                                               WHILE execute do
							IF aux->parent != node && aux->parent->brothers != NULL DO
								IX: .aux = aux->parent->brothers;
								     execute = 0;
							ELSE IF aux->parent == node DO
								execute = 0;
							ELSE
								XIX: .aux = aux->parent;
							END IF
					       END WHILE
					       aux2 = NULL;
				END IF
			ELSE IF aux in M.G. DO
				XII: 	.execute = 1;
					WHILE execute do
						IF aux->parent != node && aux->parent->brothers != NULL DO
							IX: .aux = aux->parent->brothers;
							     execute = 0;
						ELSE IF aux->parent == node DO
							execute = 0;
						ELSE
							XIX: .aux = aux->parent;
						END IF
					END WHILE
			END IF
		ELSE
			break;
		END IF
	END WHILE

END PROCEDURE
*/

res* prune_tree(sol* solution, res* node_Root, igraph_vector_t* m_Group, igraph_vector_t* used_Nodes)
{
    res* aux = node_Root->children, *second_Aux = NULL;
    int bool_Check, execute, pos;
//    printf("Root %d\n", node_Root->value);

    while(aux!=node_Root)
    {
//        printf("\nV %d : \n", aux->value);
        if(aux!=NULL)
        {
            bool_Check = igraph_vector_contains(m_Group ,aux->value);
            if(aux->children!=NULL)
            {
//                printf("1\n");
                aux = aux->children;
            }else if(aux->brothers!=NULL)
            {
//                printf("2\n");
                if(bool_Check)
                {
//                    printf("3\n");
                    aux = aux->brothers;
                }else
                {
//                    printf("4\n");
                    if(aux->prev_Bros!=NULL)
                    {
//                        printf("5\n");
                        aux->prev_Bros->brothers = aux->brothers;
                        aux->brothers->prev_Bros = aux->prev_Bros;
                        second_Aux = aux;
                        aux = aux->brothers;
                    }else
                    {
//                        printf("6\n");
                        aux->parent->children = aux->brothers;
                        aux->brothers->prev_Bros = NULL;
                        second_Aux = aux;
                        aux = aux->brothers;
                    }

                    solution->total_Weight -= second_Aux->weight;
                    pos = find_position(used_Nodes, second_Aux->value);
                    igraph_vector_remove(used_Nodes, pos);
                    free(second_Aux);
                    second_Aux = NULL;
                }
            }else if(!bool_Check)
            {
                if(aux->prev_Bros == NULL)
                {
//                    printf("7\n");
                    aux->parent->children = NULL;
                    second_Aux = aux;
                    aux = aux->parent;
                }else
                {
//                    printf("8\n");
                    aux->prev_Bros->brothers = NULL;
                    second_Aux = aux;
                    execute = 1;

                    while(execute)
                    {
/*                        if(aux->brothers!=NULL)
                        {
//                            printf("9\n");
                            aux = aux->brothers;
                        }else */if(aux->parent != node_Root && aux->parent->brothers != NULL)
                        {
//                            printf("10\n");
                            aux = aux->parent->brothers;
                            execute = 0;
                        }else if(aux->parent == node_Root)
                        {
//                            printf("11\n");
                            aux = aux->parent;
                            execute = 0;
                        }else
                        {
//                            printf("16\n");
                            aux = aux->parent;
                        }
                    }
                }

                solution->total_Weight -= second_Aux->weight;

                pos = find_position(used_Nodes, second_Aux->value);
                igraph_vector_remove(used_Nodes, pos);
                free(second_Aux);

                second_Aux = NULL;
            }else if(bool_Check)
            {
                execute = 1;

                while(execute)
                {
                 /*   if(aux->brothers!=NULL)
                    {
//                        printf("12\n");
                        aux = aux->brothers;
                    }else */if(aux->parent != node_Root && aux->parent->brothers != NULL)
                    {
//                        printf("13\n");
                        aux = aux->parent->brothers;
                        execute = 0;
                    }else if(aux->parent == node_Root)
                    {
//                        printf("14\n");
                        aux = aux->parent;
                        execute = 0;
                    }
                    else
                    {
//                        printf("15\n");
                        aux = aux->parent;
                    }
                }
            }
        }else{return NULL;}
    }
//    printf("Root %d\n", solution->root->value);
//    printf("===============================\n");
    return aux;
}


/**
    -I -> Update; Go to children;
    -III -> Update; Go to brother;
    -VIII -> Update; Go to up

    PROCEDURE change_costs
        Input Tree, Node;
        aux = Node->children;
        WHILE aux!=Node do
            IF aux->children != NULL DO
                I: . Tree->total_Weight -= aux->cost;
                     aux->cost = aux->parent->cost + aux->weight;
                     Tree->total_Weight += aux->cost;
                   . aux = aux->children;
            ELSE IF aux->brothers != NULL DO
                III: . Tree->total_Weight -= aux->cost;
                       aux->cost = aux->parent->cost + aux->weight;
                       Tree->total_Weight += aux->cost;
                     . aux = aux->brothers;
            ELSE
                VIII: . Tree->total_Weight -= aux->cost;
                        aux->cost = aux->parent->cost + aux->weight;
                        Tree->total_Weight += aux->cost;
                      . execute = 1;
                      . WHILE execute && aux!=Node do
                            IF aux->parent->brothers != NULL && aux->parent != Node DO
                                aux = aux->parent->brothers;
                                execute = 0;
                            ELSE IF aux->parent == Node DO
                                aux = aux->parent;
                                execute = 0;
                            ELSE
                                aux = aux->parent;
                            END IF
                        END WHILE
            END IF
        end WHILE
    end PROCEDURE
*/
void change_cost(sol* solution, res* root, float* cost_Nodes)
{
    int execute;
    res* aux = root->children;

    if(aux==NULL){return;}

    while(aux!=root)
    {
//        printf(".\n");
        if(aux->children != NULL)
        {
//            solution->total_Weight -= aux->cost;
            aux->cost = aux->parent->cost + aux->weight;
//            solution->total_Weight += aux->cost;
            if(cost_Nodes!=NULL){cost_Nodes[aux->value-1] = aux->cost;}
            aux = aux->children;
        }else if(aux->brothers!=NULL)
        {
//            solution->total_Weight -= aux->cost;
            aux->cost = aux->parent->cost + aux->weight;
//            solution->total_Weight += aux->cost;
            if(cost_Nodes!=NULL){cost_Nodes[aux->value-1] = aux->cost;}
            aux = aux->brothers;
        }else
        {
//            solution->total_Weight -= aux->cost;
            aux->cost = aux->parent->cost + aux->weight;
            if(cost_Nodes!=NULL){cost_Nodes[aux->value-1] = aux->cost;}
//            solution->total_Weight += aux->cost;
            execute = 1;
            while(execute && aux!=root)
            {
               /* if(aux->brothers!=NULL){aux = aux->brothers;}
                else */if(aux->parent->brothers!=NULL && aux->parent!=root)
                {
                    aux = aux->parent->brothers;
                    execute = 0;
                }else if(aux->parent==root)
                {
                    aux = aux->parent;
                    execute = 0;
                }else{aux = aux->parent;}
            }
        }
    }
}

void get_edges(sol* solution, igraph_vector_t* n_In, igraph_vector_t* n_Out)
{
    if(solution->root==NULL){return;}
    int execute = 1;
    res* aux = solution->root->children;

    while(aux!=solution->root)
    {
//        printf("P %d C %d : ",aux->parent->value, aux->value);
//        scanf("%*c");
        igraph_vector_insert(n_In, 0, aux->parent->value);
        igraph_vector_insert(n_Out, 0, aux->value);

        if(aux->children!=NULL){aux = aux->children;}
        else if(aux->brothers!=NULL){aux = aux->brothers;}
        else
        {
            execute = 1;
            while(execute && aux!=solution->root)
            {
                if(aux->parent->brothers!=NULL && aux->parent!=solution->root)
                {
                    aux = aux->parent->brothers;
                    execute = 0;
                }else if(aux->parent==solution->root)
                {
                    aux = aux->parent;
                    execute = 0;
                }else{aux = aux->parent;}
            }
        }
    }
}

#endif
