#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// all the basic data structures and functions are included in this template
// you can add your own auxiliary functions as you like 

// data type for avl tree nodes
typedef struct AVLTreeNode {
	int key; //key of this item
	int  value;  //value (int) of this item 
	int height; //height of the subtree rooted at this node
	struct AVLTreeNode *parent; //pointer to parent
	struct AVLTreeNode *left; //pointer to left child
	struct AVLTreeNode *right; //pointer to right child
} AVLTreeNode;

//data type for AVL trees
typedef struct AVLTree {
	int  size;      // count of items in avl tree
	AVLTreeNode *root; // root
} AVLTree;

AVLTreeNode *newAVLTreeNode(int k, int v);
AVLTree *newAVLTree();
int abs_difference(int a, int b);
void rebalance(AVLTreeNode* x, AVLTreeNode* y, AVLTreeNode* z, AVLTreeNode* a, AVLTreeNode* b, AVLTreeNode*c);
AVLTree *CreateAVLTree(const char *filename);
void clone_node(AVLTreeNode* original_node, AVLTreeNode* new_node, AVLTree* new_tree);
AVLTree *CloneAVLTree(AVLTree *T);
int in_order_traverse(AVLTreeNode* current_node, int* arr, int cnt, int mode);
AVLTreeNode* extend_to_tree(int* arr_key, int* arr_value, int start, int end);
AVLTree *AVLTreesUnion(AVLTree *T1, AVLTree *T2);
AVLTree *AVLTreesIntersection(AVLTree *T1, AVLTree *T2);
int InsertNode(AVLTree *T, int k, int v);
int DeleteNode(AVLTree *T, int k, int v);
AVLTreeNode *Search(AVLTree *T, int k, int v);
void free_node(AVLTreeNode* current_node);
void FreeAVLTree(AVLTree *T);
void print_component(AVLTreeNode* current_node);
void PrintAVLTree(AVLTree *T);

// create a new AVLTreeNode
AVLTreeNode *newAVLTreeNode(int k, int v)
{
	AVLTreeNode *new;
	new = malloc(sizeof(AVLTreeNode));
	assert(new != NULL);
	new->key = k;
	new->value = v;
	new->height = 0; // height of this new node is set to 0
	new->left = NULL; // this node has no child
	new->right = NULL;
	new->parent = NULL; // no parent
	return new;
}

// create a new empty avl tree
AVLTree *newAVLTree()
{
	AVLTree *T;
	T = malloc(sizeof(AVLTree));
	assert(T != NULL);
	T->size = 0;
	T->root = NULL;
	return T;
}

//self-written function for compare the abs difference between two integers
int abs_difference(int a, int b)
{
	return (a > b) ? (a - b) : (b - a);
}

//self-written function for rebalance the AVL Tree under four conditions
void rebalance(AVLTreeNode* x, AVLTreeNode* y, AVLTreeNode* z, AVLTreeNode* a, AVLTreeNode* b, AVLTreeNode*c)
{
	if (a==z && b==y && c==x)//condition 1
	{
		//modify the height and the parent-child relation
		AVLTreeNode* t0 = a->left;
		AVLTreeNode* t1 = b->left;
		AVLTreeNode* t2 = c->left;
		AVLTreeNode* t3 = c->right;
		int height_0 = (t0) ? t0->height : -1;
		int height_1 = (t1) ? t1->height : -1;
		int height_2 = (t2) ? t2->height : -1;
		int height_3 = (t3) ? t3->height : -1;
		a->height = ((height_0 > height_1) ? height_0 : height_1) + 1;
		c->height = ((height_2 > height_3) ? height_2 : height_3) + 1;
		b->height = ((a->height > c->height) ? a->height : c->height) + 1;
		b->left = a;
		b->right = c;
		a->parent = b;
		c->parent = b;
		a->right = t1;
		if (t1)
		{
			t1->parent = a;
		}
	}
	else if (a==x && b==y && c==z)//condition 2
	{
		//modify the height and the parent-child relation
		AVLTreeNode* t0 = a->left;
		AVLTreeNode* t1 = a->right;
		AVLTreeNode* t2 = b->right;
		AVLTreeNode* t3 = c->right;
		int height_0 = (t0) ? t0->height : -1;
		int height_1 = (t1) ? t1->height : -1;
		int height_2 = (t2) ? t2->height : -1;
		int height_3 = (t3) ? t3->height : -1;
		a->height = ((height_0 > height_1) ? height_0 : height_1) + 1;
		c->height = ((height_2 > height_3) ? height_2 : height_3) + 1;
		b->height = ((a->height > c->height) ? a->height : c->height) + 1;
		b->left = a;
		b->right = c;
		a->parent = b;
		c->parent = b;
		c->left = t2;
		if (t2)
		{
			t2->parent = c;
		}
	}
	else if (a==z && b==x && c==y)//condition 3
	{
		//modify the height and the parent-child relation
		AVLTreeNode* t0 = a->left;
		AVLTreeNode* t1 = b->left;
		AVLTreeNode* t2 = b->right;
		AVLTreeNode* t3 = c->right;
		int height_0 = (t0) ? t0->height : -1;
		int height_1 = (t1) ? t1->height : -1;
		int height_2 = (t2) ? t2->height : -1;
		int height_3 = (t3) ? t3->height : -1;
		a->height = ((height_0 > height_1) ? height_0 : height_1) + 1;
		c->height = ((height_2 > height_3) ? height_2 : height_3) + 1;
		b->height = ((a->height > c->height) ? a->height : c->height) + 1;
		b->left = a;
		b->right = c;
		a->parent = b;
		c->parent = b;
		a->right = t1;
		c->left = t2;
		if (t1)
			t1->parent = a;
		if (t2)
			t2->parent = c;
	}
	else if (a==y && b==x && c==z)//condition 4
	{
		//modify the height and the parent-child relation
		AVLTreeNode* t0 = c->right;
		AVLTreeNode* t1 = b->right;
		AVLTreeNode* t2 = b->left;
		AVLTreeNode* t3 = a->left;
		b->left = a;
		b->right = c;
		a->parent = b;
		c->parent = b;
		int height_0 = (t0) ? t0->height : -1;
		int height_1 = (t1) ? t1->height : -1;
		int height_2 = (t2) ? t2->height : -1;
		int height_3 = (t3) ? t3->height : -1;
		a->height = ((height_2 > height_3) ? height_2 : height_3) + 1;
		c->height = ((height_0 > height_1) ? height_0 : height_1) + 1;
		b->height = ((a->height > c->height) ? a->height : c->height) + 1;
		a->right = t2;
		c->left = t1;
		if (t1)
			t1->parent = c;
		if (t2)
			t2->parent = a;
	}
}

