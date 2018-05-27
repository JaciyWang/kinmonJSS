#ifndef HEYMI_GME
#define HEYMI_GME

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "../algo.h"
#include <unistd.h>


typedef struct elem_t {
    int product,machine,piece,cost,start_at,finish_at,prev_finish_at;
    struct elem_t *prev,*next;
    struct elem_t *prev_padding,*next_padding;

} Elem;

typedef struct {
    Elem *table,*pure_table;
    int machine_count;
    int product_count;
} DecoderAdjMat;

typedef enum {
    THREADING_WAITING = 0,
    THREADING_PROCESSING = 1,
    THREADING_FINISHED = 2,
    THREADING_EXIT = 3
} DecoderThreadingWorkingType;

typedef struct{
    DecoderThreadingWorkingType working_at;
    DecoderAdjMat *mat;
    Elem **machines,**padding_machines,**padding_last_machines,**products;
    pthread_mutex_t  mp;
    int *gens[10000];
    int gen_span[10000];
    int gens_count;
    pthread_t thread;
    bool _output;
} DecoderThreadingState;
DecoderThreadingState* decoder_create_thread(JSSInput *input,bool _output);
void decoder_thread_set_gen_count(DecoderThreadingState *ts,int count);
void decoder_thread_set_gen(DecoderThreadingState *ts,int i,int *gen);
void  decoder_thread_tick(DecoderThreadingState *ts);
int *decoder_thread_wait_finish(DecoderThreadingState *ts);
int *decoder_thread_destroy(DecoderThreadingState *ts);
#endif