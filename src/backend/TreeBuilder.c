#include <glib.h>
#include <stdio.h>
#include "Dictionary.h"
#include "Node.h"
#include "Stack.h"

// -------------------------------------------------------

#define BASE_SIZE 5
#define NODE(o) (*(Node*)(o))

// -------------------------------------------------------

typedef struct treebuilder {
	Dictionary files;
	GNode **tree;
	Stack *coordinator;
	int main_nodes, cords,
		n_main_node, n_cords;
} *TreeBuilder;

// -------------------------------------------------------

static void add_main_file(TreeBuilder tb, char *file);
static void add_main_dir(TreeBuilder tb, char *dir);
static void double_tree(TreeBuilder tb);
static void double_coord(TreeBuilder tb, int level);
static Node make_file(TreeBuilder tb, char *file);
static GNode *add_to_tree(TreeBuilder tb, Node node);

// -------------------------------------------------------

TreeBuilder make_tree_builder() {
	int i;
	TreeBuilder tb = malloc(sizeof(struct treebuilder));

	tb->files = makeDictionary();

	tb->tree = malloc(sizeof(GNode*)*BASE_SIZE);
	tb->coordinator = malloc(sizeof(Stack)*BASE_SIZE);

	for(i = 0; i < BASE_SIZE; i++) {
		tb->tree[i] = NULL;
		tb->coordinator[i] = make_stack();
	}

	tb->main_nodes = BASE_SIZE;
	tb->cords = BASE_SIZE;
	tb->n_main_node = tb->n_cords = 0;

	return tb;
}

int add_file_tree_builder(TreeBuilder tb, char *file, unsigned int level) {
	int r = 1;
	GNode *dir;

	if(level == 0) {
		add_main_file(tb, file);
	}
	else if(level <= tb->cords 
		&& size_stack(tb->coordinator[level-1]) > 0) {

		dir = (GNode*)peek_stack(tb->coordinator[level-1]);

		g_node_append(dir, g_node_new(make_file(tb, file)));
	}	
	else {
		r = 0;
	}

	return r;
}

int add_dir_tree_builder(TreeBuilder tb, char *dir, unsigned int level) {
	int r = 1;
	GNode *dirno,*new;

	if(level == 0) {
		add_main_dir(tb, dir);
	}
	else if(level <= tb->cords 
		&& size_stack(tb->coordinator[level-1]) > 0) {

		double_coord(tb, level);

		dirno = (GNode*)peek_stack(tb->coordinator[level-1]);

		new = g_node_append(dirno, g_node_new(make_node(dir, 0)));
		
		push_stack(tb->coordinator[level], new);
	}	
	else {
		r = 0;
	}

	return r;
}

static void add_main_file(TreeBuilder tb, char *file) {
	add_to_tree(tb, make_file(tb, file));
}

static void add_main_dir(TreeBuilder tb, char *dir) {
	GNode * nn = add_to_tree(tb, make_node(dir, 0));

	push_stack(tb->coordinator[0], nn);
}

static void double_tree(TreeBuilder tb) {
	int i, N = tb->n_main_node;
	if(N == tb->main_nodes) {
		tb->tree = g_realloc(tb->tree, sizeof(GNode*)*2*N);
		tb->main_nodes = 2*N;

		for(i = N; i < tb->main_nodes; i++)
			tb->tree[i] = NULL;
	}
}

static void double_coord(TreeBuilder tb, int level) {
	int i;

	if(level >= tb->cords) {
		tb->coordinator = 
			g_realloc(tb->coordinator, sizeof(Stack)*2*tb->cords);
		tb->cords *= 2;

		for(i = tb->cords / 2; i < tb->cords; i++)
			tb->coordinator[i] = make_stack();
	}
}

static Node make_file(TreeBuilder tb, char *file) {
	insertDictionary(tb->files, file, NULL);

	return make_node(file, 1);
}

static GNode *add_to_tree(TreeBuilder tb, Node node) {
	double_tree(tb);

	GNode * nn = g_node_new(node);

	tb->tree[tb->n_main_node++] = nn;

	return nn;
}

void destroy_tree_builder(TreeBuilder tb) {
	int i;

	free(tb->files);

	for(i = 0; i < tb->main_nodes; i++) {
		if(tb->tree[i])
			g_node_destroy(tb->tree[i]);
	}

	free(tb->tree);

	for(i = 0; i < tb->cords; i++) {
		destroy_stack(tb->coordinator[i]);
	}

	free(tb->coordinator);

	free(tb);
}

void func_cois(GNode *node, gpointer data) {
	Node tmp = NODE(node);
	printf(">IN %s\n", get_name_node(tmp));
	g_node_children_foreach(node,G_TRAVERSE_ALL,func_cois,NULL);
}

void print_tree_builder(TreeBuilder tb) {
	int i;
	Node tmp;

	for(i = 0; i < tb->main_nodes; i++) {
		if(tb->tree[i]) {
			tmp = NODE(tb->tree[i]);
			printf("*IN %s\n", get_name_node(tmp));
			g_node_children_foreach(tb->tree[i],G_TRAVERSE_ALL,func_cois,NULL);
		}
	}
}

/*
int main() {
	TreeBuilder tb = make_tree_builder();

	printf("tree:\n");

	add_file_tree_builder(tb, strdup("bruh"), 0);

	add_dir_tree_builder(tb, strdup("bruthaa"), 0);

	add_dir_tree_builder(tb, strdup("meeen"), 1);

	add_dir_tree_builder(tb, strdup("rqqm"), 0);

	add_dir_tree_builder(tb, strdup("meeen"), 1);

	add_dir_tree_builder(tb, strdup("sgod"), 2);

	add_dir_tree_builder(tb, strdup("meeen22"), 1);

	add_dir_tree_builder(tb, strdup("subdirgod"), 2);

	print_tree_builder(tb);

	destroy_tree_builder(tb);
}*/