// put your time complexity analysis of CreateAVLTree() here
/*
 This function calls the InsertNode function, which, as shown in the InsertNode part, has the time complexity of O(logn).
<--Assume there are n elements to be inserted from the file or the stdin, and the average length of each element in a, so there are overall a*n characters--->

The simplified structure of this function is:

Function:
	get input from the file or stdin, char by char: (Loop, run a*n times)
		after getting a pair of (key, value), call the InsertNode function (Time: O(logn))

So the overall time of this function is: a*n*O(logn), which means that the time complexity of this function is O(nlogn), in which n is the size of the AVL tree.
 */
AVLTree *CreateAVLTree(const char *filename)
{
	// put your code here
	AVLTree* new_tree = newAVLTree();
	//get input from the file or stdin, with multiple input format error detection judgments
	FILE* fp;
	if (strcmp(filename, "stdin") == 0)//read from input
	{
		fp = stdin;
	}
	else
	{
		fp = fopen(filename, "r");
	}
	//read from file pointer
	char temp_line[120];
	while (fgets(temp_line, 120, fp))
	{
		if (strcmp(temp_line, "\n") == 0)//modified 20190321
			break;//modified 20190321
		int left_par = 0;
		int current_key = 0;
		int current_key_flag = 0;//new number get
		int current_value = 0;
		int current_value_flag = 0;//new number get
		int current_position = 0;//0 is for key and 1 is for value
		int mul = 1;
		int cnt = 0;
		char current_char;
		while (cnt<strlen(temp_line))// && *(temp_line+cnt) != '\n'
		{
			current_char = *(temp_line + cnt);
			switch (current_char)
			{
			case '(':
				if (left_par)//in case two '(' occurs without ')' in between
				{
					printf("Input format error, multiple '(' included without ')' in between.\n");
					exit(1);
				}
				left_par++;
				break;
			case '-':
				if (!left_par)//in case no '(' occurs before '-'
				{
					printf("Input format error, no '(' shown before '-'.\n");
					exit(1);
				}
				if (mul != 1)//in case two continuous '-' shown, like --3
				{
					printf("Input format error, continuous two '-' shown.\n");
					exit(1);
				}
				if (current_position == 0)
				{
					if (current_key_flag)//in case '-' occurs between two nums, like 3-6
					{
						printf("Input format error, '-' occurs between two numbers.\n");
						exit(1);
					}
				}
				if (current_position == 1)
				{
					if (current_value_flag)//in case '-' occurs between two nums, like 3-6
					{
						printf("Input format error, '-' occurs between two numbers.\n");
						exit(1);
					}
				}
				mul = -1;
				break;
			case ',':
				if (!left_par)//in case no '(' shown before the number
				{
					printf("Input format error, no '(' shown before.\n");
					exit(1);
				}
				if (current_position)//in case there are multiple numbers in a pair, like (36,15,16)
				{
					printf("Input format error, multiple numbers in a pair.\n");
					exit(1);
				}
				current_position = 1;
				if (!current_key_flag)//in case there is no first number in a pair, like (,36)
				{
					printf("Input format error, no first num in a pair occurs.\n");
					exit(1);
				}
				current_key = current_key * mul;
				mul = 1;
				break;
			case ')':
				if (!left_par)//in case no '(' shown before ')', like 15,16)
				{
					printf("Input format error, no '(' shown before ')'.\n");
					exit(1);
				}
				if (!current_position || !current_key_flag || !current_value_flag)//in case not enough numbers in a pair
				{
					printf("Input format error, not enough numbers in a pair.\n");
					exit(1);
				}
				current_value = current_value * mul;
				// printf("Checking point, %d, %d\n", current_key, current_value);
				//insert the node here, (current_key, current_value)
				InsertNode(new_tree, current_key, current_value);
				//
				left_par = 0;
				current_position = 0;
				current_key_flag = 0;
				current_value_flag = 0;
				current_key = 0;
				current_value = 0;
				mul = 1;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (!left_par)//in case no '(' occurs before number
				{
					printf("Input format error, no '(' shown before number.\n");
					exit(1);
				}
				if (current_position == 0)
				{
					current_key = current_key * 10 + (current_char - '0');
					current_key_flag = 1;
				}
				else
				{
					current_value = current_value * 10 + (current_char - '0');
					current_value_flag = 1;
				}
				break;
			case ' ':
			case '\r':
			case '\n':  // modified 20190321
				break;
			default://invalid characters shown
				printf("Input format error, invalid character shown.\n");
				exit(1);
				break;
			}
			cnt++;
		}
	}
	return new_tree;
}

