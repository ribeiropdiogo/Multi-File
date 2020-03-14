#ifndef __TREE_BUILDER_DIRFIL__
#define __TREE_BUILDER_DIRFIL__

typedef struct treebuilder * TreeBuilder;

TreeBuilder make_tree_builder();

int add_file_tree_builder(TreeBuilder tb, char *file, unsigned int level);

int add_dir_tree_builder(TreeBuilder tb, char *dir, unsigned int level);

void destroy_tree_builder(TreeBuilder tb);

#endif