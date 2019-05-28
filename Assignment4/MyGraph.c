#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#define INITSIZE 100
#define RINFINITY 100000.0
#define COMPARE_INFINITY 80000.0

// A vertex is a 2D point
typedef struct Vertex {
	int x; // x-coordinate
	int y; // y-coordinate
} Vertex;

// each edge is a pair of vertices (end-points)
typedef struct Edge {
	Vertex *p1; // first end point
	Vertex *p2; // second end point
} Edge;

typedef struct NEdge {
	Vertex *p1;// first end point
	Vertex *p2;// second end point
	int status;// 0 for UNEXPLORED, 1 for EXPLORED
} NEdge;

typedef struct VertexNode {
	Vertex *v;
	double D;//designed to initialize the value of D to be large enough, for example, 100000
	struct HeapNode* heap_node;//pointer to Binomial node
	struct VertexLinkedNode* linked;//pointer to linked list
	int BFS_label_v1;//used in reachable vertices function, 0 means unvisited, 1 means visited
	int del_flag;//if 1, means that this node is deleted from the graph, default to be 0
	struct VertexNode* previous_node;//used to store the previous node in the shortest_path found
} VertexNode;

typedef struct VertexLinkedNode {
	struct VertexNode* vertex_node;//pointer to an array of VertexNode*
	struct NEdge* edge;
	double distance;
	struct VertexLinkedNode* next_linked_node;
} VertexLinkedNode;

typedef struct GraphRep *Graph;

typedef struct GraphRep { // graph header
	VertexNode **vertices; // an array of pointers to VertexNode
	int nV; // #vertices
	int nE; // #edges
	int nD;// number of deleted nodes, initialized to be zero, increase when delete isolate nodes
} GraphRep;
// A vertex node stores a vertex and other information, and you need to expand this type

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------Priority Queue ADT--------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
// data type for heap nodes
typedef struct HeapNode {
	// the key should be the D
	// the value should be  the unique ID of the node
	double key;
	int value;//the ID of the node
	struct HeapNode* first_child;//new added
	struct HeapNode* last_child;//new added
	struct HeapNode* parent;//new added attribute
	int seq_num;
	struct HeapNode* right_sibling;
	struct HeapNode* left_sibling;
} HeapNode;

//data type for a priority queue (heap) 
typedef struct BinomialHeap { //this is heap header
	int  size;      // count of items in the heap
	struct HeapNode* first_child;
	struct HeapNode* last_child;
} BinomialHeap;

// create a new heap node to store an item (task) 
HeapNode *newHeapNode(double k, int v)
{
	HeapNode *new;
	new = malloc(sizeof(HeapNode));
	assert(new != NULL);
	new->key = k;
	new->value = v;
	new->seq_num = 0;//new added
	new->first_child = NULL;//new added
	new->last_child = NULL;//new added
	new->parent = NULL;//new added attribute
	new->right_sibling = NULL;
	new->left_sibling = NULL;
	return new;
}

// create a new empty heap-based priority queue
BinomialHeap *newHeap()
{ // this function creates an empty binomial heap-based priority queue
	BinomialHeap *T;
	T = malloc(sizeof(BinomialHeap));
	assert(T != NULL);
	T->size = 0;
	T->first_child = NULL;
	T->last_child = NULL;
	return T;
}

//self-written function used to add a binomial tree to a binomial heap
void add_tree_to_heap(struct BinomialHeap* new_heap, struct HeapNode* temp_node)
{
	int initial_seq = temp_node->seq_num;
	if (!new_heap->size)//new heap is empty
	{
		new_heap->first_child = temp_node;
		new_heap->last_child = temp_node;
	}
	else//new heap is not empty
	{
		if (new_heap->last_child->seq_num < temp_node->seq_num)//no duplicate tree in new heap
		{
			new_heap->last_child->right_sibling = temp_node;
			temp_node->left_sibling = new_heap->last_child;
			new_heap->last_child = temp_node;
		}
		else//duplicate tree in new heap, need to merge these trees
		{
			if (temp_node->key < new_heap->last_child->key)//need to attach the last element to temp node
			{
				temp_node->right_sibling = NULL;
				if (new_heap->last_child->left_sibling)//last node is not the only node in the new heap
				{
					new_heap->last_child->left_sibling->right_sibling = temp_node;
					temp_node->left_sibling = new_heap->last_child->left_sibling;
				}
				else//last node is the only node in the new heap
				{
					temp_node->left_sibling = NULL;
					new_heap->first_child = temp_node;
				}
				if (temp_node->first_child)//temp node has at least one child
				{
					new_heap->last_child->left_sibling = temp_node->last_child;
					temp_node->last_child->right_sibling = new_heap->last_child;
					temp_node->last_child = new_heap->last_child;
					temp_node->seq_num++;
				}
				else//temp node does not have a child
				{
					new_heap->last_child->left_sibling = NULL;
					temp_node->first_child = new_heap->last_child;
					temp_node->last_child = new_heap->last_child;
					temp_node->seq_num++;
				}
				new_heap->last_child->right_sibling = NULL;
				new_heap->last_child->parent = temp_node;//new added attribute
				new_heap->last_child = temp_node;////////////////
			}
			else if (temp_node->key > new_heap->last_child->key)//need to attach temp node to the last element
			{
				temp_node->right_sibling = NULL;
				if (new_heap->last_child->first_child)//the last node has at least one child
				{
					temp_node->left_sibling = new_heap->last_child->last_child;
					new_heap->last_child->last_child->right_sibling = temp_node;
					new_heap->last_child->last_child = temp_node;//?????????????????????????
				}
				else//the last node has no child
				{
					temp_node->left_sibling = NULL;
					new_heap->last_child->first_child = temp_node;
					new_heap->last_child->last_child = temp_node;
				}
				new_heap->last_child->seq_num++;
				temp_node->parent = new_heap->last_child;//new added attribute
			}
			else
			{
				//two keys are the same in this condition, what to do then?
				//in this condition I just treat it with the same condition as (temp_node->key > new_heap->last_child->key)
				//that is, if two keys are the same, I will attach temp node to the last node
				temp_node->right_sibling = NULL;//modified to deal with same-key condition
				if (new_heap->last_child->first_child)//the last node has at least one child
				{
					temp_node->left_sibling = new_heap->last_child->last_child;
					new_heap->last_child->last_child->right_sibling = temp_node;
					new_heap->last_child->last_child = temp_node;//?????????????????????????
				}
				else//the last node has no child
				{
					temp_node->left_sibling = NULL;
					new_heap->last_child->first_child = temp_node;
					new_heap->last_child->last_child = temp_node;
				}
				new_heap->last_child->seq_num++;
				temp_node->parent = new_heap->last_child;//new added attribute
			}
		}
	}
	//modified to solve the problem of size error
	new_heap->size += (int)pow(2, initial_seq);//in all these situations above, modify the size of the new heap
	//printf("%d ", temp_node->seq_num);//for debug use only
	//printf("%f\n", pow(2, temp_node->seq_num));//for debug use only
}