//self-written function for cloning nodes
void clone_node(AVLTreeNode* original_node, AVLTreeNode* new_node, AVLTree* new_tree)
{
	if (original_node->left)
	{
		AVLTreeNode* new_left_node = newAVLTreeNode(original_node->left->key, original_node->left->value);
		new_node->left = new_left_node;
		new_left_node->parent = new_node;
		new_left_node->height = original_node->left->height;
		new_tree->size++;
		clone_node(original_node->left, new_left_node, new_tree);
	}
	if (original_node->right)
	{
		AVLTreeNode* new_right_node = newAVLTreeNode(original_node->right->key, original_node->right->value);
		new_node->right = new_right_node;
		new_right_node->parent = new_node;
		new_right_node->height = original_node->right->height;
		new_tree->size++;
		clone_node(original_node->right, new_right_node, new_tree);
	}
	return;
}

// put your time complexity analysis for CloneAVLTree() here
/*
 This function calls the clone_node function.
<--Assume the size of the AVL tree is n--->

Because this function traverses every node in the AVL tree exactly one time, and the operation on each node takes constant time.
The time complexity of this function is O(n), in which n is the size of the AVL tree.
 */
AVLTree *CloneAVLTree(AVLTree *T)
{
	// put your code here
	AVLTree* new_tree = newAVLTree();
	if (!T->size)
		return new_tree;
	AVLTreeNode* current_original = T->root;
	AVLTreeNode* current_new = newAVLTreeNode(current_original->key, current_original->value);
	current_new->height = current_original->height;
	new_tree->root = current_new;
	new_tree->size++;
	clone_node(current_original, current_new, new_tree);
	return new_tree;
}


// self-written function used for in-order traversing the tree and save the result in an array
int in_order_traverse(AVLTreeNode* current_node, int* arr, int cnt, int mode)
{
	if (!current_node)
		return cnt;
	if (!mode)  // for key
	{
		int temp = in_order_traverse(current_node->left, arr, cnt,0);
		*(arr + temp) = current_node->key;
		int new_temp = in_order_traverse(current_node->right, arr, temp + 1,0);
		return new_temp;
	}
	else  // for value
	{
		int temp = in_order_traverse(current_node->left, arr, cnt,1);
		*(arr + temp) = current_node->value;
		int new_temp = in_order_traverse(current_node->right, arr, temp + 1,1);
		return new_temp;
	}
}


//self-written function used for recursively convert a sorted array to an AVL Tree
AVLTreeNode* extend_to_tree(int* arr_key, int* arr_value, int start, int end)
{
	int length = end - start + 1;
	if (length == 1)
	{
		AVLTreeNode* new_created = newAVLTreeNode(*(arr_key + start), *(arr_value + start));
		new_created->height = 0;
		return new_created;
	}
	if (length <= 0)
		return NULL;
	int mid = length / 2;
	AVLTreeNode* subroot = newAVLTreeNode(*(arr_key + start + mid), *(arr_value + start + mid));
	AVLTreeNode* left_subtree = extend_to_tree(arr_key, arr_value, start, start+mid - 1);
	AVLTreeNode* right_subtree = extend_to_tree(arr_key, arr_value, start+mid + 1, end);
	subroot->left = left_subtree;
	subroot->right = right_subtree;
	int left_height = (left_subtree) ? left_subtree->height : -1;
	int right_height = (right_subtree) ? right_subtree->height : -1;
	subroot->height = (left_height > right_height) ? (left_height + 1) : (right_height + 1);
	if (left_subtree)
		left_subtree->parent = subroot;
	if (right_subtree)
		right_subtree->parent = subroot;
	return subroot;
}

