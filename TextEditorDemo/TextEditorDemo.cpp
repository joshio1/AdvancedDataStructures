#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

typedef char* object_t;
typedef int key_t;
typedef struct text_t {
	key_t        key;
	struct text_t  *left;
	struct text_t *right;
	int height;
	int number_of_leaves;
} text_t;

text_t * stack[100];

int top = -1;

/*resets the stack pointer to make a new stack */
void createstack() {
	top = -1;
}

/*Pop's the node out of stack*/
text_t * pop() {
	if (top == -1) {
		printf("Stack is empty, underflow");
		exit(1);
	}
	return stack[top--];
}
void remove_stack() {
	top = -1;
}

/*Test if stack is empty*/
int stack_empty() {
	if (top == -1) {
		return 1;
	}
	else {
		return 0;
	}
}

/*Pushes the node into stack */
void push(text_t * tree_node) {
	if (top == 100) {
		printf("Stack limit reached");
		exit(1);
	}
	stack[++top] = tree_node;
}

text_t *currentblock = NULL;
int    size_left;
text_t *free_list = NULL;

text_t *get_node()
{
	text_t *tmp;
	if (free_list != NULL)
	{
		tmp = free_list;
		free_list = free_list->left;
	}
	else
	{
		if (currentblock == NULL || size_left == 0)
		{
			currentblock =
				(text_t *)malloc(BLOCKSIZE * sizeof(text_t));
			size_left = BLOCKSIZE;
		}
		tmp = currentblock++;
		size_left -= 1;
	}
	return(tmp);
}


void return_node(text_t *node)
{
	node->left = free_list;
	free_list = node;
}

void left_rotation(text_t *n)
{
	text_t *tmp_node;
	key_t        tmp_key;
	tmp_node = n->left;
	tmp_key = n->key;
	n->left = n->right;
	n->key = n->right->key + n-> key;
	n->right = n->left->right;
	n->left->right = n->left->left;
	n->left->left = tmp_node;
	n->left->key = tmp_key;
	n->left->number_of_leaves = n->left->left->number_of_leaves + n->left->right->number_of_leaves;
}

void right_rotation(text_t *n)
{
	text_t *tmp_node;
	key_t        tmp_key;
	tmp_node = n->right;
	tmp_key = n->key;
	n->right = n->left;
	n->key = n->left->key;
	n->left = n->right->left;
	n->right->left = n->right->right;
	n->right->right = tmp_node;
	n->right->key = tmp_key - n->key;
	n->right->number_of_leaves = n->right->left->number_of_leaves + n->right->right->number_of_leaves;
}

int length_text(text_t *txt) {
	if (txt != NULL)
		return (txt->number_of_leaves - 1); //-1 is for the \0 in the end
	else
		return (-1);
}
char * get_line(text_t *tree, int query_key) {
	text_t *tmp_node;
	if (tree->left == NULL)
		return(NULL);
	else
	{
		tmp_node = tree;
		while (tmp_node->right != NULL)
		{
			if (query_key <= tmp_node->key)
				tmp_node = tmp_node->left;
			else {
				query_key -= tmp_node->key;
				tmp_node = tmp_node->right;
			}
		}
		return (char *)(tmp_node->left);
	}
}
char * set_line(text_t *tree, int query_key, char * new_line) {
	text_t *tmp_node;
	if (tree->left == NULL)
		return(NULL);
	else
	{
		tmp_node = tree;
		while (tmp_node->right != NULL)
		{
			if (query_key <= tmp_node->key)
				tmp_node = tmp_node->left;
			else {
				query_key -= tmp_node->key;
				tmp_node = tmp_node->right;
			}
		}
		if (query_key == 1) {
			char * temp = (char*)tmp_node->left;
			tmp_node->left = (struct text_t *)new_line;
			return temp;
		}
		else {
			return NULL;
		}
	}
}

