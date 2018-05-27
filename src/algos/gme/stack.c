#include "stack.h"

Stack* stack_init() {
    return (Stack*)calloc(1, sizeof(Stack));
}
void stack_destory(Stack* s) {
    free(s);
}
void stack_push(Stack* s, void* udata) {
    assert(++(s->count) < MAX_STACK_SIZE);
    s->store[s->count] = udata;
}
void* stack_pop(Stack* s) {
    return s->count  == 0 ? NULL : s->store[s->count--];
}
void* stack_top(Stack* s){
    return s->count  == 0 ? NULL : s->store[s->count];
}