// put your time complexity for ALVTreesUNion() here
/*
 <---Assume the first tree has m nodes and the second tree has n nodes--->

The simplified structure of this function is:

Function:
	if either tree is empty: (Time: constant)
		return the other tree (Time: constant)
	in-order traverse the first tree and store the elements in an array (Time: O(m) )
	in-order traverse the second tree and store the elements in an array (Time: O(n) )
	combine two pairs of arrays into a new pair, with non-descending order, with each element occurs only once (Time: O(m+n) )
	recursively convert the new_created pair of array into an AVL Tree (Time: O(m+n) )

Explain: In the in-order traverse, each node in the tree is visited exactly once, so the time complexity of traversing the tree is O(n).
While combining two sorted arrays into a new one, in the worst case, all elements are visited exactly once, so time complexity is O(m+n).
While recursively convert the sorted array into an AVL Tree, each element is visited exactly once and operation of each element needs constant time, so
the time complexity of this is O(m+n).

So the overall time of this function is O(m)+O(n)+O(m+n)+O(m+n), which means that the time complexity is O(m+n).
 */
AVLTree *AVLTreesUnion(AVLTree *T1, AVLTree *T2)
{
	// check whether the tree is empty
	if (!T1->size)
		return CloneAVLTree(T2);
	if (!T2->size)
		return CloneAVLTree(T1);
	// save the elements of each tree in the array (in-order)
	int *arr1_key = (int *)malloc(T1->size * sizeof(int));
	int* arr1_value = (int *)malloc(T1->size * sizeof(int));
	int *arr2_key = (int *)malloc(T2->size * sizeof(int));
	int *arr2_value = (int *)malloc(T2->size * sizeof(int));
	int *arr_key = (int *)malloc((T1->size + T2->size) * sizeof(int));
	int *arr_value = (int *)malloc((T1->size + T2->size) * sizeof(int));
	in_order_traverse(T1->root, arr1_key, 0,0);
	in_order_traverse(T1->root, arr1_value, 0,1);
	in_order_traverse(T2->root, arr2_key, 0, 0);
	in_order_traverse(T2->root, arr2_value, 0, 1);
	//combine two pairs of arrays into a new pair, with non-descending order, with each element occurs only once
	int cnt1 = 0;
	int cnt2 = 0;
	int cnt = 0;
	int duplicate_num = 0;
	while (cnt1<T1->size && cnt2<T2->size)
	{
		if (*(arr1_key+cnt1) < *(arr2_key+cnt2))
		{
			*(arr_key + cnt) = *(arr1_key + cnt1);
			*(arr_value + cnt) = *(arr1_value + cnt1);
			cnt1++;
			cnt++;
		}
		else if (*(arr1_key + cnt1) > *(arr2_key + cnt2))
		{
			*(arr_key + cnt) = *(arr2_key + cnt2);
			*(arr_value + cnt) = *(arr2_value + cnt2);
			cnt2++;
			cnt++;
		}
		else
		{
			if (*(arr1_value + cnt1) < *(arr2_value + cnt2))
			{
				*(arr_key + cnt) = *(arr1_key + cnt1);
				*(arr_value + cnt) = *(arr1_value + cnt1);
				cnt1++;
				cnt++;
			}
			else if (*(arr1_value + cnt1) > *(arr2_value + cnt2))
			{
				*(arr_key + cnt) = *(arr2_key + cnt2);
				*(arr_value + cnt) = *(arr2_value + cnt2);
				cnt2++;
				cnt++;
			}
			else
			{
				*(arr_key + cnt) = *(arr1_key + cnt1);
				*(arr_value + cnt) = *(arr1_value + cnt1);
				cnt1++;
				cnt2++;
				cnt++;
				duplicate_num++;
			}
		}
	}
	if (cnt1 < T1->size)
	{
		for (int i = cnt1;i<T1->size;i++)
		{
			*(arr_key + cnt) = *(arr1_key + i);
			*(arr_value + cnt) = *(arr1_value + i);
			i++;
			cnt++;
		}
	}
	if (cnt2 < T2->size)
	{
		for (int i = cnt2; i < T2->size; i++)
		{
			*(arr_key + cnt) = *(arr2_key + i);
			*(arr_value + cnt) = *(arr2_value + i);
			i++;
			cnt++;
		}
	}
	// change the new created array pair into AVL Tree directly
	int new_length = (T1->size + T2->size) - duplicate_num;
	AVLTree* new_created_tree = newAVLTree();
	AVLTreeNode *new_root = extend_to_tree(arr_key, arr_value, 0, new_length - 1);
	new_created_tree->root = new_root;
	new_created_tree->size = new_length;
	return new_created_tree;
}


// put your time complexity for ALVTreesIntersection() here
/*
 <---Assume there are m elements in the first tree and n elements in the second tree--->

The simplified structure of this function is:

Function:
	if at least one tree is empty: (Time: constant)
		return NULL (Time: constant)
	save the elements of the first tree into a pair of array with in-order (Time: O(m) )
	save the elements of the second tree into a pair of array with in-order (Time: O(n) )
	combine two pairs of array into a new pair of array, with non-descending order, with only intersection elements (Time: O(m+n) )
	convert the new created array pair into AVL Tree directly (Time: worst: O(min(m,n)) )

Explain: In the in-order traverse, each node in the tree is visited exactly once, so the time complexity of traversing the tree is O(n).
While combining two sorted arrays into a new one, in the worst case, all elements are visited exactly once, so time complexity is O(m+n).
In the worst case, the number of elements in the intersection array is min(m,n), that is, the tree with minimum element number is a subset of another tree.
While recursively convert the sorted array into an AVL Tree, each element is visited exactly once and operation of each element needs constant time, so
the time complexity of this is O(min(m,n)).

So the overall time of this function is O(m)+O(n)+O(m+n)+O(min(m,n)), which means that the time complexity is O(m+n).
 */
