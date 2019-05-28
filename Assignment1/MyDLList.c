#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// all the basic data structures and functions are included in this template
// you can add your own auxiliary functions as you like 

// data structures representing DLList

// data type for nodes
typedef struct DLListNode {
	int  value;  // value (int) of this list item 
	struct DLListNode *prev;
	// pointer previous node in list
	struct DLListNode *next;
	// pointer to next node in list
} DLListNode;

//data type for doubly linked lists
typedef struct DLList {
	int  size;      // count of items in list
	DLListNode *first; // first node in list
	DLListNode *last;  // last node in list
} DLList;

// create a new DLListNode
DLListNode *newDLListNode(int it)
{
	DLListNode *new;
	new = malloc(sizeof(DLListNode));
	assert(new != NULL);
	new->value = it;
	new->prev = new->next = NULL;
	return new;
}

// create a new empty DLList
DLList *newDLList()
{
	DLList *L;

	L = malloc(sizeof(struct DLList));
	assert(L != NULL);
	L->size = 0;
	L->first = NULL;
	L->last = NULL;
	return L;
}

//own function
void add_node_at_end(DLListNode* current_node, DLListNode* temp)
{
	while (current_node->next)
	{
		current_node = current_node->next;
	}
	current_node->next = temp;
	temp->prev = current_node;
	//when using this function, need to consider size and last issue
	return;
}

//own function
void add_node_at_end_v2(DLList* current_list, int value)
{
	DLListNode* newone = newDLListNode(value);
	if(current_list->size==0)
	{
		current_list->first = newone;
		current_list->last = newone;
		current_list->size++;
		return;
	}
	current_list->last->next = newone;
	newone->prev = current_list->last;
	current_list->last = newone;
	current_list->size++;
	return;
}

// create a DLList from a text file
// put your time complexity analysis for CreateDLListFromFileDlist() here
/*
 <---Assume there are "n" numbers read from stdin or file, and the average number of chars of each number is "a"--->

 Code in this function also calls the add_node_at_end_v2 function, this called function needs constant time 
because it uses the "last" pointer and simply adds the new created node after it--->

 The simplified structure of this function is: 

 Function:
	check whether to read from stdin or file (Time: constant)
	if get the input from the stdin:
		preparation (Time: constant)
		for each_char from stdin: (Loop, run a*n times)
			get the num and push it in linked list (Time: constant)
	else get the input from the file:
		preparation (Time: constant)
		for each_char from file: (Loop, run a*n times)
			get the num and push it in linked list (Time: constant)

So the overall time is: c+c+a*n*c, which means that the time complexity of this function is O(n), in which n is the number of numbers
*/
DLList *CreateDLListFromFileDlist(const char *filename)
{
	// put your code here
	const char std[6] = "stdin";//is it okay to include another library, that is, <string.h>?
	int cnt = 0;
	for (int i=0;i<6;i++)//check whether the  filename is stdin or not
	{
		if(std[i]==*(filename+i))
		{
			cnt++;
		}
	}
	if (cnt==6)//in this situation, filename is stdin, then get input from stdin
	{
		DLList *new_list = newDLList();//create a empty doubly linked list
		char temp;
		int flag = 0;//this is the flag shows that there has been one \s in the recent read
		int current_value;
		int current_flag = 0;//this is the flag shows that there has already been value stored in current_value
		int mul = 1;
		while (scanf("%c", &temp) != 0)// get the input char by char and store it in temp
		{
			if (temp == ' ' || temp == '\r' || temp == '\n')//if the input is \s
			{
				if (current_flag)
				{
					current_value *= mul;
					mul = 1;
					add_node_at_end_v2(new_list, current_value);
					current_flag = 0;
					current_value = 0;
				}
				if (temp == '\n')
				{
					if (flag) break;//two continuous \n, quit the current loop for reading the input
					else flag++;//no two continuous \n, continue reading the input
				}
			}
			else//the input is a number or a symbol like '+' or '-'
			{
				flag = 0;
				if (temp == '-')
				{
					assert(!current_flag);
					mul *= -1;
				}
				else
				{
					if (temp == '+')
					{
						continue;
					}
					else
					{
						
						if (!current_flag)
						{
							current_flag = 1;
							current_value = temp - '0';
						}
						else
						{
							current_value = (current_value * 10 + (temp - '0'));
						}
					}
				}
				
			}
		}
		return new_list;
	}
	else//in this situation, get the numbers from the file
	{
		DLList *new_list = newDLList();//create a empty doubly linked list
		FILE* f = fopen(filename, "r");//open the file in read only mode
		char temp;
		int flag = 0;//this uses nearly the same method as reading from the stdin
		int current_value;
		int current_flag = 0;
		int mul = 1;
		while (fscanf(f, "%c", &temp) != 0)
		{
			if (temp == ' ' || temp == '\r' || temp == '\n')
			{
				if (current_flag)
				{
					current_value *= mul;
					mul = 1;
					add_node_at_end_v2(new_list,  current_value);
					current_flag = 0;
					current_value = 0;
				}
				if (temp == '\n')
				{
					if (flag) break;
					else flag++;
				}
			}
			else
			{
				flag = 0;
				if (temp == '-')
				{
					assert(!current_flag);
					mul *= -1;
				}
				else
				{
					if (temp == '+')
					{
						continue;
					}
					else
					{
						if (!current_flag)
						{
							current_flag = 1;
							current_value = temp - '0';
						}
						else
						{
							current_value = (current_value * 10 + (temp - '0'));
						}
					}
				}
			}
		}
		return new_list;
	}
}

