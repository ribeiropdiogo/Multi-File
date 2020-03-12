#ifndef _MKFRT_DICTIONARY_H_
#define _MKFRT_DICTIONARY_H_

#include <glib.h>

typedef struct stack * Stack;

Stack makeStack();

int isEmpty(Stack s);

int sizeStack(Stack s);

void pushStack(Stack s, gpointer entry);

gpointer popStack(Stack s);

void destroyStack(Stack s);

#endif
