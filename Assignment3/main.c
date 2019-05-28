#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

// This template is only a guide 
// You need to include additional fields, data structures and functions  

//self-written data type used to store job info
//typedef struct JobInfo {
//	int job_name;//the name
//	int job_start_time;//the time when the job is allocated to a core
//	int job_finish_time;//the time the job should get finished
//} JobInfo;

// data type for heap nodes
typedef struct HeapNode {
	// each node stores the priority (key), name, execution time,
	//  release time and deadline of one task
	int key; //key of this task
	int TaskName;  //task name 
	int Etime; //execution time of this task
	int Rtime; // release time of this task
	int Dline; // deadline of this task
	// add additional fields here
	// int initial_children_num;//this is used as the initial length of the children array
	// int child_num;//actual number of children
	// struct HeapNode** children;
	struct HeapNode* first_child;//new added
	struct HeapNode* last_child;//new added
	struct HeapNode* parent;//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	// the following are the properties only used when the node is a root of a binomial tree
	int seq_num;
	struct HeapNode* right_sibling;
	struct HeapNode* left_sibling;
} HeapNode;

//data type for a priority queue (heap) 
typedef struct BinomialHeap { //this is heap header
	int  size;      // count of items in the heap
	// add additional fields here
	struct HeapNode* first_child;
	struct HeapNode* last_child;
} BinomialHeap;

// create a new heap node to store an item (task) 
HeapNode *newHeapNode(int k, int n, int c, int r, int d, ...)
{ // k:key, n:task name, c: execution time, r:release time, d:deadline
  // ... you need to define other parameters yourself) 	 
	HeapNode *new;
	new = malloc(sizeof(HeapNode));
	assert(new != NULL);
	new->key = k;
	new->TaskName = n;
	new->Etime = c;
	new->Rtime = r;
	new->Dline = d;
	// initialise other fields 
	// new->initial_children_num = 10;
	// new->child_num = 0;
	// new->children = (struct HeapNode**)calloc(new->initial_children_num , sizeof(struct HeapNode*));
	// assert(new->children != NULL);
	new->seq_num = 0;//new added
	new->first_child = NULL;//new added
	new->last_child = NULL;//new added
	 new->parent = NULL;//&&&&&&&&&&&&&&&&&&&&&&&&&&&
	new->right_sibling = NULL;
	new->left_sibling = NULL;
	// new->seq_num = -1;//means it is not a root
	return new;
}

// create a new empty heap-based priority queue
BinomialHeap *newHeap()
{ // this function creates an empty binomial heap-based priority queue
	BinomialHeap *T;
	T = malloc(sizeof(BinomialHeap));
	assert(T != NULL);
	// initialise all the fields here
	T->size = 0;
	T->first_child = NULL;
	T->last_child = NULL;
	return T;
}

//self-written function used to create a new JobInfo struct object
//struct JobInfo* newJobInfo(int job_name, int job_start, int job_finish)
//{
//	struct JobInfo* new_job = (struct JobInfo*)malloc(sizeof(struct JobInfo));
//	new_job->job_name = job_name;
//	new_job->job_start_time = job_start;
//	new_job->job_finish_time = job_finish;
//	return new_job;
//}

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
				new_heap->last_child->parent = temp_node;//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
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
				temp_node->parent = new_heap->last_child;//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
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
				temp_node->parent = new_heap->last_child;//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
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
	sub_node->parent = temp_node;//&&&&&&&&&&&&&&&&&&&&&&&&
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
void Insert(struct BinomialHeap** T, int k, int n, int c, int r, int d)
{
	struct BinomialHeap* temp_heap = newHeap();
	struct HeapNode* new_node = newHeapNode(k, n, c, r, d);
	new_node->seq_num = 0;
	temp_heap->size++;
	temp_heap->first_child = new_node;
	temp_heap->last_child = new_node;
	*T = merge_heaps(*T, temp_heap);
}