void insert_line(text_t *txt, int index, char * new_line) {
	text_t *tmp_node;
	if (txt->left == NULL)
	{
		txt->left = (text_t *)new_line;
		txt->key = 0;
		txt->height = 0;
		txt->number_of_leaves = 1;
		txt->right = NULL;
	}
	else
	{
		text_t * path_stack[100]; int  path_st_p = 0;
		tmp_node = txt;
		while (tmp_node->right != NULL)
		{
			path_stack[path_st_p++] = tmp_node;
			// add an element onto the key_increment_stack only when it is not the same.      
			if (index <= tmp_node->key) {
				tmp_node->key = tmp_node->key + 1;
				tmp_node = tmp_node->left;
			}
			else {
				index = index - tmp_node->key;
				tmp_node = tmp_node->right;
			}
		}
		/*perform the insert */
		{  text_t *old_leaf, *new_leaf;
		old_leaf = get_node();
		// old_leaf->left is the object held by tmp_node
		old_leaf->left = tmp_node->left;
		old_leaf->key = 0;
		old_leaf->right = NULL;
		old_leaf->height = 0;
		old_leaf->number_of_leaves = 1;

		new_leaf = get_node();
		new_leaf->left = (text_t *)new_line;
		new_leaf->key = 0;
		new_leaf->right = NULL;
		new_leaf->height = 0;
		new_leaf->number_of_leaves = 1;
		int difference = index - tmp_node->key;
		if (difference == 0 || difference == 1)
		{
			tmp_node->left = new_leaf;
			tmp_node->right = old_leaf;
		}
		else
		{
			tmp_node->left = old_leaf;
			tmp_node->right = new_leaf;
		}
		tmp_node->key = 1;
		tmp_node->height = 1;
		tmp_node->number_of_leaves = tmp_node->left->number_of_leaves + tmp_node->right->number_of_leaves;
		}
		/* rebalance */
		while (path_st_p > 0)
		{
			int tmp_height, old_height;
			tmp_node = path_stack[--path_st_p];
			tmp_node->number_of_leaves = tmp_node->left->number_of_leaves + tmp_node->right->number_of_leaves;
			old_height = tmp_node->height;
			if (tmp_node->left->height -
				tmp_node->right->height == 2)
			{
				if (tmp_node->left->left->height -
					tmp_node->right->height == 1)
				{
					right_rotation(tmp_node);
					tmp_node->right->height =
						tmp_node->right->left->height + 1;
					tmp_node->height = tmp_node->right->height + 1;
				}
				else
				{
					left_rotation(tmp_node->left);
					right_rotation(tmp_node);
					tmp_height = tmp_node->left->left->height;
					tmp_node->left->height = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			}
			else if (tmp_node->left->height -
				tmp_node->right->height == -2)
			{
				if (tmp_node->right->right->height -
					tmp_node->left->height == 1)
				{
					left_rotation(tmp_node);
					tmp_node->left->height =
						tmp_node->left->right->height + 1;
					tmp_node->height = tmp_node->left->height + 1;
				}
				else
				{
					right_rotation(tmp_node->right);
					left_rotation(tmp_node);
					tmp_height = tmp_node->right->right->height;
					tmp_node->left->height = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			}
			else /* update height even if there was no rotation */
			{
				if (tmp_node->left->height > tmp_node->right->height)
					tmp_node->height = tmp_node->left->height + 1;
				else
					tmp_node->height = tmp_node->right->height + 1;
			}
		}

	}
}

void append_line(text_t *txt, char * new_line) {
	int key_to_insert, success;
	int number_of_leaves = length_text(txt);
	key_to_insert = number_of_leaves + 1;
	insert_line(txt, key_to_insert, new_line);
	//if (success == 0)
	//printf("  insert successful, key = %d, \
		 //           height is %d,\  number_of_leaves is %d\n",
//	key_to_insert, txt->height, txt->number_of_leaves);
//else
//	printf("  insert failed, success = %d\n", success);
}

//char * delete_line(text_t *txt, int index) {
//	//object_t *old_line = find(txt, index);
//	//if (old_line == NULL)
//	//	return(NULL);
//	//else {
//	//	object_t* insobj;
//	//	insobj = (object_t *)malloc(sizeof(int));
//	//	*insobj = *old_line;
//	//	int i = index;
//	//	for (i = index;  i < length_text(txt); i++) {
//	//		set_line(txt, i, *find(txt, i+1));
//	//	}
//	//	delete_node(txt, i);
//	//	return *insobj;
//	//}
//	/*object_t* deletedObj = delete_node(txt, index);
//	if (deletedObj == NULL)
//		return NULL;
//	else
//		return *deletedObj;*/
//	return NULL;
//}

/*
Deletes the line of number index, renumbering all
lines after that line, and returns a pointer to the deleted line
*/
char * delete_line(text_t *txt, int index) {
	text_t * upper_node,* selected_node,* other_node;
	text_t * init_text = txt;
	int length = length_text(txt);
	if (tree->left == NULL)
			return(NULL);
	//Check index with maxlength
	if (index>length) {
		return NULL;
	}
	else {
		text_t * path_stack[100]; int path_st_p = 0;
		while (txt->right != NULL) {
			push(txt);
			upper_node = txt;
			if (index <= txt->key) {
				selected_node = txt->left;
				other_node = txt->right;
				txt->key = txt->key - 1;
				txt = txt->left;

			}
			else {
				index = index - txt->key;
				selected_node = txt->right;
				other_node = txt->left;
				txt = txt->right;
			}


		}
		//Pop the last node
		pop();

		upper_node->left = other_node->left;
		upper_node->right = other_node->right;
		upper_node->key = other_node->key;
		upper_node->height = other_node->height;
		upper_node->number_of_leaves = other_node->number_of_leaves;

		text_t * tmp_node = NULL;

		while (!stack_empty()) {
			int tmp_height, old_height;
			tmp_node = pop();
			if (tmp_node->right != NULL)
				tmp_node->number_of_leaves = tmp_node->left->number_of_leaves + tmp_node->right->number_of_leaves;
			old_height = tmp_node->height;

			if (tmp_node->left->height - tmp_node->right->height == 2)
			{
				if (tmp_node->left->left->height - tmp_node->right->height == 1)
				{
					right_rotation(tmp_node);
					tmp_node->right->height = tmp_node->right->left->height + 1;
					tmp_node->height = tmp_node->right->height + 1;
				}
				else
				{
					left_rotation(tmp_node->left);
					right_rotation(tmp_node);
					tmp_height =
						tmp_node->left->left->height;
					tmp_node->left->height = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			}
			else if (tmp_node->left->height - tmp_node->right->height == -2)
			{
				if (tmp_node->right->right->height - tmp_node->left->height == 1)
				{
					left_rotation(tmp_node);
					tmp_node->left->height = tmp_node->left->right->height + 1;
					tmp_node->height = tmp_node->left->height + 1;
				}
				else
				{
					right_rotation(tmp_node->right);
					left_rotation(tmp_node);
					tmp_height = tmp_node->right->right->height;
					tmp_node->left->height = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			}
			else /* update height even if there
					was no rotation */
			{
				if (tmp_node->left->height > 	tmp_node->right->height)
					tmp_node->height = tmp_node->left->height + 1;
				else
					tmp_node->height = tmp_node->right->height + 1;
			}
		}
		remove_stack();
		return (char *)selected_node->left;
		}
}

text_t *create_tree(void)
{
	text_t *root_node;
	root_node = get_node();
	root_node->left = NULL;
	root_node->number_of_leaves = 0;
	root_node->right = NULL;
	root_node->height = 0;
	insert_line(root_node, 1, "\\0");
	return(root_node);
}

text_t * create_text() {
	text_t* tree = create_tree();
	return tree;
}

void traverse_tree(text_t *txt) {
	text_t *tmp = txt;
	if (tmp->right == NULL) {
		printf("--------\n");
		printf("|Leaf key: %d |\n", tmp->key);
		printf("|Leaf text: %s |\n", *(object_t*)tmp->left);
		printf("--------\n");
	}
	else {
		traverse_tree(tmp->left);
		printf("Node Key: %d\n", tmp->key);
		printf("Number of leaves: %d\n", tmp->number_of_leaves);
		traverse_tree(tmp->right);
	}
}

int main()
{
	int i;
	text_t* text1 = create_text();
	append_line(text1, "line one");
	append_line(text1, "line hundred");
	insert_line(text1, 2, "line ninetynine");
	insert_line(text1, 2, "line ninetyeight");
	insert_line(text1, 2, "line ninetyseven");
	insert_line(text1, 2, "line ninetysix");
	insert_line(text1, 2, "line ninetyfive");
	for (i = 2; i <95; i++)
		insert_line(text1, 2, "some filler line between 1 and 95");

	char* output = (get_line(text1, 2));
	char* output1 = (get_line(text1, 99));
	set_line(text1, 100, "the last line");
	char* output2 =  (get_line(text1, 100));
	 for( i=99; i>=1; i-- )
		delete_line(text1, i );
	 char* output5 = (get_line(text1, 1));
	getchar();
	//text_t* text = create_text();
	//char nextop;
	////while ((nextop = getchar()) != 'q') {
	////append_line(text, "foobar");
	////printf("Length After Insert = %d \n",length_text(text));
	////}
	//append_line(text, "hello");
	//append_line(text, "how");
	//append_line(text, "are");
	//append_line(text, "you");
	//append_line(text, "?");
	//append_line(text, "Fine");
	//traverse_tree(text);
	//printf("-------------------------------------");
	//char *insobj = "pranav";
	//char *old_line = set_line(text, 2, insobj);
	//traverse_tree(text);
	//printf("-------------------------------------");
	//insert_line(text, 2, "omkar");
	//traverse_tree(text);
	//delete_line(text, 2);
	//printf("-------------------------------------");
	//traverse_tree(text);
	//text_t *searchtree;
	//char nextop;
	//searchtree = create_tree();
	//printf("Made Tree: Height-Balanced Tree\n");
	//while ((nextop = getchar()) != 'q')
	//{
	//	if (nextop == 'i')
	//	{
	//		int inskey, *insobj, success;
	//		insobj = (int *)malloc(sizeof(int));
	//		scanf(" %d", &inskey);
	//		*insobj = 10 * inskey + 2;
	//		success = insert(searchtree, inskey, insobj);
	//		if (success == 0)
	//			printf("  insert successful, key = %d, object value = %d, \
	 //                 height is %d\n",
//				inskey, *insobj, searchtree->height);
//		else
//			printf("  insert failed, success = %d\n", success);
//	}
//	if (nextop == 'f')
//	{
//		int findkey, *findobj;
//		scanf(" %d", &findkey);
//		findobj = find(searchtree, findkey);
//		if (findobj == NULL)
//			printf("  find failed, for key %d\n", findkey);
//		else
//			printf("  find successful, found object %d\n", *findobj);
//	}
//	if (nextop == 'd')
//	{
//		int delkey, *delobj;
//		scanf(" %d", &delkey);
//		delobj = delete_node(searchtree, delkey);
//		if (delobj == NULL)
//			printf("  delete failed for key %d\n", delkey);
//		else
//			printf("  delete successful, deleted object %d, height is now %d\n",
//				*delobj, searchtree->height);
//	}
//	if (nextop == '?')
//	{
//		printf("  Checking tree\n");
//		check_tree(searchtree, 0, -1000, 1000);
//		printf("\n");
//		if (searchtree->left != NULL)
//			printf("key in root is %d, height of tree is %d\n",
//				searchtree->key, searchtree->height);
//		printf("  Finished Checking tree\n");
//	}
//}
	return(0);
}