AVLTree *AVLTreesIntersection(AVLTree *T1, AVLTree *T2)
{
	// check whether the tree is empty
	if (!T1->size)
		return NULL;
	if (!T2->size)
		return NULL;
	// save the elements of each tree in the array (in-order)
	int *arr1_key = (int *)malloc(T1->size * sizeof(int));
	int* arr1_value = (int *)malloc(T1->size * sizeof(int));
	int *arr2_key = (int *)malloc(T2->size * sizeof(int));
	int *arr2_value = (int *)malloc(T2->size * sizeof(int));
	int *arr_key = (int *)malloc((T1->size + T2->size) * sizeof(int));
	int *arr_value = (int *)malloc((T1->size + T2->size) * sizeof(int));
	in_order_traverse(T1->root, arr1_key, 0, 0);
	in_order_traverse(T1->root, arr1_value, 0, 1);
	in_order_traverse(T2->root, arr2_key, 0, 0);
	in_order_traverse(T2->root, arr2_value, 0, 1);
	//combine two pairs of arrays into a new pair, with non-descending order, with only intersection elements
	int cnt1 = 0;
	int cnt2 = 0;
	int cnt = 0;
	int duplicate_num = 0;
	while (cnt1 < T1->size && cnt2 < T2->size)
	{
		if (*(arr1_key + cnt1) < *(arr2_key + cnt2))
		{
			cnt1++;
		}
		else if (*(arr1_key + cnt1) > *(arr2_key + cnt2))
		{
			cnt2++;
		}
		else
		{
			if (*(arr1_value + cnt1) < *(arr2_value + cnt2))
			{
				cnt1++;
			}
			else if (*(arr1_value + cnt1) > *(arr2_value + cnt2))
			{
				cnt2++;
			}
			else
			{
				*(arr_key + cnt) = *(arr1_key + cnt1);
				*(arr_value + cnt) = *(arr1_value + cnt1);
				cnt1++;
				cnt2++;
				cnt++;
				duplicate_num++;
			}
		}
	}
	// change the new created array pair into AVL Tree directly
	int new_length = duplicate_num;
	AVLTree* new_created_tree = newAVLTree();
	AVLTreeNode *new_root = extend_to_tree(arr_key, arr_value, 0, new_length - 1);
	new_created_tree->root = new_root;
	new_created_tree->size = new_length;
	return new_created_tree;
}


// put the time complexity analysis for InsertNode() here
/*
 <---Assume the size of this AVL tree is n--->

 The simplified structure of this function is:

Function:
	create a new node with the certain key and value (Time: constant)
	if the tree is empty: (Time: constant)
		insert the node into the root of the tree (Time: constant)
	else:
		insert the node as in the general binary search tree (Time: O(logn) )
		go through the path back, modify the height of the new added node's ancestors, check the AVL condition, find the node x, y, and z (Time: worst O(logn) )
		rebalance, calls the rebalance function, which takes constant time (Time: constant)
		go through the path back from the violate node and modify the height and child-parent relation of its ancestor (Time: worst O(logn)

So the worst overall time is: O(1)+O(logn)+O(logn)+O(1)+O(logn), which means that the time complexity of this function is O(logn).
 */
