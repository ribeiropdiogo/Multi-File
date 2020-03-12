#include <glib.h>
#include "Stack.h"

typedef struct stack {
	GDestroyNotify destroyFunc;
	GQueue *stack;
} *Stack;

Stack makeStack() {
	return makeFullStack(NULL);
}

Stack makeFullStack(GDestroyNotify destroyFunc) {
	Stack s = g_malloc(sizeof(struct stack));

	s->destroyFunc = destroyFunc;

	s->stack = g_queue_new();

	return s;
}

int isEmpty(Stack s) {
	return g_queue_is_empty(s->stack);
}

int sizeStack(Stack s) {
	return g_queue_get_length(s->stack);
}

void pushStack(Stack s, gpointer entry) {
	g_queue_push_head(s->stack, entry);
}

gpointer popStack(Stack s) {
	return g_queue_pop_head(s->stack);
}

void destroyStack(Stack s) {
	gpointer tmp;

	if(s->destroyFunc) {
		while( (tmp = popStack(s)) != NULL)
			(*s->destroyFunc)(tmp);
	}

	g_queue_free(s->stack);
	g_free(s);
}