// clone a DLList
// put your time complexity analysis for cloneList() here
/*
 This function also calls the add_node_at_end_v2 function, and this called function needs constant time.

<---Assume there are n elements in the linked list--->

The simplified structure of this function is:

Function:
	preparation, create new list (Time: constant)
	for each_node in the original list: (Loop, run n times)
		create new node with the same value and add it to new list (Time: constant)
	finish and return (Time: constant)

So the overall time of this function is: c+n*c+c, so the time complexity is O(n), in which n is the number of elements in the list
 */
DLList *cloneList(DLList *u)//I think the nodes of the original one and the clone one are different, or say, independent
{
	// put your code here
	// The following is another method, but I will analyse the time complexity of the uncommented one
	// DLList* clone_list = newDLList();
	// DLListNode* first_node_original = u->first;
	// DLListNode* first_node_clone = newDLListNode(first_node_original->value);
	// clone_list->first = first_node_clone;
	// clone_list->size++;
	// while (first_node_original->next)
	// {
	// 	first_node_original = first_node_original->next;
	// 	DLListNode* new_clone_node = newDLListNode(first_node_original->value);
	// 	first_node_clone->next = new_clone_node;
	// 	new_clone_node->prev = first_node_clone;
	// 	first_node_clone = first_node_clone->next;
	// 	clone_list->size++;
	// }
	// clone_list->last = first_node_clone;
	// // or i think, use the function defined before
	DLList* clone_list = newDLList();
	DLListNode* current = u->first;
	while(current->next)
	{
		add_node_at_end_v2(clone_list, current->value);
		current = current->next;
	}
	add_node_at_end_v2(clone_list, current->value);
	return clone_list;
}

int check_existence(DLList* current_list, int value)
{
	if (current_list->size == 0) return 0;
	DLListNode* current = current_list->first;
	while(current->next)
	{
		if(current->value==value)
		{
			return 1;
		}
		current = current->next;
	}
	if (current->value == value)
	{
		return 1;
	}
	return 0;
}

// compute the union of two DLLists u and v
// put your time complexity analysis for intersection() here
/*
This function also calls the check_existence function and add_node_at_end_v2 function
The self-written check_existence function goes through the linked_list to check whether certain element exists, so it needs n (time) in the worse case
The add_node_at_end_v2 function needs constant time, as mentioned before.

<---Assume there is n1 elements in the first list, n2 elements in the second list, n elements overall--->
So n=n1+n2, let n1=a*n, n2=b*n, in which a and b are both constants

The simplified structure of this function is:

Function:
	preparation, check whether two lists are empty (Time: constant)
	create the clone of the first list (Time: c*n1)
	for each_node in list2: (Loop, run n2 times)
		if current node not in clone list, add it (Time: up to n2)
	return (Time: constant)

So the overall time is: c+c*n1+n2*n2+c=c+c*a*n+b*b*n*n+c, so the time complexity is O(n^2), in which n is the number of overall elements.
(Note that in this O(n^2), it does not matter that n is the number of element in list1 or list2 or there two lists)
 */
DLList *setUnion(DLList *u, DLList *v)
{
	// put your code here
	if(u->size == 0 && v->size ==0)
	{
		DLList* temp = newDLList();
		return temp;
	}
	if(u->size == 0)
	{
		DLList* temp = cloneList(v);
		return temp;
	}
	if (v->size ==0)
	{
		DLList* temp = cloneList(u);
		return temp;
	}
	DLList* newlist = cloneList(u);
	DLListNode* first_node = v->first;
	if (!check_existence(newlist,first_node->value))
	{
		add_node_at_end_v2(newlist, first_node->value);
	}
	while(first_node->next)
	{
		first_node = first_node->next;
		if (!check_existence(newlist,first_node->value))
		{
			add_node_at_end_v2(newlist, first_node->value);
		}
	}
	return newlist;
}