int InsertNode(AVLTree *T, int k, int v)
{
	//put your code here
	AVLTreeNode* new_node = newAVLTreeNode(k, v);
	if (T->size == 0)//the tree is empty
	{
		T->size++;
		T->root = new_node;
	}
	else//the tree is not empty
	{
		//first insert the node as in the general binary search tree
		AVLTreeNode* current_node = T->root;
		T->size++;
		AVLTreeNode* last_node = NULL;
		while (current_node)
		{
			last_node = current_node;
			if (current_node->key == k && current_node->value == v)
				return 0;
			if (current_node->key > k)
				current_node = current_node->left;
			else if (current_node->key < k)
				current_node = current_node->right;
			else
			{
				if (current_node->value > v)
					current_node = current_node->left;
				else
					current_node = current_node->right;
			}
		}
		if (last_node->key > k || (last_node->key == k  && last_node->value>v))
			last_node->left = new_node;
		else if (last_node->key<k || (last_node->key == k && last_node->value<v))
			last_node->right = new_node;
		new_node->parent = last_node;
		//then get the path back, update the height of its ancestors, check AVL condition, and find x, y, z
		//reuse current_node
		int cnt = 0;
		current_node = new_node->parent;
		if (current_node->left && current_node->right)//no need to update the height and no need to rebalance, right?
		{
			//no need to update the height and no need to rebalance because the formal tree is AVL, and the insertion of this node has no influence of the height of other nodes
			;
		}
		else
		{
			AVLTreeNode* violate_node = NULL;
			int violate_flag = 0;
			while (current_node)
			{
				current_node->height++;//modify the height
				//judge the height of two children
				int left_height = (current_node->left) ? current_node->left->height : -1;//modify 20190319, 0->-1
				int right_height = (current_node->right) ? current_node->right->height : -1;//modify 20190319, 0->-1
				if (abs_difference(left_height, right_height) > 1 && !violate_flag)
				{
					violate_node = current_node;
					violate_flag = 1;
				}		
				current_node = current_node->parent;
			}
			if (!violate_node)
			{
				//no violate node, no need to rebalance
				// if (violate_node)
				// 	printf("CHECKING POINT, violate node: %d, %d\n", violate_node->key, violate_node->value);
				// else
				// 	printf("CHECKING POINT, violate node is NULL\n");
			}
			else
			{
				//need to find the x, y, z
				//the violate_node is the highest node that violates the AVL condition, so this node is z
				AVLTreeNode* z = violate_node;
				int left_height_of_violate = (violate_node->left) ? violate_node->left->height : -1;//modify 20190319, 0->-1
				int right_height_of_violate = (violate_node->right) ? violate_node->right->height : -1;//modify 20190319, 0->-1
				AVLTreeNode* y = (left_height_of_violate > right_height_of_violate) ? violate_node->left : violate_node->right;
				int left_height_of_y = (y->left) ? y->left->height : -1;//modify 20190319, 0->-1
				int right_height_of_y = (y->right) ? y->right->height : -1;//modify 20190319, 0->-1
				AVLTreeNode* x = (left_height_of_y > right_height_of_y) ? y->left : y->right;
				//find a, b, c from these three nodes
				AVLTreeNode *a = x;
				AVLTreeNode *b = y;
				AVLTreeNode *c = z;
				AVLTreeNode* temp;
				if (b->key > c->key || (b->key == c->key && b->value > c->value))
				{
					b = z;
					c = y;
				}
				if (a->key > b->key || (a->key == b->key && a->value > b->value))
				{
					temp = a;
					a = b;
					b = temp;
				}
				if (b->key > c->key || (b->key == c->key && b->value>c->value))
				{
					temp = b;
					b = c;
					c = temp;
				}
				//rebalance
				//After rebalancing, b is the top one of the three
				AVLTreeNode* violate_parent = violate_node->parent;
				rebalance(x, y, z, a, b, c);
				if (!violate_parent)
				{
					T->root = b;
					b->parent = NULL;
				}
				else
				{
					if (b->key < violate_parent->key || (b->key == violate_parent->key && b->value < violate_parent->value))
						violate_parent->left = b;
					else
						violate_parent->right = b;
					b->parent = violate_parent;
					int c = b->height;
					while(violate_parent)
					{
						violate_parent->height = c + 1;
						c++;
						violate_parent = violate_parent->parent;//remember to add this on delete
					}
				}
			}
		}
	}
	return 1;
}

// put your time complexity for DeleteNode() here
/*
 This function calls the rebalance function.

<---Assume there are n elements in the tree--->

The simplified structure of this function is:

Function:
	find the node and remember its parent node (Time: worst O(logn) )
	if it has two children:
		find the successor and delete the successor (the overall time of this and the last operation is O(logn) because it traverses the height of the tree)
	else:
		delete this node and let its child (if it has) replace its position (Time: constant)
	go up from the deleted node through the path, modify the height of its ancestors, check the AVL condition and find nodes x, y, and z (Time: O(logn) )
	rebalace (Time: constant)
	remodify the height of the ancestors of the violate node (Time: worst O(logn) )

So the worst overall time is: O(logn)+O(logn)+O(1)+O(logn), which means the time complexity of this function is O(logn).
 */
