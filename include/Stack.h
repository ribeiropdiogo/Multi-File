#ifndef _MKFRT_STACK_H_
#define _MKFRT_STACK_H_

#include <glib.h>

typedef struct stack * Stack;

Stack make_stack();

int is_empty_stack(Stack s);

int size_stack(Stack s);

void push_stack(Stack s, gpointer entry);

gpointer pop_stack(Stack s);

gpointer peek_stack(Stack s);

void destroy_stack(Stack s);

#endif
