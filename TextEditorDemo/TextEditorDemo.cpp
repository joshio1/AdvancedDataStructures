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


text_t *create_tree(void)
{
	text_t *tmp_node;
	tmp_node = get_node();
	tmp_node->left = NULL;
	tmp_node->number_of_leaves = 0;
	return(tmp_node);
}

void left_rotation(text_t *n)
{
	text_t *tmp_node;
	key_t        tmp_key;
	tmp_node = n->left;
	tmp_key = n->key;
	n->left = n->right;
	n->key = n->right->key;
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
	n->right->key = tmp_key;
	n->right->number_of_leaves = n->right->left->number_of_leaves + n->right->right->number_of_leaves;
}

object_t *find(text_t *tree, key_t query_key)
{
	text_t *tmp_node;
	if (tree->left == NULL)
		return(NULL);
	else
	{
		tmp_node = tree;
		while (tmp_node->right != NULL)
		{
			if (query_key < tmp_node->key)
				tmp_node = tmp_node->left;
			else
				tmp_node = tmp_node->right;
		}
		if (tmp_node->key == query_key)
			return((object_t *)tmp_node->left);
		else
			return(NULL);
	}
}

int insert(text_t *tree, key_t new_key, object_t *new_object)
{
	text_t *tmp_node;
	int finished;
	if (tree->left == NULL)
	{
		tree->left = (text_t *)new_object;
		tree->key = new_key;
		tree->height = 0;
		tree->number_of_leaves = 1;
		tree->right = NULL;
	}
	else
	{
		text_t * path_stack[100]; int  path_st_p = 0;
		tmp_node = tree;
		while (tmp_node->right != NULL)
		{
			path_stack[path_st_p++] = tmp_node;
			// add an element onto the key_increment_stack only when it is not the same.      
			if (new_key < tmp_node->key) {
				tmp_node = tmp_node->left;
			}
			else {
				tmp_node = tmp_node->right;
			}
		}
		/* found the candidate leaf. Test whether key distinct */
		if (tmp_node->key == new_key) {
			return NULL;
		}
		/* key is distinct, now perform the insert */
		{  text_t *old_leaf, *new_leaf;
		old_leaf = get_node();
		// old_leaf->left is the object held by tmp_node
		old_leaf->left = tmp_node->left;
		old_leaf->key = tmp_node->key;
		old_leaf->right = NULL;
		old_leaf->height = 0;
		old_leaf->number_of_leaves = 1;

		new_leaf = get_node();
		new_leaf->left = (text_t *)new_object;
		new_leaf->key = new_key;
		new_leaf->right = NULL;
		new_leaf->height = 0;
		new_leaf->number_of_leaves = 1;
		if (tmp_node->key < new_key)
		{
			tmp_node->left = old_leaf;
			tmp_node->right = new_leaf;
			tmp_node->key = new_key;
		}
		else
		{
			tmp_node->left = new_leaf;
			tmp_node->right = old_leaf;
		}
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
	return(0);
}



object_t *delete_node(text_t *tree, int index)
{
	text_t *tmp_node, *upper_node, *other_node;
	object_t *deleted_object; int finished;
	if (tree->left == NULL)
		return(NULL);
	else if (tree->right == NULL)
	{
		if (tree->key == index)
		{
			deleted_object = (object_t *)tree->left;
			tree->left = NULL;
			return(deleted_object);
		}
		else
			return(NULL);
	}
	else
	{
		text_t * path_stack[100]; int path_st_p = 0;
		tmp_node = tree;
		while (tmp_node->right != NULL)
		{
			path_stack[path_st_p++] = tmp_node;
			upper_node = tmp_node;
			if (index < tmp_node->key)
			{
				tmp_node = upper_node->left;
				other_node = upper_node->right;
			}
			else
			{
				tmp_node = upper_node->right;
				other_node = upper_node->left;
			}
		}
		if (tmp_node->key != index)
			deleted_object = NULL;
		else
		{
			upper_node->key = other_node->key;
			upper_node->left = other_node->left;
			upper_node->right = other_node->right;
			upper_node->height = other_node->height;
			upper_node->number_of_leaves = other_node->number_of_leaves;
			deleted_object = (object_t *)tmp_node->left;
			return_node(tmp_node);
			return_node(other_node);
		}

		/*start rebalance*/
		path_st_p -= 1;
		while (path_st_p > 0)
		{
			int tmp_height, old_height;
			tmp_node = path_stack[--path_st_p];
			if (tmp_node->right != NULL)
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
		/*end rebalance*/
		return(deleted_object);
	}
}

text_t * create_text() {
	text_t* tree = create_tree();
	return tree;
}
int length_text(text_t *txt) {
	if (txt != NULL)
		return txt->number_of_leaves;
	else
		return (-1);
}
char * get_line(text_t *txt, int index) {
	object_t* findobj = find(txt, index);
	if (findobj == NULL)
		return NULL;
	else
		return *findobj;
}
void append_line(text_t *txt, char * new_line) {
	int key_to_insert, success;
	int number_of_leaves = length_text(txt);
	key_to_insert = number_of_leaves + 1;
	object_t* insobj;
	insobj = (object_t *)malloc(sizeof(int));
	*insobj = new_line;
	success = insert(txt, key_to_insert, insobj);
	//if (success == 0)
		//printf("  insert successful, key = %d, \
	 //           height is %d,\  number_of_leaves is %d\n",
		//	key_to_insert, txt->height, txt->number_of_leaves);
	//else
	//	printf("  insert failed, success = %d\n", success);
}
char * set_line(text_t *txt, int index, char * new_line) {

	object_t* insobj;
	object_t *found_line = find(txt, index);
	if (found_line == NULL)
		return(NULL);
	insobj = (object_t *)malloc(sizeof(int));
	*insobj = *found_line;
	*found_line = new_line;
	return *insobj;
}
void insert_line(text_t *txt, int index, char * new_line) {
	char* old_line = set_line(txt, index, new_line);
	if (old_line != NULL) {
		for (int i = index + 1; i <= length_text(txt); i++) {
			old_line = set_line(txt, i, old_line);
		}
		append_line(txt, old_line);
	}
	else {
		append_line(txt, new_line);
	}

}
char * delete_line(text_t *txt, int index) {
	object_t *old_line = find(txt, index);
	if (old_line == NULL)
		return(NULL);
	else {
		object_t* insobj;
		insobj = (object_t *)malloc(sizeof(int));
		*insobj = *old_line;
		int i = index;
		for (i = index;  i < length_text(txt); i++) {
			set_line(txt, i, *find(txt, i+1));
		}
		delete_node(txt, i);
		return *insobj;
	}
	/*object_t* deletedObj = delete_node(txt, index);
	if (deletedObj == NULL)
		return NULL;
	else
		return *deletedObj;*/
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
	int i, tmp;
	text_t *txt2;
	char *c;


	txt2 = create_text();
	for (i = 1; i <= 10000; i++) {
		if (i % 2 == 1)
			append_line(txt2, "A");
		else
			append_line(txt2, "B");
	}

	for (i = 10000; i > 1; i -= 2) {
		c = delete_line(txt2, i);
		printf("%c",*c);
		append_line(txt2, c);
		printf(get_line(txt2, 9998));
	}

	for (i = 1; i <= 5000; i++) {
		c = get_line(txt2, i);
		printf("%c", *c);
	}
	/*for (i = 1; i <= 5000; i++) {
		c = get_line(txt2, i);
		EXPECT_EQ(*c, 'A') << ("line %d of txt2 should be A, found %s\n", i, c);
	}*/
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