//self-written function used to add the first tree to the new heap
void add_first_tree_to_heap(struct BinomialHeap* new_heap, struct BinomialHeap* heap)
{
	//modify the heap1 info
	struct HeapNode* temp_node;
	temp_node = heap->first_child;
	if (temp_node->right_sibling)//temp_node is not the only node in heap
	{
		temp_node->right_sibling->left_sibling = NULL;
		heap->first_child = temp_node->right_sibling;
	}
	else//temp_node is the only node in heap
	{
		heap->first_child = NULL;
		heap->last_child = NULL;
	}
	temp_node->right_sibling = NULL;
	//printf("%d\n", temp_node->seq_num);
	heap->size -= (int)pow(2, temp_node->seq_num);
	//modify the new heap info
	add_tree_to_heap(new_heap, temp_node);
}

//self-written function used to add the combination of two first nodes into new heap
void add_two_first_nodes_to_heap(struct BinomialHeap* new_heap, struct BinomialHeap* heap1, struct BinomialHeap* heap2)
{
	//attach the first node of heap2 to the first node of heap1
	struct HeapNode* temp_node;
	temp_node = heap1->first_child;
	if (temp_node->right_sibling)//temp_node is not the only node in the heap1
	{
		temp_node->right_sibling->left_sibling = NULL;
		heap1->first_child = temp_node->right_sibling;
	}
	else//temp_node is the only node in the heap1
	{
		heap1->first_child = NULL;
		heap1->last_child = NULL;
	}
	heap1->size -= (int)pow(2, temp_node->seq_num);
	//printf("%d\n", temp_node->seq_num);
	struct HeapNode* sub_node = heap2->first_child;
	if (sub_node->right_sibling)//sub_node is not the only node in heap2
	{
		sub_node->right_sibling->left_sibling = NULL;
		heap2->first_child = sub_node->right_sibling;
	}
	else//sub_node is the only node in heap2
	{
		heap2->first_child = NULL;
		heap2->last_child = NULL;
	}
	temp_node->right_sibling = NULL;
	if (temp_node->first_child)//temp node has at least one child
	{
		temp_node->last_child->right_sibling = sub_node;
		sub_node->left_sibling = temp_node->last_child;
		temp_node->last_child = sub_node;
	}
	else//temp node has no child
	{
		sub_node->left_sibling = NULL;
		temp_node->first_child = sub_node;
		temp_node->last_child = sub_node;
	}
	sub_node->right_sibling = NULL;
	temp_node->seq_num++;
	sub_node->parent = temp_node;//new added attribute
	heap2->size -= (int)pow(2, sub_node->seq_num);
	//printf("%d\n", temp_node->seq_num);
	add_tree_to_heap(new_heap, temp_node);
}

//self-written function used to merge two heaps
struct BinomialHeap* merge_heaps(struct BinomialHeap* heap1, struct BinomialHeap* heap2)
{
	// //if at least one heap is empty, return another heap
	// if (!heap1->size)
	// {
	// 	return heap2;
	// }
	// if (!heap2->size)
	// {
	// 	return heap1;
	// }
	//both two heaps are not empty
	struct BinomialHeap* new_heap = newHeap();
	//struct HeapNode* temp_node;//did not use this???
	/*int left_seq;//did not use this???
	int right_seq;//did not use this???*/
	while (heap1->size && heap2->size)
	{
		if (heap1->first_child->seq_num < heap2->first_child->seq_num)//add the first child of heap1 to the new heap
		{
			add_first_tree_to_heap(new_heap, heap1);
		}
		else if (heap1->first_child->seq_num > heap2->first_child->seq_num)//add the first child of heap2 to the new heap
		{
			add_first_tree_to_heap(new_heap, heap2);
		}
		else//need to merge the two first nodes of two heaps (in this way i think there is no duplicate in the new heap)
		{
			//first, merge two first nodes into a node called temp_node
			if (heap1->first_child->key < heap2->first_child->key)
				add_two_first_nodes_to_heap(new_heap, heap1, heap2);
			else if (heap1->first_child->key > heap2->first_child->key)
				add_two_first_nodes_to_heap(new_heap, heap2, heap1);
			else
			{
				//In this case two keys are the same, what to do then?
				//in this condition, I just treat it as the same way as (heap1->first_child->key < heap2->first_child->key)
				add_two_first_nodes_to_heap(new_heap, heap1, heap2);//modified to deal with same-key condition
			}
		}
	}
	//then, if one is not empty, for each element in that heap, add to the new heap
	if (heap1->size)//the remaining heap is heap1
	{
		while (heap1->size)
		{
			add_first_tree_to_heap(new_heap, heap1);
		}
	}
	else if (heap2->size)//the remaining heap is heap2
	{
		while (heap2->size)
		{
			add_first_tree_to_heap(new_heap, heap2);
		}
	}
	return new_heap;
}


// put the time complexity analysis for Insert() here
/*
<---Assume there are n elements in the previous heap--->

The simplified structure of this function is:

Function:
	create a heap, containing only one node, with the key k  (Time: constant)
	merge these two heaps

The merge operation operates every tree of two heaps only once, and each operation time takes constant time.
The sizes of two heaps to merge here is n and 1, respectively.
So the time complexity of the merge operation in this case in O(log(n+1)) = O(logn), in which n is the size of the first heap.
So the time complexity of this function is O(logn).
*/
HeapNode* Insert(struct BinomialHeap** T, double k, int v)
{
	struct BinomialHeap* temp_heap = newHeap();
	struct HeapNode* new_node = newHeapNode(k, v);
	new_node->seq_num = 0;
	temp_heap->size++;
	temp_heap->first_child = new_node;
	temp_heap->last_child = new_node;
	*T = merge_heaps(*T, temp_heap);
	return new_node;
}

void print_tree(struct HeapNode* current_tree)
{
	struct HeapNode* temp = current_tree->first_child;
	printf("(%f, %d) ", current_tree->key, current_tree->value);
	while (temp)
	{
		print_tree(temp);
		temp = temp->right_sibling;
	}
}

void print_heap(struct BinomialHeap* T)
{
	if (!T->size)
	{
		printf("The current heap is empty.\n");
		return;
	}
	struct HeapNode* current_node = T->first_child;
	while (current_node)
	{
		printf("Tree:\n");
		print_tree(current_node);
		printf("\n");
		current_node = current_node->right_sibling;
	}
}

