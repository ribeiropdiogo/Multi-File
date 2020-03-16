#include <glib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
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
		n_main_node, n_cords, bef;
} *TreeBuilder;

// -------------------------------------------------------

static void add_main_file(TreeBuilder tb, char *file);
static void add_main_dir(TreeBuilder tb, char *dir);
static void double_tree(TreeBuilder tb);
static void double_coord(TreeBuilder tb, int level);
static Node make_file(TreeBuilder tb, char *file);
static GNode *add_to_tree(TreeBuilder tb, Node node);
static DIR *opendirat(int dir, const char *dirname);
static void write_file(TreeBuilder tb, int at, char * file);
static void write_dir(GNode *node, gpointer data);

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

static DIR *opendirat(int dir, const char *dirname) {
  // Open directory.
  int fd = openat(dir, dirname, O_RDONLY|O_DIRECTORY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
  
  if (fd == -1)
    return NULL;

  // Create directory handle.
  DIR *result = fdopendir(fd);
  if (result == NULL)
    close(fd);
  return result;
}

static void write_file(TreeBuilder tb, int at, char * file) {
	int fd, x;
	Node node;
	gpointer tmp;

	if(at < 0)
		fd = open(file, O_CREAT | O_WRONLY, 0777);
	else
		fd = openat(at, file, O_CREAT | O_WRONLY, 0777);

	tmp = getValueDictionary(tb->files, file);

	//node = (Node*)tmp;

	printf("!%s! file TMP IS %ld\n", file, tmp);

	if(tmp != NULL) {
		node = (Node)(tmp);
		printf("I has %ld len %ld \n%s\n", node, get_id_node(node), (char*)get_name_node(node));
		printf("What is up\n");
		x = write(fd,
			get_name_node(node),
			get_id_node(node)
			);
	}

	close(fd);
}

static void write_dir(GNode *node, gpointer data) {
	DIR *dir;
	int id, fd, at;

	Node tmp = NODE(node);
	char *name = (char*)get_name_node(tmp);
	TreeBuilder tb = (TreeBuilder)data;
	
	id = get_id_node(tmp);
	at = tb->bef;

	if(id == 0) {
		
		if(at < 0) {
			mkdir(name, 0777);
			dir = opendir(name);
		}
		else {
			
			mkdirat(at, name, 0777);
			
			dir = opendirat(at, name);
			
		}

		tb->bef = dirfd(dir);
		
		// for each
		g_node_children_foreach(
			node,
			G_TRAVERSE_ALL,
			write_dir,
			data
			);

		closedir(dir);
	}
	else {
		write_file(tb, at, name);
	}

	tb->bef = at;	
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

void dump_tree_builder(TreeBuilder tb) {
	Node node;

	int i, id, fd, aux, N = tb->main_nodes;
	DIR *dir;
	GNode *tmp;
	char *name;

	for(i = 0; i < N; i++) {

		tmp = tb->tree[i];
		
		if(tmp) {
			
			//node = make_node(tb, -1);
			tb->bef = -1;
			
			write_dir(tmp, tb);
	
			//destroy_node(node);
		}
	}
}

int contains_file(TreeBuilder tb, char *file) {
	return containsDictionary(tb->files, file);
}

void add_info_to_file(TreeBuilder tb, char *file, char *info, int len) {
	Node node = make_node(info, len);

	printf("Coisa %ld\n", node);
	printf("My size is %d,\nAQUI\n%s", get_id_node(node), (char*)get_name_node(node));

	appendDictionary(tb->files, file, (gpointer)node);
}
/*
int main() {
	TreeBuilder tb = make_tree_builder();

	//add_file_tree_builder(tb, strdup("bruh"), 0);

	add_dir_tree_builder(tb, strdup("bruthaa"), 0);

	add_dir_tree_builder(tb, strdup("meeen"), 1);

	add_file_tree_builder(tb, strdup("file0"), 1);

	add_dir_tree_builder(tb, strdup("rqqm"), 0);

	add_dir_tree_builder(tb, strdup("meeen"), 1);

	add_dir_tree_builder(tb, strdup("sgod"), 2);

	add_dir_tree_builder(tb, strdup("meeen22"), 1);

	add_dir_tree_builder(tb, strdup("subdirgod"), 2);

	add_file_tree_builder(tb, strdup("file1"), 1);

	add_file_tree_builder(tb, strdup("file2"), 2);

	add_file_tree_builder(tb, strdup("file3"), 3);

	dump_tree_builder(tb);

	destroy_tree_builder(tb);
}*/