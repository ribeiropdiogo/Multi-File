#ifndef __NODE_FILE_H__
#define __NODE_FILE_H__

#include <glib.h>

typedef struct node * Node;

/**
 * Cria um nó.
 * @param name Nome do nó.
 * @param id Id do nó, 0 é diretoria, 1 é ficheiro.
 */
Node make_node(gpointer name, int id);

gpointer get_name_node(Node node);

int get_id_node(Node node);

void set_id_node(Node node, int id);

void destroy_node(Node node);

#endif