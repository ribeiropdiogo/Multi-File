#include <glib.h>
#include "Node.h"

typedef struct node {
	char *name;
	int id;
} *Node;

Node make_node(char * name, int id){
	Node node = g_malloc(sizeof(struct node));

	node->name = name;

	node->id = id;

	return node;
}

char *get_name_node(Node node){
	return node->name;
}

int get_id_node(Node node){
	return node->id;
}

void destroy_node(Node node){
	g_free(node);
}