#include <glib.h>
#include "Stack.h"

typedef struct stack {
	GDestroyNotify destroyFunc;
	GQueue *stack;
} *Stack;

Stack make_full_stack(GDestroyNotify destroyFunc) {
	Stack s = g_malloc(sizeof(struct stack));

	s->destroyFunc = destroyFunc;

	s->stack = g_queue_new();

	return s;
}

Stack make_stack() {
	return make_full_stack(NULL);
}

int is_empty_stack(Stack s) {
	return g_queue_is_empty(s->stack);
}

int size_stack(Stack s) {
	return g_queue_get_length(s->stack);
}

void push_stack(Stack s, gpointer entry) {
	g_queue_push_head(s->stack, entry);
}

gpointer pop_stack(Stack s) {
	return g_queue_pop_head(s->stack);
}

gpointer peek_stack(Stack s) {
	return g_queue_peek_head(s->stack);
}

void destroy_stack(Stack s) {
	gpointer tmp;

	if(s->destroyFunc) {
		while( (tmp = pop_stack(s)) != NULL)
			(*s->destroyFunc)(tmp);
	}

	g_queue_free(s->stack);
	g_free(s);
}