// put your time complexity for RemoveMin() here
/*
<---Assume there are n nodes in the heap--->

The simplified structure of this function is:

Function:
	Find the tree with the smallest root key in the heap (Time: O(logn) )
	Create a new heap and move this tree (without the root) to the new heap (Time: constant)
	Merge two heaps (Time: O(logn) )

Because in the implementation of the Binomial Node, I use doubly linked list, that is, a right_sibling pointer to the next sibling,
and a left-sibling pointer to the previous sibing, and I also use first_child and last_child pointers to refer to the first child node
and last child node, respectively. So the removal of the root of a Binomial Tree takes constant time.

So the time complexity of this function is: O(logn), in which n is the size of the Binomial Heap.
*/
HeapNode *RemoveMin(BinomialHeap **T)
{
	// put your code here
	// first get the tree with the minimum key
	if (!((*T)->size))//if the heap is empty
	{
		printf("Cannot remove element from an empty heap.\n");
		exit(1);
	}
	struct HeapNode* current_node = (*T)->first_child;
	double current_smallest_key = current_node->key;
	struct HeapNode* current_smallest_node = current_node;
	while (current_node)
	{
		if (current_node->key < current_smallest_key)
		{
			current_smallest_key = current_node->key;
			current_smallest_node = current_node;
		}
		current_node = current_node->right_sibling;
	}
	// now, the minimum key is current_smallest_key, and the minimum node is current_smallest_node
	// then, get the tree out of the heap
	if (!(current_smallest_node->right_sibling) && !(current_smallest_node->left_sibling))//current_smallest_node is the only tree in the heap
	{
		if (!(current_smallest_node->first_child))//current_smallest_node does not have a child
		{
			(*T)->size -= 1;
			(*T)->first_child = NULL;
			(*T)->last_child = NULL;
			current_smallest_node->parent = NULL;//new added attribute
			return current_smallest_node;
		}
		else//current_smallest node has at least one child
		{
			(*T)->size -= 1;
			(*T)->first_child = current_smallest_node->first_child;
			(*T)->last_child = current_smallest_node->last_child;
			//update the parent
			HeapNode* current = (*T)->first_child;//new added attribute
			while (current)//new added attribute
			{
				current->parent = NULL;//new added attribute
				current = current->right_sibling;//new added attribute
			}
			current_smallest_node->seq_num = 0;
			current_smallest_node->first_child = NULL;
			current_smallest_node->last_child = NULL;
			current_smallest_node->parent = NULL;//new added attribute
			return current_smallest_node;
		}
	}
	else//current_smallest_node is not the only tree in the heap
	{
		(*T)->size -= (int)pow(2, current_smallest_node->seq_num);
		if (current_smallest_node->left_sibling)//current_smallest_node is not the first node
		{
			current_smallest_node->left_sibling->right_sibling = current_smallest_node->right_sibling;
		}
		else//current_smallest_node is the first node
		{
			(*T)->first_child = current_smallest_node->right_sibling;
		}
		if (current_smallest_node->right_sibling)//current_smallest_node is not the last node
		{
			current_smallest_node->right_sibling->left_sibling = current_smallest_node->left_sibling;
		}
		else//current_smallest_node is the last node
		{
			(*T)->last_child = current_smallest_node->left_sibling;
		}
		current_smallest_node->left_sibling = NULL;
		current_smallest_node->right_sibling = NULL;
	}
	// make it a new heap and delete the root
	struct BinomialHeap* new_heap = newHeap();
	new_heap->size = (int)pow(2, current_smallest_node->seq_num);
	new_heap->size -= 1;//because the node current_smallest_node need to be removed
	if (current_smallest_node->first_child)//current_smallest_node has at least one child
	{
		new_heap->first_child = current_smallest_node->first_child;
		new_heap->last_child = current_smallest_node->last_child;
		//update the parent
		HeapNode* current = new_heap->first_child;//new added attribute
		while (current)//new added attribute
		{
			current->parent = NULL;//new added attribute
			current = current->right_sibling;//new added attribute
		}
		current_smallest_node->first_child = NULL;
		current_smallest_node->last_child = NULL;
		current_smallest_node->parent = NULL;//new added attribute
		current_smallest_node->seq_num = 0;
	}
	else//current_smallest_node has no child
	{
		return current_smallest_node;
	}
	// merge two heaps
	*T = merge_heaps(*T, new_heap);
	return current_smallest_node;
}


/*
In the root of each Binomial Tree (it is still a node but uses other properties) I use sibiling to refer to the next root of the same heap.
So in order to find the minimum key, only need to traverse the root of each Binomial Tree in the heap.
Because the number of Binomial Tree in a Binomial Heap is O(logn), where n is the size of the heap.
So need to traverse logn root nodes to get the minimum key.
And because the operation on each root node takes constant time.
So the time complexity of this function is O(logn), in which n is the size of the heap.
*/
double Min(struct BinomialHeap *T)//this version returns the min key
{
	// put your code here
	if (!T->size)//the heap is empty
	{
		printf("The heap is empty, cannot return the min from it.\n");
		exit(1);
	}
	double current_smallest = T->first_child->key;
	struct HeapNode* current_node = T->first_child;
	while (current_node)
	{
		if (current_node->key < current_smallest)
		{
			current_smallest = current_node->key;
		}
		current_node = current_node->right_sibling;
	}
	return current_smallest;
}

/*
In the root of each Binomial Tree (it is still a node but uses other properties) I use sibiling to refer to the next root of the same heap.
So in order to find the node with the minimum key, only need to traverse the root of each Binomial Tree in the heap.
Because the number of Binomial Tree in a Binomial Heap is O(logn), where n is the size of the heap.
So need to traverse logn root nodes to get the node with the minimum key.
And because the operation on each root node takes constant time.
So the time complexity of this function is O(logn), in which n is the size of the heap.
*/
struct HeapNode* MinNode(struct BinomialHeap* current_heap)//this version returns the node with the smallest key
{
	if (!current_heap->size)
	{
		printf("The heap is empty, cannot find the min node from it.\n");
		exit(1);
	}
	struct HeapNode* current_min_node = current_heap->first_child;
	struct HeapNode* current_node = current_heap->first_child;
	while (current_node)
	{
		if (current_node->key < current_min_node->key)
		{
			current_min_node = current_node;
		}
		current_node = current_node->right_sibling;
	}
	return current_min_node;
}
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------Priority Queue ADT--------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------Queue ADT-----------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

// the value stored in queue is the pointer to VertexNode

typedef struct queue
{
	int size;//the size of the allocated space
	int base;//the index (just call it index) of the last element in the allocated space
	int num;//the number of elements in the allocated space
	VertexNode **data;//the value stored in queue should be pointer to VertexNode
} queue;

struct queue* initQueue()
{
	struct queue* x = (struct queue*)malloc(sizeof(struct queue));
	x->size = INITSIZE;
	x->base = 0;
	x->num = 0;
	x->data = (VertexNode**)malloc(INITSIZE * sizeof(VertexNode*));
	return x;
}

// time complexity: O(1)
int isQueueEmpty(const struct queue* current_queue)
{
	return (current_queue->num == 0);//return 1 if empty, 0 otherwise
}

