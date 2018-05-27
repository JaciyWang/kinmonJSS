#ifndef HEYMI_DS_STACK
#define HEYMI_DS_STACK

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef MAX_STACK_SIZE
#define MAX_STACK_SIZE 4096
#endif

typedef struct stack_s {
    void* store[MAX_STACK_SIZE];
    size_t count;
} Stack;

Stack* stack_init();
void stack_destory(Stack* s);
void stack_push(Stack* s, void* udata);
void* stack_top(Stack* s);
void* stack_pop(Stack* s);

#endif