int DeleteNode(AVLTree *T, int k, int v)
{
	// put your code here
	if (!T->size)
		return 0;
	//first, find the node and remember its parent
	AVLTreeNode* current_node = T->root;
	AVLTreeNode* last_node = NULL;
	char direction;
	while(current_node)
	{
		if (current_node->key == k && current_node->value == v)
			break;
		last_node = current_node;
		if (current_node->key > k || (current_node->key == k && current_node->value > v))
		{
			current_node = current_node->left;
			direction = 'L';
		}	
		else
		{
			current_node = current_node->right;
			direction = 'R';
		}
	}
	if (!current_node)//not found the certain node with key k and value v
		return 0;
	T->size--;
	//after finding the current node, check if it has less than or one child or if it has two children
	if (current_node->left && current_node->right)//has two children
	{
		//find the successor node, copy the successor node's (key, value) pair and delete the successor
		//also need to get the parent of the deleted node, go up to root to change its height and find whether it violates the AVL condition
		AVLTreeNode* successor = current_node->right;
		AVLTreeNode* last_of_successor = current_node;
		while(successor)
		{
			last_of_successor = successor;
			successor = successor->left;
		}
		printf("%d %d\n", last_of_successor->key, last_of_successor->value);//modify 20190319, testing, find errors of the success, last_of_successor pointers
		current_node->key = last_of_successor->key;//modify 20190319, testing
		current_node->value = last_of_successor->value;//modify 20190319, testing
		AVLTreeNode* last_of_last_successor = last_of_successor->parent;//modify 20190319, testing, new added variable
		if (last_of_last_successor == current_node)//modify 20190319, testing
		{
			last_of_last_successor->right = NULL;//modify 20190319, testing
		}
		else
			last_of_last_successor->left = NULL;//modify 20190319, testing
		free(last_of_successor);//modify 20190319, testing
		last_node = last_of_last_successor;//modify 20190319, testing
	}
	else//has one or no child
	{
		//delete the current_node, if it has a child, then let the child replace it
		//go from the parent, change the height and check AVL condition
		if (!last_node)//the node to be deleted in the root
		{
			if (current_node->left)
			{
				current_node->left->parent = NULL;
				T->root = current_node->left;
			}
			else if (current_node->right)
			{
				current_node->right->parent = NULL;
				T->root = current_node->right;
			}
			else
				T->root = NULL;
		}
		else
		{
			//first delete the target node
			if (current_node->left)
			{
				if (direction == 'L')
					last_node->left = current_node->left;
				else
					last_node->right = current_node->left;
				current_node->left->parent = last_node;
			}
			else if (current_node->right)
			{
				if (direction == 'L')
					last_node->left = current_node->right;
				else
					last_node->right = current_node->right;
				current_node->right->parent = last_node;
			}
			else
			{
				if (direction == 'L')
					last_node->left = NULL;
				else
					last_node->right = NULL;
			}
			free(current_node);
		}
	}//modify 20190319, testing, restructure, take the remaining part out of the else part above
	//then go up modifying the height, and check the AVL condition, finding the violate node
	//reuse current node
	current_node = last_node;
	AVLTreeNode* violate_node=NULL;
	int violate_flag = 0;
	while (current_node)
	{
		int left_height = (current_node->left) ? current_node->left->height : -1;//modify 20190319, 0->-1
		int right_height = (current_node->right) ? current_node->right->height : -1;//modify 20190319, 0->-1
		if (!current_node->left && !current_node->right)
			current_node->height = 0;
		else
			current_node->height = ((left_height > right_height) ? left_height : right_height) + 1;
		if (abs_difference(left_height, right_height) > 1 && !violate_flag)
		{
			violate_node = current_node;
			violate_flag = 1;
		}
		// current_node->height = (left_height > right_height) ? left_height : right_height;
		current_node = current_node->parent;
	}
	if (!violate_node)
	{
		//no violate node, no need to rebalance
		;
	}
	else
	{
		//need to find the x, y, z
		//the violate_node is the highest node that violates the AVL condition, so this node is z
		AVLTreeNode* z = violate_node;
		int left_height_of_violate = (violate_node->left) ? violate_node->left->height : -1;//modify 20190319, 0->-1
		int right_height_of_violate = (violate_node->right) ? violate_node->right->height : -1;//modify 20190319, 0->-1
		AVLTreeNode* y = (left_height_of_violate > right_height_of_violate) ? violate_node->left : violate_node->right;
		int left_height_of_y = (y->left) ? y->left->height : -1;//modify 20190319, 0->-1
		int right_height_of_y = (y->right) ? y->right->height : -1;//modify 20190319, 0->-1
		AVLTreeNode* x = (left_height_of_y > right_height_of_y) ? y->left : y->right;
		//find a, b, c from these three nodes
		AVLTreeNode *a = x;
		AVLTreeNode *b = y;
		AVLTreeNode *c = z;
		AVLTreeNode* temp;
		if (b->key > c->key || (b->key == c->key && b->value > c->value))
		{
			b = z;
			c = y;
		}
		if (a->key > b->key || (a->key == b->key && a->value > b->value))
		{
			temp = a;
			a = b;
			b = temp;
		}
		if (b->key > c->key || (b->key == c->key && b->value > c->value))
		{
			temp = b;
			b = c;
			c = temp;
		}
		//rebalance
		//After rebalancing, b is the top one of the three
		AVLTreeNode* violate_parent = violate_node->parent;
		rebalance(x, y, z, a, b, c);
		if (!violate_parent)
		{
			T->root = b;
			b->parent = NULL;
		}
		else
		{
			if (b->key < violate_parent->key || (b->key == violate_parent->key && b->value < violate_parent->value))
				violate_parent->left = b;
			else
				violate_parent->right = b;
			b->parent = violate_parent;
			int c = b->height;
			while (violate_parent)
			{
				violate_parent->height = c + 1;
				c++;
				violate_parent = violate_parent->parent;//remember to add this on delete
			}
		}
	}
	return 1;
}

// put your time complexity analysis for Search() here
/*
 <---Assume the size of the tree is n--->

The simplified structure of this function is:

This function acts similar as the search in general binary search tree, but this function first judge the key, if key is the same, it then judge the value

Function:
	if the tree is empty:
		return NULL
	else:
		while current_node is not NULL: (Loop, run log(n) times)
			if current_node->key > target_key or (current_node->key == target_key and current_node->value > target_value):
				go left
			else
				go right

The time complexity of this function is O(logn).
 */