// time complexity: O(1)
char* isQueueEmptyVi(const struct queue* current_queue)
{
	return (current_queue->num == 0) ? "This queue is empty." : "This queue is not empty";
}

// time complexity: O(1)
int lenQueue(const struct queue* current_queue)
{
	return current_queue->num;
}

// time complexity: O(1)
void enqueue(struct queue* current_queue, VertexNode* vn)
{
	*(current_queue->data + (current_queue->base + current_queue->num) % INITSIZE) = vn;
	current_queue->num++;
	return;
}

// time complexity: O(1)
VertexNode* dequeue(struct queue* current_queue)
{
	assert(!isQueueEmpty(current_queue));
	VertexNode* temp = *(current_queue->data + current_queue->base);
	current_queue->base = (current_queue->base + 1) % current_queue->size;
	current_queue->num--;
	return temp;
}

// time complexity: O(1)
VertexNode* peek(const struct queue* current_queue)
{
	assert(!isQueueEmpty(current_queue));
	return *(current_queue->data + current_queue->base);
}

// time complexity: O(1)
void clearQueue(struct queue* current_queue)
{
	free(current_queue->data);
	free(current_queue);
	return;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------Queue ADT-----------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------Stack ADT------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

struct stack {
	int n;
	VertexNode** data;
};

struct stack* emptyStack()
{
	struct stack* new_stack = (struct stack*)malloc(sizeof(struct stack));
	new_stack->n = 0;
	new_stack->data = (VertexNode**)malloc(100 * sizeof(VertexNode*));
	return new_stack;
}

// time complexity: O(1)
void push(struct stack* current_stack, VertexNode* new_vertex_node)
{
	if (current_stack->n >= 100)
	{
		// normally, this should not happen
		printf("The stack is full.\n");
		return;
	}
	*(current_stack->data + current_stack->n) = new_vertex_node;
	(current_stack->n)++;
}

// time complexity: O(1)
VertexNode* pop(struct stack* current_stack)
{
	if (!current_stack->n)
	{
		printf("The stack is empty.\n");
		return NULL;
	}
	(current_stack->n)--;
	return *(current_stack->data + current_stack->n);
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------Stack ADT------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

//global variables here
BinomialHeap* priority_queue;

// self-written function used to check whether two vertexes are same or not
int is_vertex_same(Vertex* v1, Vertex* v2)
{
	if (v1->x == v2->x && v1->y == v2->y)
		return 1;
	else
		return 0;
}

// self-written function used to calculate the distance between two vertexes
double calculate_distance(Vertex* v1, Vertex* v2)
{
	double x_diff = v1->x - v2->x;
	double y_diff = v1->y - v2->y;
	double temp = x_diff * x_diff + y_diff * y_diff;
	return sqrt(temp);
}

// self-written function used to calculate pow-3
int x_3(int x)
{
	return x * x * x;
}

// self-written function used to calculate pow-4
int x_4(int x)
{
	return x_3(x) * x;
}

// self-written function used to generate ID
int ID_pair_calculation(Vertex* v1, Vertex* v2)
{
	int result = x_3(v1->x) + x_4(v1->y) + x_3(v2->x) + x_4(v2->y) + (v1->x - v1->y) * (v1->x - v1->y) + (v1->x + v1->y) * (v1->x + v1->y) + (v2->x - v2->y) * (v2->x - v2->y) + (v2->x + v2->y) * (v2->x + v2->y);
	return result;
}

// Add the time complexity analysis of CreateEmptyGraph() here
/*
The time complexity of this function is O(1), because the allocation of space and all other operations take constant time.
*/
Graph CreateEmptyGraph()
{
	Graph new_graph = (Graph)malloc(sizeof(struct GraphRep));
	new_graph->nE = 0;
	new_graph->nV = 0;
	new_graph->nD = 0;
	new_graph->vertices = (struct VertexNode**) malloc(100 * sizeof(struct VertexNode*));//pre-allocate 100 cells
	priority_queue = newHeap();
	return new_graph;
}

// Add the time complexity analysis of InsertEdge() here
/*
<---Assume the number of vertexes is n and the number of edges is m.--->
Because the "hole" exists in the array, that is, after deleting all edges connected with a certain vertex, the vertex should be removed from the array, thus creating a
so-called "hole" that does not contain any vertex, I use N to represent the number of vertexes and the "hole".

Because the insertion function here need to check for duplication, that is, the function need to check whether the vertex already exists in the graph or not, and whether the
edge already exists or not, which means that it is not possible to just append linked node of vertex or edge of the linked list, the function need to traverse the linked list (both the
vertices linked list and the edge linked list stored in each vertex).

In the worst case, the function will take O(N) + O(degree of p1) + O(degree of p2), in which p1 and p2 are two vertexes of the new-added edge.
From the professor, because O(N) is underestimate and O(N+m) is overestimate, the precise time complexity is O(N+max{degree of p1 and p2}).
*/
int InsertEdge(Graph g, Edge *e)
{
	int return_value = 0;
	Vertex* p1 = e->p1;//the first vertex of the edge
	int flag1 = 0;//if the first vertex is found in the array, set it to 1
	Vertex* p2 = e->p2;//the second vertex of the edge
	int flag2 = 0;//if the second vertex is found in the array, set it to 1
	double distance = calculate_distance(e->p1, e->p2);//the distance between p1 and p2
	VertexNode* current_vertex_node;
	Vertex* current_vertex;
	VertexLinkedNode* current_linked_node;
	VertexLinkedNode* last_linked_node;
	VertexLinkedNode* temp1 = NULL;//waiting for store p2 node here, new added
	VertexLinkedNode* temp2 = NULL;//waiting for store p1 node here, new added
	VertexNode* new1;//save the node with p1
	VertexNode* new2;//save the node with p2
	NEdge* new_edge = (NEdge*)malloc(sizeof(NEdge));//new edge
	new_edge->status = 0;
	if ((p1->x < p2->x) || (p1->x == p2->x && p1->y < p2->y))
	{
		new_edge->p1 = p1;
		new_edge->p2 = p2;
	}
	else
	{
		new_edge->p1 = p2;
		new_edge->p2 = p1;
	}
	int linked_flag;//linked_flag is 1 if find the same one
	for (int i = 0; i < (g->nV + g->nD); i++)//modified the g->nV to g->nV+g->nD
	{
		//need to check whether the current vertex node is deleted or not
		current_vertex_node = *(g->vertices + i);
		if (current_vertex_node->del_flag)//the current vertex node is deleted
			continue;
		current_vertex = current_vertex_node->v;
		if (is_vertex_same(current_vertex, p1))//current is the same as p1
		{
			new1 = current_vertex_node;
			flag1 = 1;
			// append to linked list
			current_linked_node = current_vertex_node->linked;
			linked_flag = 0;
			if (!current_linked_node)//linked list is empty
			{
				//create new linked node, containing p2
				VertexLinkedNode* new_linked_node = (VertexLinkedNode*)malloc(sizeof(VertexLinkedNode));
				temp1 = new_linked_node;
				new_linked_node->edge = new_edge;
				new_linked_node->distance = distance;
				new_linked_node->next_linked_node = NULL;
				current_vertex_node->linked = new_linked_node;
				g->nE++;
				return_value = 1;
			}
			else//linked list is not empty
			{
				last_linked_node = NULL;
				while (current_linked_node)
				{
					if (is_vertex_same(current_linked_node->vertex_node->v, p2))//p2 already exists in the linked list
					{
						linked_flag = 1;
						break;
					}
					last_linked_node = current_linked_node;
					current_linked_node = current_linked_node->next_linked_node;
				}
				if (linked_flag)//the p2 already exists in the linked list
					return 0;
				else//the p2 does not exist in the linked list
				{
					//create new linked node, containing p2
					VertexLinkedNode* new_linked_node = (VertexLinkedNode*)malloc(sizeof(VertexLinkedNode));
					new_linked_node->edge = new_edge;
					temp1 = new_linked_node;
					new_linked_node->distance = distance;
					new_linked_node->next_linked_node = NULL;
					g->nE++;
					return_value = 1;
					//here is safe to assume that last_linked_node is not NULL
					//but i will check again
					if (last_linked_node)
						last_linked_node->next_linked_node = new_linked_node;
					else
						printf("70921 last_linked_node is NULL.\n");
				}
			}
		}
		else if (is_vertex_same(current_vertex, p2))//current is the same as p2
		{
			new2 = current_vertex_node;
			flag2 = 1;
			// append to linked list
			current_linked_node = current_vertex_node->linked;
			linked_flag = 0;
			if (!current_linked_node)//linked list is empty
			{
				//create new linked node, containing p1
				VertexLinkedNode* new_linked_node = (VertexLinkedNode*)malloc(sizeof(VertexLinkedNode));
				new_linked_node->edge = new_edge;
				temp2 = new_linked_node;
				new_linked_node->distance = distance;
				new_linked_node->next_linked_node = NULL;
				current_vertex_node->linked = new_linked_node;
				return_value = 1;
			}
			else//linked list is not empty
			{
				last_linked_node = NULL;
				while (current_linked_node)
				{
					if (is_vertex_same(current_linked_node->vertex_node->v, p1))//p1 already exists in the linked list
					{
						linked_flag = 1;
						break;
					}
					last_linked_node = current_linked_node;
					current_linked_node = current_linked_node->next_linked_node;
				}
				if (linked_flag)//the p1 already exists in the linked list
					return 0;
				else//the p1 does not exist in the linked list
				{
					//create new linked node, containing p1
					VertexLinkedNode* new_linked_node = (VertexLinkedNode*)malloc(sizeof(VertexLinkedNode));
					new_linked_node->edge = new_edge;
					temp2 = new_linked_node;
					new_linked_node->distance = distance;
					new_linked_node->next_linked_node = NULL;
					return_value = 1;
					//here is safe to assume that last_linked_node is not NULL
					//but i will check again
					if (last_linked_node)
						last_linked_node->next_linked_node = new_linked_node;
					else
						printf("70922 last_linked_node is NULL.\n");
				}
			}
		}
		if (flag1 && flag2)
			break;
	}
	if (!flag1)
	{
		//p1 does not exist
		//create new cell with the unique ID of current nV
		VertexNode* new_vertex_node = (VertexNode*)malloc(sizeof(VertexNode));
		new_vertex_node->D = RINFINITY;//large enough
		new_vertex_node->v = p1;
		new_vertex_node->BFS_label_v1 = 0;
		new_vertex_node->del_flag = 0;
		new_vertex_node->previous_node = NULL;
		new_vertex_node->heap_node = Insert(&priority_queue, new_vertex_node->D, g->nV);
		VertexLinkedNode* new_linked_node = (VertexLinkedNode*)malloc(sizeof(VertexLinkedNode));
		temp1 = new_linked_node;
		new_linked_node->distance = distance;
		new_linked_node->edge = new_edge;
		new_linked_node->next_linked_node = NULL;
		new_vertex_node->linked = new_linked_node;
		*(g->vertices + g->nV) = new_vertex_node;
		g->nV++;
		g->nE++;
		return_value = 1;
		new1 = new_vertex_node;
	}
	if (!flag2)
	{
		//p2 does not exist
		//create new cell with the unique ID of current nV
		VertexNode* new_vertex_node = (VertexNode*)malloc(sizeof(VertexNode));
		new_vertex_node->D = RINFINITY;//large enough
		new_vertex_node->v = p2;
		new_vertex_node->BFS_label_v1 = 0;
		new_vertex_node->del_flag = 0;
		new_vertex_node->previous_node = NULL;
		new_vertex_node->heap_node = Insert(&priority_queue, new_vertex_node->D, g->nV);
		VertexLinkedNode* new_linked_node = (VertexLinkedNode*)malloc(sizeof(VertexLinkedNode));
		temp2 = new_linked_node;
		new_linked_node->distance = distance;
		new_linked_node->edge = new_edge;
		new_linked_node->next_linked_node = NULL;
		new_vertex_node->linked = new_linked_node;
		*(g->vertices + g->nV) = new_vertex_node;
		g->nV++;
		return_value = 1;
		new2 = new_vertex_node;
	}
	//only update the g->nE in p1 mode above
	temp1->vertex_node = new2;
	temp2->vertex_node = new1;
	return return_value;
}

// Add the time complexity analysis of DeleteEdge() here
/*
<---Assume the number of vertexes is n and the number of edges is m.--->
Because the "hole" exists in the array, that is, after deleting all edges connected with a certain vertex, the vertex should be removed from the array, thus creating a
so-called "hole" that does not contain any vertex, I use N to represent the number of vertexes and the "hole".

So the function need to traverse the array of the vertexes to find the vertex p1 and p2, in which p1 and p2 are the vertexes of the target edge. After finding the vertex,
then need to traverse the linked list and delete the certain edge.

In the worst case, the function will take O(N) + O(degree of p1) + O(degree of p2). From the professor, O(N) is underestimate and O(N+m) is overestimate, the precise
time complexity is O(N+max{degree of p1 and p2}).
*/
void DeleteEdge(Graph g, Edge *e)
{
	// if after deletion, a vertex becomes an isolated vertex
	// then, set the del_flag to 1
	// no need to change the binomial node
	Vertex* p1 = e->p1;
	Vertex* p2 = e->p2;
	int flag1 = 0;
	int flag2 = 0;
	VertexNode* current_vertex_node;
	VertexLinkedNode* current_linked_node;
	VertexLinkedNode* last_linked_node;
	for (int index = 0; index < (g->nV + g->nD); index++)
	{
		current_vertex_node = *(g->vertices + index);
		if (current_vertex_node->del_flag)//the current vertex node is deleted
			continue;
		last_linked_node = NULL;
		if (is_vertex_same(current_vertex_node->v, p1))//the vertex of current_vertex_node is the same as p1
		{
			flag1 = 1;
			current_linked_node = current_vertex_node->linked;
			if (!current_linked_node)//the linked list is empty
				continue;
			if (!(current_linked_node->next_linked_node))//the linked list contains only one linked node
			{
				if (is_vertex_same(current_linked_node->vertex_node->v, p2))
				{
					//the vertex becomes an isolated vertex
					free(current_linked_node->edge);
					free(current_linked_node);
					current_vertex_node->linked = NULL;
					current_vertex_node->del_flag = 1;
					g->nV--;
					g->nD++;
				}
				continue;
			}
			//the linked list has at least two linked nodes
			while (current_linked_node)
			{

				if (is_vertex_same(current_linked_node->vertex_node->v, p2))
				{
					if (!last_linked_node)//the first one
					{
						free(current_linked_node->edge);
						current_vertex_node->linked = current_linked_node->next_linked_node;
						free(current_linked_node);
					}
					else//not the first one
					{
						last_linked_node->next_linked_node = current_linked_node->next_linked_node;
						free(current_linked_node->edge);
						free(current_linked_node);
					}
					break;
				}
				last_linked_node = current_linked_node;
				current_linked_node = current_linked_node->next_linked_node;
			}
		}
		else if (is_vertex_same(current_vertex_node->v, p2))//the vertex of current_vertex_node is the same as p2
		{
			flag2 = 1;
			current_linked_node = current_vertex_node->linked;
			if (!current_linked_node)//the linked list is empty
				continue;
			if (!(current_linked_node->next_linked_node))//the linked list contains only one linked node
			{
				if (is_vertex_same(current_linked_node->vertex_node->v, p1))
				{
					//the vertex becomes an isolated vertex
					free(current_linked_node);
					current_vertex_node->linked = NULL;
					current_vertex_node->del_flag = 1;
					g->nV--;
					g->nD++;
				}
				continue;
			}
			//the linked list has at least two linked nodes
			while (current_linked_node)
			{
				if (is_vertex_same(current_linked_node->vertex_node->v, p1))
				{
					if (!last_linked_node)//the first one
					{
						current_vertex_node->linked = current_linked_node->next_linked_node;
						free(current_linked_node);
					}
					else//not the first one
					{
						last_linked_node->next_linked_node = current_linked_node->next_linked_node;
						free(current_linked_node);
					}
					break;
				}
				last_linked_node = current_linked_node;
				current_linked_node = current_linked_node->next_linked_node;
			}
		}
		if (flag1 && flag2)
		{
			g->nE--;
			break;
		}
	}
}

// Add the time complexity analysis of ReachableVertices() here
/*
This function uses the BFS to get the reachable vertexes.
<---Assume the graph contains n vertexes and m edges.--->
Because the "hole" exists in the array, that is, after deleting all edges connected with a certain vertex, the vertex should be removed from the array, thus creating a
so-called "hole" that does not contain any vertex, I use N to represent the number of vertexes and the "hole".

The simplified structure of this function is:

Function:
	find the starting vertex (Time: O(N))
	BFS processing (Time: O(m+n))
	reset the BFS lebel (Time: O(N))

Explanation: Because in the BFS processing, every vertex and every edge are visited only once, and the operation on each vertex or edge takes constant time, so the
time complexity of the BFS processing is O(m+n). So the overall time complexity of this function is O(N) + O(m+n) + O(N), which is O(m+N).
*/
void ReachableVertices(Graph g, Vertex *v)
{
	//BFS
	//first, find the vertice
	VertexNode* corresponding_node = NULL;
	int comma_flag = 0;
	for (int index = 0; index < (g->nV + g->nD); index++)
	{
		if ((*(g->vertices + index))->del_flag)//the current vertex node is deleted
			continue;
		if (is_vertex_same((*(g->vertices + index))->v, v))
		{
			corresponding_node = *(g->vertices + index);
			break;
		}
	}
	if (!corresponding_node)
		return;
	queue* current_queue = initQueue();
	enqueue(current_queue, corresponding_node);
	corresponding_node->BFS_label_v1 = 1;
	VertexNode* current_vertex_node;
	VertexLinkedNode* current_vertex_linked_node;
	while (!isQueueEmpty(current_queue))//while the queue is not empty
	{
		current_vertex_node = dequeue(current_queue);
		if (!is_vertex_same(current_vertex_node->v, v))
		{
			if (comma_flag)
				printf(",");
			printf("(%d, %d)", current_vertex_node->v->x, current_vertex_node->v->y);
			comma_flag = 1;
		}
		current_vertex_linked_node = current_vertex_node->linked;
		while (current_vertex_linked_node)
		{
			if (!current_vertex_linked_node->vertex_node->BFS_label_v1)
			{
				current_vertex_linked_node->vertex_node->BFS_label_v1 = 1;
				enqueue(current_queue, current_vertex_linked_node->vertex_node);
			}
			current_vertex_linked_node = current_vertex_linked_node->next_linked_node;
		}
	}
	printf("\n");
	// reset all BFS_label_v1 to 0, to solve the problem of not able to reuse this function
	for (int index = 0; index < (g->nV + g->nD); index++)
	{
		if ((*(g->vertices + index))->del_flag)//the current vertex node is deleted
			continue;
		(*(g->vertices + index))->BFS_label_v1 = 0;
	}
}

// self-written function used to change the key of Binomial node, with time complexity of O(logn)
void update_key(BinomialHeap* heap, HeapNode* current_node, double new_key)
{
	double old_key = current_node->key;
	current_node->key = new_key;
	HeapNode* temp_node = (HeapNode*)malloc(sizeof(HeapNode));////////////////////////////////////////
	HeapNode* parent;
	if (old_key > new_key)//up burble
	{
		while (current_node->parent)
		{
			if (current_node->parent->key <= current_node->key)//no need to swap
				break;
			else//swap
			{
				parent = current_node->parent;
				temp_node->first_child = current_node->first_child;
				temp_node->last_child = current_node->last_child;
				temp_node->seq_num = current_node->seq_num;
				temp_node->right_sibling = current_node->right_sibling;
				temp_node->left_sibling = current_node->left_sibling;
				current_node->seq_num = parent->seq_num;
				current_node->right_sibling = parent->right_sibling;
				if (parent->right_sibling)
					parent->right_sibling->left_sibling = current_node;
				current_node->left_sibling = parent->left_sibling;
				if (parent->left_sibling)
					parent->left_sibling->right_sibling = current_node;
				parent->right_sibling = temp_node->right_sibling;
				if (temp_node->right_sibling)
					temp_node->right_sibling->left_sibling = parent;
				parent->left_sibling = temp_node->left_sibling;
				if (temp_node->left_sibling)
					temp_node->left_sibling->right_sibling = parent;
				if (parent->first_child == current_node)
					current_node->first_child = parent;
				else
					current_node->first_child = parent->first_child;
				if (parent->last_child == current_node)
					current_node->last_child = parent;
				else
					current_node->last_child = parent->last_child;
				if (parent->parent)
				{
					if (parent->parent->first_child == parent)
						parent->parent->first_child = current_node;
					if (parent->parent->last_child == parent)
						parent->parent->last_child = current_node;
					current_node->parent = parent->parent;
				}
				else
				{
					current_node->parent = parent->parent;
				}
				parent->parent = current_node;
				parent->first_child = temp_node->first_child;
				parent->last_child = temp_node->last_child;
				parent->seq_num = temp_node->seq_num;
				HeapNode* x = current_node->first_child;
				while (x)
				{
					x->parent = current_node;
					x = x->right_sibling;
				}
				x = parent->first_child;
				while (x)
				{
					x->parent = parent;
					x = x->right_sibling;
				}
			}
		}
		if (!current_node->parent)
		{
			if (!current_node->left_sibling)
				heap->first_child = current_node;
			if (!current_node->right_sibling)
				heap->last_child = current_node;
		}
	}
	else//down bulble
	{
		//I think there is no condition for down bulble here
		printf("Require for down bulble.\n");
	}
}

// Add the time complexity analysis of ShortestPath() here
/*
<---Assume the number of vertexes is n and the number of edges is m.--->
Because the "hole" exists in the array, that is, after deleting all edges connected with a certain vertex, the vertex should be removed from the array, thus creating a
so-called "hole" that does not contain any vertex, I use N to represent the number of vertexes and the "hole".

The simplified structure of this function is:
	traverse the array to get the first vertex u, set its D to 0, update the key, and update the D of its adjacent vertexes (Time: O(n)+O(logn)+O(degree(u)))
	while the minNode from the priority queue is not INFINITY: (loop)
		remove the minNode from the priority queue (Time: n(logn))
		update the D of its adjacent vertexes and update the priority queue (Time: O(degree of vertex*logn))
		if the node is v: (Time: O(1))
			break
	using stack to reverse the path and print it out (Time: worst O(n))
	restore D and the priority queue (Time: O(nlogn))

Explanation:
In the while loop section, in the worst case, every vertex in the connected component containing the vertex u will be visited only once, and for each operation, it will
take O((1+degree(vertex))*logn) time, so the time complexity of the while section should be O((m+n)logn).
So the overall time complexity of this function is: O(n)+O(logn)+O(degree(u))+O((m+n)logn)+O(nlogn), which is O((m+n)logn).
*/
void ShortestPath(Graph g, Vertex *u, Vertex *v)
{
	int flag1 = 0;
	int flag2 = 0;
	VertexNode* current_vertex_node;
	VertexLinkedNode* current_linked_node;
	VertexNode* destination;
	for (int index = 0; index < (g->nV + g->nD); index++)
	{
		current_vertex_node = *(g->vertices + index);
		if (current_vertex_node->del_flag)//the current vertex node is deleted
			continue;
		if (is_vertex_same(current_vertex_node->v, u))//starts with u only
		{
			flag1 = 1;
			current_vertex_node->D = 0;
			//update the key of the Binomial node and pop it from the heap
			update_key(priority_queue, current_vertex_node->heap_node, 0);
			RemoveMin(&priority_queue);
			//update the key of the Binomial nodes
			current_linked_node = current_vertex_node->linked;
			while (current_linked_node)
			{
				update_key(priority_queue, current_linked_node->vertex_node->heap_node, current_linked_node->distance);
				current_linked_node->vertex_node->previous_node = current_vertex_node;
				current_linked_node->vertex_node->D = current_linked_node->distance;
				current_linked_node = current_linked_node->next_linked_node;
			}
		}
		else if (is_vertex_same(current_vertex_node->v, v))
		{
			flag2 = 1;
			destination = current_vertex_node;
		}
		if (flag1 && flag2)
			break;
	}
	if (!(flag1 && flag2))
		return;
	while (Min(priority_queue) < COMPARE_INFINITY)
	{
		HeapNode* x = RemoveMin(&priority_queue);
		VertexNode* co_x = *(g->vertices + x->value);
		//printf("%f\n", co_x->D);//debug use
		VertexLinkedNode* x_linked = co_x->linked;
		while (x_linked)
		{
			if ((x_linked->distance + co_x->D) < x_linked->vertex_node->D)
			{
				update_key(priority_queue, x_linked->vertex_node->heap_node, x_linked->distance + co_x->D);
				x_linked->vertex_node->D = x_linked->distance + co_x->D;
				x_linked->vertex_node->previous_node = co_x;
			}
			x_linked = x_linked->next_linked_node;
		}
		if (is_vertex_same(co_x->v, v))
			break;
	}
	int destination_flag = 0;
	if (destination->previous_node)
		destination_flag = 1;
	struct stack* current_stack = emptyStack();
	while (destination->previous_node)
	{
		push(current_stack, destination->previous_node);
		destination = destination->previous_node;
	}
	while (current_stack->n)
	{
		VertexNode* temp_node = pop(current_stack);
		printf("(%d, %d),", temp_node->v->x, temp_node->v->y);
	}
	if (destination_flag)
		printf("(%d, %d)\n", v->x, v->y);
	priority_queue->first_child = NULL;
	priority_queue->last_child = NULL;
	priority_queue->size = 0;
	for (int index = 0; index < (g->nD + g->nV); index++)
	{
		VertexNode* current = *(g->vertices + index);
		if (current->del_flag)
			continue;
		current->D = RINFINITY;
		current->previous_node = NULL;
		current->heap_node = Insert(&priority_queue, current->D, index);
	}
	//print out
	//reset D and restore the priority queue
}

// self-written function used to free the VertexNode
void freeVertexNode(VertexNode* current_vertex_node)
{
	VertexLinkedNode* current_linked_node = current_vertex_node->linked;
	VertexLinkedNode* temp_linked_node;
	while (current_linked_node)//free the linked list in the vertex node
	{
		temp_linked_node = current_linked_node;
		current_linked_node = current_linked_node->next_linked_node;
		free(temp_linked_node);
	}
	free(current_vertex_node->v);//free the vertex pointed from the vertex node
	free(current_vertex_node->heap_node);//free the heap node pointed from the vertex node
	free(current_vertex_node);//free the vertex node
	return;
}

// Add the time complexity analysis of FreeGraph() here
/*
<---Assume the number of vertexes is n and the number of edges is m.--->
Because the "hole" exists in the array, that is, after deleting all edges connected with a certain vertex, the vertex should be removed from the array, thus creating a
so-called "hole" that does not contain any vertex, I use N to represent the number of vertexes and the "hole".

The simplified structure of this function is:

Function:
	free the priority queue
	free each vertex node
	free the graph

Explanation:
The time used to free the priority queue is O(1) because each binomial node is deleted during the deletion of each vertex node.
The time used in free each vertex node is O(degree(vertex)). Because in the free procedure, every vertex is used
only once, and the number of freeing edge is 2m (also the overall degree in the graph), the time complexity in freeing all vertexes is O(m+N).
The time used to free the graph structure is O(1).
So the overall time complexity of this function is O(1)+O(m+N)+O(1), which is O(m+N).
*/
void FreeGraph(Graph g)
{
	//free binomial heap
	free(priority_queue);
	for (int index = 0; index < g->nV; index++)//free all vertex nodes pointed from g
	{
		freeVertexNode(*(g->vertices + index));
	}
	free(g);//free the graph
	return;
}

// Add the time complexity analysis of ShowGraph() here
/*
<---Assume the number of vertexes is n and the number of edges is m.--->
And assume the overall number of the vertexes as well as the "hole" is N.

Because each vertex and each edge are visited exactly two times (one time in the BFS procedure, and one time in the reset procedure). So the time complexity
of this function is O(N+m).
*/
void ShowGraph(Graph g)
{
	//BFS
	if (!g->nV)//no vertex in graph
	{
		return;
	}
	queue* current_queue = initQueue();
	for (int index = 0; index < (g->nD + g->nV); index++)
	{
		if ((*(g->vertices + index))->del_flag)
			continue;
		if ((*(g->vertices + index))->BFS_label_v1)
			continue;
		enqueue(current_queue, *(g->vertices + index));
		(*(g->vertices + index))->BFS_label_v1 = 1;
		VertexNode* current_vertex_node;
		VertexNode* temp;
		VertexLinkedNode* current_vertex_linked_node;
		while (!isQueueEmpty(current_queue))//while the queue is not empty
		{
			current_vertex_node = dequeue(current_queue);
			current_vertex_linked_node = current_vertex_node->linked;
			while (current_vertex_linked_node)
			{
				temp = current_vertex_linked_node->vertex_node;
				temp->BFS_label_v1 = 1;
				if (current_vertex_linked_node->edge->status)
				{
					current_vertex_linked_node = current_vertex_linked_node->next_linked_node;
					continue;
				}
				enqueue(current_queue, temp);
				current_vertex_linked_node->edge->status = 1;
				printf("(%d, %d),(%d, %d) ", current_vertex_node->v->x, current_vertex_node->v->y, temp->v->x, temp->v->y);
				current_vertex_linked_node = current_vertex_linked_node->next_linked_node;
			}
		}
	}
	printf("\n");
	// reset all BFS_label_v1 to 0, to solve the problem of not able to reuse this function
	for (int index = 0; index < (g->nV + g->nD); index++)
	{
		if ((*(g->vertices + index))->del_flag)//the current vertex node is deleted
			continue;
		(*(g->vertices + index))->BFS_label_v1 = 0;
		VertexLinkedNode* current_linked_node = (*(g->vertices + index))->linked;
		while (current_linked_node)
		{
			current_linked_node->edge->status = 0;//reset the status of the edge in the linked list
			current_linked_node = current_linked_node->next_linked_node;
		}
	}
}

int main()
//sample main for testing 
{
	Graph g1;
	Edge *e_ptr;
	Vertex *v1, *v2;

	// Create an empty graph g1;
	g1 = CreateEmptyGraph();

	// Create first connected component 
	// Insert edge (0,0)-(0,10)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 0;
	v1->y = 0;
	v2->x = 0;
	v2->y = 10;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	// Insert edge (0,0)-(5,6)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 0;
	v1->y = 0;
	v2->x = 5;
	v2->y = 6;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	// Insert edge (0, 10)-(10, 10)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 0;
	v1->y = 10;
	v2->x = 10;
	v2->y = 10;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	// Insert edge (0,10)-(5,6)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 0;
	v1->y = 10;
	v2->x = 5;
	v2->y = 6;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	// Insert edge (0,0)-(5,4)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 0;
	v1->y = 0;
	v2->x = 5;
	v2->y = 4;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	// Insert edge (5, 4)-(10, 4)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 5;
	v1->y = 4;
	v2->x = 10;
	v2->y = 4;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	// Insert edge (5,6)-(10,6)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 5;
	v1->y = 6;
	v2->x = 10;
	v2->y = 6;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	// Insert edge (10,10)-(10,6)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 10;
	v1->y = 10;
	v2->x = 10;
	v2->y = 6;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	// Insert edge (10, 6)-(10, 4)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 10;
	v1->y = 6;
	v2->x = 10;
	v2->y = 4;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	// Create second connected component
	// Insert edge (20,4)-(20,10)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 20;
	v1->y = 4;
	v2->x = 20;
	v2->y = 10;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	// Insert edge (20,10)-(30,10)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 20;
	v1->y = 10;
	v2->x = 30;
	v2->y = 10;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	// Insert edge (25,5)-(30,10) 	
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 25;
	v1->y = 5;
	v2->x = 30;
	v2->y = 10;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	if (InsertEdge(g1, e_ptr) == 0) printf("edge exists\n");

	//Display graph g1
	ShowGraph(g1);

	// Find the shortest path between (0,0) and (10,6) 
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 0;
	v1->y = 0;
	v2->x = 10;
	v2->y = 6;
	ShortestPath(g1, v1, v2);
	free(v1);
	free(v2);

	// Delete edge (0,0)-(5, 6)
	e_ptr = (Edge*)malloc(sizeof(Edge));
	assert(e_ptr != NULL);
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 0;
	v1->y = 0;
	v2->x = 5;
	v2->y = 6;
	e_ptr->p1 = v1;
	e_ptr->p2 = v2;
	DeleteEdge(g1, e_ptr);
	free(e_ptr);
	free(v1);
	free(v2);

	// Display graph g1
	ShowGraph(g1);

	// Find the shortest path between (0,0) and (10,6) 
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 0;
	v1->y = 0;
	v2->x = 10;
	v2->y = 6;
	ShortestPath(g1, v1, v2);
	free(v1);
	free(v2);

	// Find the shortest path between (0,0) and (25,5)
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v2 = (Vertex *)malloc(sizeof(Vertex));
	assert(v2 != NULL);
	v1->x = 0;
	v1->y = 0;
	v2->x = 25;
	v2->y = 5;
	ShortestPath(g1, v1, v2);
	free(v1);
	free(v2);

	// Find reachable vertices of (0,0)
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v1->x = 0;
	v1->y = 0;
	ReachableVertices(g1, v1);
	free(v1);

	// Find reachable vertices of (20,4)
	v1 = (Vertex*)malloc(sizeof(Vertex));
	assert(v1 != NULL);
	v1->x = 20;
	v1->y = 4;
	ReachableVertices(g1, v1);
	free(v1);

	// Free graph g1
	FreeGraph(g1);

	return 0;
}

//by Yu Liu, z5190675