// compute the insection of two DLLists u and v
// put your time complexity analysis for intersection() here
/*
This function calls the check_existence function and add_node_at_end_v2 function.
As mentioned before, the check_existence function needs time n, and add_node_at_end_v2 function needs constant time.

<---Assume list1 has n1 elements, list2 has n2 elements, n elements overall--->
Let n1=a*n, n2=b*n, in which a and b are both constants.

The simplified structure of this function is:

Function:
	preparation, check whether two lists are empty, and create new list (Time: constant)
	for each_node in list1: (Loop, run n1 times)
		check whether the current node exists in list2, if so, add it in the new list (Time: c*n2)
	return (Time: constant)

So the overall time is: c+n1*c*n2+c=c+a*b*c*n*n+c, so the time complexity of this function is O(n^2), in which n is the number of elements.
(Note that in this O(n^2), it does not matter that n is the number of element in list1 or list2 or there two lists)
 */
DLList *setIntersection(DLList *u, DLList *v)
{
	// put your code here
	if (u->size==0 || v->size ==0)
	{
		DLList* temp = newDLList();
		return temp;
	}
	DLList* newlist = newDLList();
	DLListNode* first_node_u = u->first;
	if(check_existence(v, first_node_u->value))//check whether the first node in u exists in v
	{
		add_node_at_end_v2(newlist, first_node_u->value);//if so, add it in the new list
	}
	while(first_node_u->next)//for the rest node in u, do the same thing
	{
		first_node_u = first_node_u->next;
		if (check_existence(v,first_node_u->value))
		{
			add_node_at_end_v2(newlist, first_node_u->value);
		}
	}
	return newlist;
}

// free up all space associated with list
// put your time complexity analysis for freeDLList() here
/*
 <---Assume there are n elements in the list--->

The simplified structure of this function is:

Function:
	preparation (Time: constant)
	for each_node in this list: (Loop, run n times)
		free this node (Time: constant)
	free the list (Time constant)

So the overall time is: c+c*n+c, so the time complexity is O(n), in which n is the number of elements in the list.
 */
void freeDLList(DLList *L)//I think it is necessary to free each node in the list, because the list only contains some addresses to certain nodes, not the block
{
	// put your code here	
	DLListNode* first_node = L->first;
	DLListNode* temp;
	while (first_node->next)
	{
		temp = first_node;
		first_node = first_node->next;
		free(temp);
	}
	free(first_node);
	free(L);
	return;
}


// display items of a DLList
// put your time complexity analysis for printDDList() here
/*
 <---Assume there are n elements in the list--->

The simplified structure of this function is:

Function:
	preparation, check whether the list is empty (Time: constant)
	for each_node in the list: (Loop, run n times)
		print current node (Time: constant)

So the overall time is: c+n*c+c, which means that the time complexity is O(n), in which n is the number of elements in the list.
 */
void printDLList(DLList *u)
{
	// put your code here
	DLListNode* first_node = u->first;
	if (!first_node)
	{
		return;
	}
	while(first_node->next)//or I think use first_node!=u->last is also valid
	{
		printf("%d\n", first_node->value);
		first_node = first_node->next;
	}
	printf("%d\n", first_node->value);
	return;
}

int main()
{
	DLList *list1, *list2, *list3, *list4;
	
	list1 = CreateDLListFromFileDlist("File1.txt");
	printDLList(list1);
	
	list2 = CreateDLListFromFileDlist("File2.txt");
	printDLList(list2);
	
	list3 = setUnion(list1, list2);
	printDLList(list3);
	
	list4 = setIntersection(list1, list2);
	printDLList(list4);
	
	freeDLList(list1);
	freeDLList(list2);
	freeDLList(list3);
	freeDLList(list4);
	
	printf("please type all the integers of list1\n");
	list1 = CreateDLListFromFileDlist("stdin");
	
	printf("please type all the integers of list2\n");
	list2 = CreateDLListFromFileDlist("stdin");
	
	list3 = cloneList(list1);
	printDLList(list3);
	list4 = cloneList(list2);
	printDLList(list4);
	
	freeDLList(list1);
	freeDLList(list2);
	freeDLList(list3);
	freeDLList(list4);

	return 0;
}