AVLTreeNode *Search(AVLTree *T, int k, int v)
{
	// put your code here
	if (T->size == 0)//the tree is empty
	{
		return NULL;
	}
	else//the tree is not empty
	{
		//first insert the node as in the general binary search tree
		AVLTreeNode* current_node = T->root;
		T->size++;
		AVLTreeNode* last_node = NULL;
		while (current_node)
		{
			last_node = current_node;
			if (current_node->key == k && current_node->value == v)
				return current_node;
			if (current_node->key > k || (current_node->key == k && current_node->value >v))
				current_node = current_node->left;
			else
				current_node = current_node->right;
		}
		return NULL;
	}
}

//self-written function to free node
void free_node(AVLTreeNode* current_node)
{
	if (!current_node)
		return;
	free_node(current_node->left);
	free_node(current_node->right);
	free(current_node);
	return;
}

// put your time complexity analysis for freeAVLTree() here
/*
 The function uses the free_node function.

<---Assume the number of elements in the AVL tree is n--->

Because this function traverses each node exactly once, and the operation on each node takes constant time, so the time complexity is O(n).
 */
void FreeAVLTree(AVLTree *T)
{
	// put your code here	
	if (!T->size)
	{
		free(T);
		return;
	}
	free(T->root);
	return;
}

//self-written function for printing nodes
void print_component(AVLTreeNode* current_node)
{
	if (!current_node)
		return;
	print_component(current_node->left);
	printf("(%d, %d), %d\n ", current_node->key, current_node->value, current_node->height);
	print_component(current_node->right);
}

// put your time complexity analysis for PrintAVLTree() here
/*
 This function uses print_component function.

<---Assume there are n elements in the AVL tree--->

Because this function traverses each node exactly once, and each operation on each node takes constant time, so the time complexity is O(n).
 */
void PrintAVLTree(AVLTree *T)
{
	// put your code here
	print_component(T->root);
}

//self-written function for displaying the tree
void print_binary_nodes(AVLTreeNode* current_node, int cnt)
{
	if (!current_node)
	{
		printf("\n");
		return;
	}
	if (current_node->left)
	{
		print_binary_nodes(current_node->left, cnt + 6);
		for (int i = 0; i < cnt + 1; i++)
			printf(" ");
		printf("/\n");
	}
	for (int i = 0; i < cnt-2; i++)
		printf(" ");
	printf("(%d,%d)\n", current_node->key, current_node->value);
	if (current_node->right)
	{
		for (int i = 0; i < cnt + 1; i++)
			printf(" ");
		printf("\\\n");
		print_binary_nodes(current_node->right, cnt + 6);
	}

}
//self-written function for displaying the tree
void print_binary_tree(AVLTree* current_tree)
{
	print_binary_nodes(current_tree->root, 2);
}
//

int main() //sample main for testing 
{
	int i, j;
	AVLTree *tree1, *tree2, *tree3, *tree4, *tree5, *tree6, *tree7, *tree8;
	AVLTreeNode *node1;

	tree1 = CreateAVLTree("stdin");
	PrintAVLTree(tree1);
	FreeAVLTree(tree1);
	//you need to create the text file file1.txt
	// to store a set of items without duplicate items
	tree2 = CreateAVLTree("file1.txt");
	PrintAVLTree(tree2);
	tree3 = CloneAVLTree(tree2);
	PrintAVLTree(tree3);
	FreeAVLTree(tree2);
	FreeAVLTree(tree3);
	//Create tree4 
	tree4 = newAVLTree();
	j = InsertNode(tree4, 10, 10);
	for (i = 0; i < 15; i++)
	{
		j = InsertNode(tree4, i, i);
		if (j == 0) printf("(%d, %d) already exists\n", i, i);
	}
	PrintAVLTree(tree4);
	node1 = Search(tree4, 20, 20);
	if (node1 != NULL)
		printf("key= %d value= %d\n", node1->key, node1->value);
	else
		printf("Key 20 does not exist\n");

	for (i = 17; i > 0; i--)
	{
		j = DeleteNode(tree4, i, i);
		if (j == 0)
			printf("Key %d does not exist\n", i);
		PrintAVLTree(tree4);
	}
	FreeAVLTree(tree4);
	//Create tree5
	tree5 = newAVLTree();
	j = InsertNode(tree5, 6, 25);
	j = InsertNode(tree5, 6, 10);
	j = InsertNode(tree5, 6, 12);
	j = InsertNode(tree5, 6, 20);
	j = InsertNode(tree5, 9, 25);
	j = InsertNode(tree5, 10, 25);
	PrintAVLTree(tree5);
	//Create tree6
	tree6 = newAVLTree();
	j = InsertNode(tree6, 6, 25);
	j = InsertNode(tree6, 5, 10);
	j = InsertNode(tree6, 6, 12);
	j = InsertNode(tree6, 6, 20);
	j = InsertNode(tree6, 8, 35);
	j = InsertNode(tree6, 10, 25);
	PrintAVLTree(tree6);
	tree7 = AVLTreesIntersection(tree5, tree6);
	tree8 = AVLTreesUnion(tree5, tree6);
	PrintAVLTree(tree7);
	PrintAVLTree(tree8);
	return 0;
}

// z5190675 Liu