void print_tree(struct HeapNode* current_tree)
{
	struct HeapNode* temp = current_tree->first_child;
	printf("%d ", current_tree->key);
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
	int current_smallest_key = current_node->key;
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
			current_smallest_node->parent = NULL;//&&&&&&&&&&&&&&&&&
			return current_smallest_node;
		}
		else//current_smallest node has at least one child
		{
			(*T)->size -= 1;
			(*T)->first_child = current_smallest_node->first_child;
			(*T)->last_child = current_smallest_node->last_child;
			//update the parent
			HeapNode* current = (*T)->first_child;//&&&&&&&&&&&
			while (current)//&&&&&&&&&&&&&
			{//&&&&&&&&&&&&&&&
				current->parent = NULL;//&&&&&&&&&&&&&&&
				current = current->right_sibling;//&&&&&&&&&&&&&&&
			}//&&&&&&&&&&&&&&&&&&&&&
			current_smallest_node->seq_num = 0;
			current_smallest_node->first_child = NULL;
			current_smallest_node->last_child = NULL;
			current_smallest_node->parent = NULL;//&&&&&&&&&&&&&&&&&&&&&&&&&
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
		HeapNode* current = new_heap->first_child;//&&&&&&&&&&&
		while (current)//&&&&&&&&&&&&&
		{//&&&&&&&&&&&&&&&
			current->parent = NULL;//&&&&&&&&&&&&&&&
			current = current->right_sibling;//&&&&&&&&&&&&&&&
		}//&&&&&&&&&&&&&&&&&&&&&
		current_smallest_node->first_child = NULL;
		current_smallest_node->last_child = NULL;
		current_smallest_node->parent = NULL;//&&&&&&&&&&&&&&&&&&77
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
int Min(struct BinomialHeap *T)//this version returns the min key
{
	// put your code here
	if (!T->size)//the heap is empty
	{
		printf("The heap is empty, cannot return the min from it.\n");
		exit(1);
	}
	int current_smallest = T->first_child->key;
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

void update_key(BinomialHeap* current_heap, HeapNode* current_node, int new_key)
{
	int old_key = current_node->key;
	current_node->key = new_key;
	HeapNode* temp_node = (HeapNode*)malloc(sizeof(HeapNode));////////////////////////////////////////
	HeapNode* parent;
	if (old_key > new_key)//up bulble
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
	}
	else//down bulble
	{
		//I think there is no condition for down bulble here
		printf("Require for down bulble.\n");
	}
}

// put your time complexity analysis for MyTaskScheduler here
/*
<---Assume the number of jobs in n--->
From the time complexity analysis before, the insert, min, and removemin all take O(logn) time.

The simplified structure of this function is:

Function:
	create two new heaps (Time: constant)
	for each element from file1, insert it to heap1 (Time: O(nlogn) )
	while two heaps are not both empty: (Loop, run n times)
		//this loop run n times because every job need to be executed exactly once
		if at least one core is idle:
			get the available job with the minimum release time and the minimum deadline
			and then allocate this job to an idle core
		else (no core is idle, so the checkpoint is the departure of the nearest finished job):
			get the available job with the available release time and the minimum deadline
			and then allocate this job to the core that finishes the job

In the body of the while loop, the program needs to remove min element from the heap, insert new element
to the heap, and find the min element in the heap, but in the worst case, these operations take O(logn) time.
And also, because the m is not treated as a constant in the time complexity analysis, I did not use an array as the container to
hold the core info, instead, I use two binomial heaps to hold the idle core and busy core, respectively, which can provide
a search time of O(logm). Because adding all idle cores to the idle core heap will use nlogn time, instead,
at the very first beginning, when I allocate the first job for each core, I used a special loop and avoid the
high-time-consuming initialization.
So, as the lectures said in the announcement, this can surely prove that m is removed from the time complexity.

So the overall time is O(n*logn), in which n is the number of jobs.
*/
int TaskScheduler(char *f1, char *f2, int m)
{
	// put your code here
	//create two heaps
	//note that while reading from f1, only save the file in the heap1
	//each time get the least-release-time job from heap1, just remove it and add it to heap2
	struct BinomialHeap* heap1 = newHeap();//store the release time as the key
	struct BinomialHeap* heap2 = newHeap();//store the deadline as the key
	// read from file1
	FILE* f = fopen(f1, "r");
	FILE* out_f = fopen(f2, "w");
	if (!f)
	{
		printf("file1 does not exist\n");
		exit(1);
	}
	char temp_char;
	int loc = 0;//0->waiting for v, 1->waiting for c, 2->waiting for r, 3->waiting for d
	int temp = 0;
	int temp_flag = 0;//means already numbers in temp
	int current_arr[4];
	// format: v c r d
	while (fscanf(f, "%c", &temp_char) != EOF)
	{
		//if temp char is space or new line character
		if (temp_char == ' ' || temp_char == '\n')
		{
			// if there is no number stored in temp
			if (!temp_flag)
				continue;
			else//there is number stored in temp
			{
				current_arr[loc] = temp;
				temp_flag = 0;
				loc = (loc + 1) % 4;
				if (!loc)
				{
					//add the existing four elements in the heap
					Insert(&heap1, current_arr[2], current_arr[0], current_arr[1], current_arr[2], current_arr[3]);
					//print_heap(heap1);
					//printf("%d %d %d %d\n", current_arr[0], current_arr[1], current_arr[2], current_arr[3]);
				}
			}
		}
		else if (temp_char >= '0' && temp_char <= '9')
		{
			if (!temp_flag)//no number stored in temp
			{
				temp = temp_char - '0';
				temp_flag = 1;
			}
			else
			{
				temp = temp * 10 + (temp_char - '0');
			}
		}
		else
		{
			if (loc)
				printf("input error when reading the attribute of the task %d\n", current_arr[0]);
			else
				printf("input error when reading the attribute of the task\n");//task name is wrong
		}
	}
	if (temp_flag)
	{
		current_arr[loc] = temp;
		loc = (loc + 1) % 4;
		if (!loc) {
			Insert(&heap1, current_arr[2], current_arr[0], current_arr[1], current_arr[2], current_arr[3]);
			//print_heap(heap1);
			//printf("%d %d %d %d\n", current_arr[0], current_arr[1], current_arr[2], current_arr[3]);
		}
		else
			printf("input error when reading the attribute of the task %d\n", current_arr[0]);
	}
	fclose(f);
	//start scheduling
	int idle_core = m;//the number of idle cores in the system
	int idle_core_flag = 1;//after allocating the cores at the very first beginning, turn the idle_core down
	//<---struct JobInfo** core_arr = (struct JobInfo**)malloc(m * sizeof(struct JobInfo*));//hold the job info structure in the core
	//create a data structure to hold the core with the search time of O(logn)
	//plan: use two binomial heaps, one for idle cores, with the core number as the key
	//one for non-idle cores, with the finish time as the key
	//what if I do not initialize all the m cores in the first heap at the first beginning
	//just use a idle_num to hold
	//until all the cores are allocated one job
	//then, at this point, manipulate cores between these two heaps
	struct BinomialHeap* idle_core_heap = newHeap();
	//data structure of elements stored in idle_core_heap:
	//key: core_num
	//n, c, r, d: all not used in this heap, so set arbitiary values
	struct BinomialHeap* busy_core_heap = newHeap();
	//data structure of elements stored in busy_core_heap:
	//key: finish_time
	//n: job_name, c: core_num, r: job_actual_start_time, d: job_finish_time
	//<---int* core_flag = (int*)calloc(m, sizeof(int));//hold the flag in the core, all element initialized to be zero
	int nearest_depart;//the nearest time of job to be finished
	int nearest_core;//the corresponding core of the job, from 0 to m-1
	int nearest_depart_flag = 0;//0->there is no nearest depart, 1->there is a nearest depart available
	//int current_depart;//used for output, temp saved//did not get used ever?
	//int current_core;//used for output, temp saved//did not get used ever?
	struct HeapNode* first_from_heap1;
	struct HeapNode* first_from_heap2;
	struct HeapNode* first_from_idle_heap;
	struct HeapNode* first_from_busy_heap;
	int current_min_release_time;
	int return_result = 1;
	//print when allocating the job, rather than the job finished
	while (heap1->size || heap2->size)//while heap is not empty
	{
		if (idle_core_flag && idle_core > 0)//there is at least one idle core, operating at the first beginning
		{
			//in this condition, because there is at least one core idle, so there are several safe assumptions:

			//if there still are jobs in the heap2, use the heap2
			//if there is no job in the heap2, then need to get jobs from the heap1 and add it to heap2
			if (!heap2->size)
			{
				//find the available jobs
				//if there are at least one job with the minimum release time, then compare the deadline of them
				//if there is only one job with the minimum release time, then allocate that job to the heap directly
				first_from_heap1 = RemoveMin(&heap1);
				Insert(&heap2, first_from_heap1->Dline, first_from_heap1->TaskName, first_from_heap1->Etime, first_from_heap1->Rtime, first_from_heap1->Dline);
				current_min_release_time = first_from_heap1->key;
				while (heap1->size && Min(heap1) == current_min_release_time)
				{
					first_from_heap1 = RemoveMin(&heap1);
					Insert(&heap2, first_from_heap1->Dline, first_from_heap1->TaskName, first_from_heap1->Etime, first_from_heap1->Rtime, first_from_heap1->Dline);
				}
			}
			//safe assumption here: heap2 is not empty
			//so at this point, the jobs in the heap2 are all available
			//need to get the one with the smallest deadline from these jobs
			first_from_heap2 = RemoveMin(&heap2);
			//then, add the job to an idle core, and decrement the idle core num
			Insert(&busy_core_heap, first_from_heap2->Rtime + first_from_heap2->Etime, first_from_heap2->TaskName, m - idle_core, first_from_heap2->Rtime, first_from_heap2->Rtime + first_from_heap2->Etime);
			nearest_depart = MinNode(busy_core_heap)->key;
			nearest_core = MinNode(busy_core_heap)->Etime;
			if (first_from_heap2->Rtime + first_from_heap2->Etime > first_from_heap2->Dline)
			{
				return_result = 0;
			}
			fprintf(out_f, "%d core%d %d\n", first_from_heap2->TaskName, m - idle_core + 1, first_from_heap2->Rtime);
			idle_core--;
			if (!idle_core)
				idle_core_flag = 0;
		}
		else if (!idle_core_flag && idle_core_heap->size)//after the very first beginning and there is also idle cores
		{
			//in this condition, because there is at least one core idle, so there are several safe assumptions:

			//if there still are jobs in the heap2, use the heap2
			//if there is no job in the heap2, then need to get jobs from the heap1 and add it to heap2
			if (!heap2->size)
			{
				//find the available jobs
				//if there are at least one job with the minimum release time, then compare the deadline of them
				//if there is only one job with the minimum release time, then allocate that job to the heap directly
				first_from_heap1 = RemoveMin(&heap1);
				Insert(&heap2, first_from_heap1->Dline, first_from_heap1->TaskName, first_from_heap1->Etime, first_from_heap1->Rtime, first_from_heap1->Dline);
				current_min_release_time = first_from_heap1->key;
				while (heap1->size && Min(heap1) == current_min_release_time)
				{
					first_from_heap1 = RemoveMin(&heap1);
					Insert(&heap2, first_from_heap1->Dline, first_from_heap1->TaskName, first_from_heap1->Etime, first_from_heap1->Rtime, first_from_heap1->Dline);
				}
			}
			//safe assumption here: heap2 is not empty
			//so at this point, the jobs in the heap2 are all available
			//need to get the one with the smallest deadline from these jobs
			first_from_heap2 = RemoveMin(&heap2);
			//then, add the job to an idle core, and decrement the idle core num
			//the idle core with the smallest core num is:
			first_from_idle_heap = RemoveMin(&idle_core_heap);
			Insert(&busy_core_heap, first_from_heap2->Rtime + first_from_heap2->Etime, first_from_heap2->TaskName, first_from_idle_heap->key, first_from_heap2->Rtime, first_from_heap2->Rtime + first_from_heap2->Etime);
			nearest_depart = MinNode(busy_core_heap)->key;
			nearest_core = MinNode(busy_core_heap)->Etime;
			if (first_from_heap2->Rtime + first_from_heap2->Etime > first_from_heap2->Dline)
			{
				return_result = 0;
			}
			fprintf(out_f, "%d core%d %d\n", first_from_heap2->TaskName, first_from_idle_heap->key + 1, first_from_heap2->Rtime);
		}
		else//no idle core available, so the check point is the departure of the job
		{
			//if there is available jobs, assign the job with smallest deadline to the core
			//get the job available from the heap1
			first_from_busy_heap = RemoveMin(&busy_core_heap);
			nearest_depart = first_from_busy_heap->key;
			nearest_core = first_from_busy_heap->Etime;
			int current_time = nearest_depart;
			while (heap1->size && Min(heap1) <= nearest_depart)
			{
				first_from_heap1 = RemoveMin(&heap1);
				Insert(&heap2, first_from_heap1->Dline, first_from_heap1->TaskName, first_from_heap1->Etime, first_from_heap1->Rtime, first_from_heap1->Dline);
			}
			if (!heap2->size)//if there is no available job in the heap2
			{
				//remain the core to be idle
				Insert(&idle_core_heap, nearest_core, -1, -1, -1, -1);
				if(busy_core_heap->size)
				{
					nearest_depart = MinNode(busy_core_heap)->key;
					nearest_core = MinNode(busy_core_heap)->Etime;
				}
			}
			else//there is at least one available job in the heap2
			{
				//select the job with the smallest deadline
				first_from_heap2 = RemoveMin(&heap2);
				if (first_from_heap2->Rtime > current_time)
					current_time = first_from_heap2->Rtime;
				//allocate this job to the smallest idle core
				Insert(&busy_core_heap, current_time + first_from_heap2->Etime, first_from_heap2->TaskName, nearest_core, current_time, current_time + first_from_heap2->Etime);
				if (current_time + first_from_heap2->Etime > first_from_heap2->Dline)
				{
					return_result = 0;
				}
				fprintf(out_f, "%d core%d %d\n", first_from_heap2->TaskName, nearest_core + 1, current_time);
				nearest_depart = MinNode(busy_core_heap)->key;
				nearest_core = MinNode(busy_core_heap)->Etime;

			}
			//also need to travel the core array to get the nearest time and nearest core
			if (busy_core_heap->size)
			{
				nearest_depart = MinNode(busy_core_heap)->key;
				nearest_core = MinNode(busy_core_heap)->Etime;
			}
		}
		//printf("next depart: %d\n", nearest_depart);//for debug use
	}
	fclose(out_f);
	return return_result;
}

int main() //sample main for testing 
{
	//int i;
	//i = TaskScheduler("samplefile1.txt", "feasibleschedule1.txt", 4);
	//if (i == 0) printf("No feasible schedule!\n");
	///* There is a feasible schedule on 4 cores */
	//i = TaskScheduler("samplefile1.txt", "feasibleschedule2.txt", 3);
	//if (i == 0) printf("No feasible schedule!\n");
	///* There is no feasible schedule on 3 cores */
	//i = TaskScheduler("samplefile2.txt", "feasibleschedule3.txt", 5);
	//if (i == 0) printf("No feasible schedule!\n");
	///* There is a feasible schedule on 5 cores */
	//i = TaskScheduler("samplefile2.txt", "feasibleschedule4.txt", 4);
	//if (i == 0) printf("No feasible schedule!\n");
	///* There is no feasible schedule on 4 cores */
	//i = TaskScheduler("samplefile3.txt", "feasibleschedule5.txt", 2);
	//if (i == 0) printf("No feasible schedule!\n");
	///* There is no feasible schedule on 2 cores */
	//i = TaskScheduler("samplefile4.txt", "feasibleschedule6.txt", 2);
	//if (i == 0) printf("No feasible schedule!\n");
	///* There is a feasible schedule on 2 cores */
	//return 0;

	//testing for parent attribute of the heap node

	struct BinomialHeap* T = newHeap();
	Insert(&T, 0, 1, 4, 0, 4);
	//printf("Size: %d\n", T->size);
	Insert(&T, 1, 2, 4, 1, 5);
	//printf("Size: %d\n", T->size);
	//make sure
	// printf("First child: %d\n", T->first_child->key);
	// printf("Last child: %d\n", T->last_child->key);
	// printf("Size: %d\n", T->size);
	// printf("%d\n", (*T->first_child->children)->key);
	// printf("%d\n", (T->first_child->right_sibling == NULL));
	// printf("%d\n", (T->first_child->left_sibling == NULL));
	// printf("%d\n", T->first_child->child_num);
	// printf("%d\n", (*T->first_child->children)->child_num);
	Insert(&T, 4, 4, 6, 4, 11);
	//printf("Size: %d\n", T->size);
	Insert(&T, 6, 5, 4, 6, 13);
	/*printf("----------------\n");
	printf("%d\n", T->first_child->last_child->key);
	printf("%d\n", T->first_child->last_child->left_sibling->key);
	printf("----------------\n");*/
	//printf("Size: %d\n", T->size);
	//printf("Good.\n");
	Insert(&T, 3, 3, 5, 3, 10);
	Insert(&T, 8, 5, 4, 3, 2);
	Insert(&T, 2, 3, 4, 3, 2);
	/*printf("----------------\n");
	printf("%d\n", T->first_child->right_sibling->right_sibling->first_child->left_sibling == NULL);
	printf("%d\n", T->first_child->right_sibling->right_sibling->first_child->right_sibling->right_sibling == NULL);
	printf("%d\n", T->first_child->right_sibling->right_sibling->first_child->right_sibling->first_child == NULL);
	printf("----------------\n");*/
	Insert(&T, 10, 3, 4, 3, 2);
	/*printf("----------------\n");
	printf("%d\n", T->first_child->last_child->key);
	printf("%d\n", T->first_child->last_child->left_sibling->key);
	printf("%d\n", T->first_child->last_child->left_sibling->left_sibling->key);
	printf("%d\n", T->last_child->last_child->last_child->key);
	printf("----------------\n");*/
	Insert(&T, 24, 3, 4, 3, 2);
	Insert(&T, 9, 3, 4, 3, 2);
	Insert(&T, 7, 3, 4, 3, 2);
	Insert(&T, 12, 3, 4, 3, 2);
	/*printf("**********\n%d\n**********\n", Min(T));*/
	Insert(&T, -1, 3, 4, 3, 2);
	//printf("%d\n", T->first_child->key);
	//printf("%d\n", T->last_child->key);
	/*printf("%d %d %d %d\n", T->first_child->key, T->first_child->right_sibling->key, T->first_child->right_sibling->right_sibling->key, T->last_child->key);
	printf("%d %d %d\n", T->first_child->first_child == NULL, T->first_child->right_sibling->first_child == NULL, T->first_child->right_sibling->right_sibling->first_child == NULL);
	printf("%d %d %d\n", T->first_child->seq_num, T->first_child->right_sibling->seq_num, T->first_child->right_sibling->right_sibling->seq_num);
	printf("B0: %d\n", T->first_child->key);
	printf("B2: %d %d %d %d\n", T->first_child->right_sibling->key, T->first_child->right_sibling->first_child->key, T->first_child->right_sibling->first_child->right_sibling->key, T->first_child->right_sibling->first_child->right_sibling->first_child->key);
	printf("%d %d %d\n", T->last_child->left_sibling->last_child->key, T->last_child->left_sibling->last_child->left_sibling->key, T->last_child->left_sibling->last_child->last_child->key);
	printf("B3: %d %d %d %d %d %d %d %d\n", T->first_child->right_sibling->right_sibling->key, T->first_child->right_sibling->right_sibling->first_child->key, T->first_child->right_sibling->right_sibling->first_child->right_sibling->key, T->first_child->right_sibling->right_sibling->first_child->right_sibling->first_child->key, T->first_child->right_sibling->right_sibling->first_child->right_sibling->right_sibling->key, T->first_child->right_sibling->right_sibling->first_child->right_sibling->right_sibling->first_child->key, T->first_child->right_sibling->right_sibling->first_child->right_sibling->right_sibling->first_child->right_sibling->key, T->first_child->right_sibling->right_sibling->first_child->right_sibling->right_sibling->first_child->right_sibling->first_child->key);
	printf("%d %d %d\n", T->last_child->last_child->last_child->last_child->key, T->last_child->last_child->last_child->key, T->last_child->last_child->last_child->left_sibling->key);
	printf("%d %d %d\n", T->last_child->last_child->left_sibling->last_child->key, T->last_child->last_child->left_sibling->key, T->last_child->last_child->left_sibling->left_sibling->key);
	printf("-----------------\n");
	printf("%d %d\n", T->last_child->key, T->last_child->last_child->key);
	printf("%d\n", T->last_child->last_child->last_child->key);
	printf("%d\n", T->last_child->last_child->last_child->last_child->key);
	printf("%d %d %d %d\n", T->last_child->last_child->last_child->left_sibling->key, T->last_child->last_child->left_sibling->key, T->last_child->last_child->left_sibling->last_child->key, T->last_child->last_child->left_sibling->left_sibling->key);
	print_heap(T);
	printf("**********\n%d\n**********\n", Min(T));
	printf("%d\n", (RemoveMin(&T))->key);
	print_heap(T);
	printf("--------------\n");
	printf("%d\n", (RemoveMin(&T))->key);
	print_heap(T);
	printf("--------------\n");
	printf("%d\n", (RemoveMin(&T))->key);
	print_heap(T);
	printf("--------------\n");
	printf("%d\n", (RemoveMin(&T))->key);
	print_heap(T);
	printf("--------------\n");*/
	print_heap(T);
	//testing for parent
	/*printf("%d %d\n", T->first_child->key, T->first_child->parent==NULL);
	printf("%d %d %d %d\n", T->first_child->right_sibling->key, T->first_child->right_sibling->first_child->key, T->first_child->right_sibling->first_child->right_sibling->key, T->first_child->right_sibling->first_child->right_sibling->first_child->key);
	printf("%d %d %d %d\n", T->first_child->right_sibling->parent == NULL, T->first_child->right_sibling->first_child->parent->key, T->first_child->right_sibling->first_child->right_sibling->parent->key, T->first_child->right_sibling->first_child->right_sibling->first_child->parent->key);
	HeapNode* tree_node = T->first_child->right_sibling->right_sibling;
	printf("%d %d %d %d %d %d %d %d\n", tree_node->parent == NULL, tree_node->first_child->parent->key, tree_node->first_child->right_sibling->parent->key, tree_node->last_child->parent->key, tree_node->first_child->right_sibling->first_child->parent->key, tree_node->last_child->first_child->parent->key, tree_node->last_child->last_child->parent->key, tree_node->last_child->last_child->first_child->parent->key);*/
	//testing for seq_num
	/*printf("\n");
	HeapNode* tree_node = T->first_child->right_sibling->right_sibling;
	printf("%d %d %d %d %d %d %d %d\n", tree_node->seq_num, tree_node->first_child->seq_num, tree_node->first_child->right_sibling->seq_num, tree_node->last_child->seq_num, tree_node->first_child->right_sibling->first_child->seq_num, tree_node->last_child->first_child->seq_num, tree_node->last_child->last_child->seq_num, tree_node->last_child->last_child->first_child->seq_num);
*/
	//testing for update key
	HeapNode* tree_node = T->first_child->right_sibling->right_sibling;
	tree_node->key = -1;
	update_key(T, tree_node->last_child->last_child, 0);
	print_heap(T);
	printf("%d %d %d %d\n", tree_node->first_child->right_sibling->right_sibling->key, tree_node->last_child->key, tree_node->last_child->first_child->right_sibling->key, tree_node->last_child->last_child->key);
	printf("%d %d\n", tree_node->last_child->last_child->first_child->key, tree_node->last_child->last_child->last_child->key);
	printf("%d %d %d %d\n", tree_node->last_child->parent == tree_node, tree_node->last_child->first_child->parent == tree_node->last_child, tree_node->last_child->last_child->parent == tree_node->last_child, tree_node->last_child->last_child->last_child->parent == tree_node->last_child->last_child);
}

//By Yu Liu, z5190675
