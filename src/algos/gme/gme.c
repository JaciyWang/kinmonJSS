//
// Created by heymi on 2018/5/24.
//
#include "gme.h"
#include "../../readers/reader.h"
#include "stack.h"
#include "../../readers/dataset2.h"
#include "../../utils/dump_jss_input.h"
#define  MAX(a,b) ((a)>(b) ? (a) :(b))
#define  MIN(a,b) ((a)<(b) ? (a) :(b))

#define ELEM(s,product,piece) (((s)->table + (s)->machine_count * ((product) - 1) + (piece) -1))

static void decoder_clean_thread(DecoderThreadingState *s){
    int machine_count =  s->mat->machine_count,product_count = s->mat->product_count;
    memcpy(s->mat->table,s->mat->pure_table,machine_count*product_count*sizeof(Elem));
    memset(s->machines,0,(machine_count+1)*sizeof(Elem*));
    memset(s->padding_machines,0,(machine_count+1)*sizeof(Elem*));
    memset(s->padding_last_machines,0,(machine_count+1)*sizeof(Elem*));
    memset(s->products,0,(product_count+1)*sizeof(Elem*));
}

void decoder_thread_set_gen_count(DecoderThreadingState *ts,int count){
    assert(pthread_mutex_lock(&(ts->mp))==0);
    ts->gens_count = count;
    assert(pthread_mutex_unlock(&(ts->mp))==0);
}
void decoder_thread_set_gen(DecoderThreadingState *ts,int i,int *gen){
    assert(pthread_mutex_lock(&(ts->mp))==0);
    assert(i < ts->gens_count);
    ts->gens[i] = gen;
    assert(pthread_mutex_unlock(&(ts->mp))==0);
}
void decoder_thread_tick(DecoderThreadingState *ts){
    assert(pthread_mutex_lock(&(ts->mp))==0);
    ts->working_at = THREADING_PROCESSING;
    assert(pthread_mutex_unlock(&(ts->mp))==0);
}
int *decoder_thread_wait_finish(DecoderThreadingState *ts){
    while(1){
        assert(pthread_mutex_lock(&(ts->mp))==0);
        if(ts->working_at == THREADING_FINISHED){
            assert(pthread_mutex_unlock(&(ts->mp))==0);
            return ts->gen_span;

        }
        assert(pthread_mutex_unlock(&(ts->mp))==0);
    }
}
 int decoder_process(DecoderThreadingState* ts,int *gen){
    DecoderAdjMat *s = ts->mat;
    Elem **machines =ts->machines,**products = ts->products;
    Elem **padding_machines =  ts->padding_machines;
    Elem **padding_last_machines =  ts->padding_last_machines;
    int last_piece_finish_at,len = s->machine_count * s->product_count;
    for(int i=0;i<len;i++){

        int p = gen[i] + 1;

        if(products[p] == NULL){
            products[p] = ELEM(s,p,1);
            last_piece_finish_at = 0;
        } else {
            last_piece_finish_at = products[p]->finish_at;
            products[p]++;
        }

        int m = products[p]->machine;

        if(machines[m] == NULL){
            machines[m] = products[p];
            if(products[p]->piece == 1){
                products[p]->start_at = 0;
            } else {
                products[p]->start_at = (products[p] - 1)->finish_at;
                products[p]->prev_finish_at = 0;
                padding_machines[m] = products[p];
                padding_last_machines[m] = products[p];
            }

            products[p]->finish_at = products[p]->start_at + products[p]->cost;
        }
        if(machines[m] != products[p]) {
            Elem *ppm = NULL, *product = products[p];
            if (padding_machines[m] != NULL) {
                ppm = padding_machines[m];
                do {
                    int a = last_piece_finish_at;
                    if (a < ppm->prev_finish_at) a = ppm->prev_finish_at;
                    if (a + product->cost < ppm->start_at) {

                        product->start_at = a;
                        product->finish_at = product->start_at+product->cost;
                        if (ppm->prev == NULL) {
                            product->prev = NULL;
                            product->next = ppm;
                            ppm->prev = product;
                        } else {
                            ppm = ppm->prev;
                            ppm->next->prev = product;
                            product->next = ppm->next;
                            ppm->next = product;
                            product->prev = ppm;
                            ppm = ppm->next->next;
                            assert(ppm!=NULL);
                        }

                        //加入完后 被加入工序前后都有空位 前有空位 后有空位
                        if (a > ppm->prev_finish_at) {
                            //前有空位
                            product->prev_finish_at = ppm->prev_finish_at;
                            if (ppm->prev_padding == NULL) {
                                product->prev_padding = NULL;
                                product->next_padding = ppm;
                                ppm->prev_padding = product;
                                padding_machines[m] = product;
                            } else {
                                ppm = ppm->prev_padding;
                                if (ppm->next_padding) ppm->next_padding->prev_padding = product;
                                product->next_padding = ppm->next_padding;
                                ppm->next_padding = product;
                                product->prev_padding = ppm;
                                ppm = ppm->next_padding->next_padding;
                                assert(ppm!=NULL);
                            }
                        } else {

                        }
                        if (product->finish_at < ppm->start_at) {

                            //后有空位

                            ppm->prev_finish_at = product->finish_at;
                        } else {
                            //移除ppm
                            if (ppm->next_padding) {
                                if(a > ppm->prev_finish_at){
                                    ppm->next_padding->prev_padding = product;
                                } else {
                                    ppm->next_padding->prev_padding = ppm->prev_padding;
                                }

                            } else {
                                padding_last_machines[m] = product;
                            }
                            product->next_padding = ppm->next_padding;
                            ppm->next_padding = NULL;
                            ppm->prev_padding = NULL;
                            //if(padding_machines[m] == ppm) padding_machines[m] = NULL;
                            if(padding_last_machines[m] == ppm) padding_last_machines[m] = NULL;
                        }

                        break;
                    }
                } while ((ppm = ppm->next_padding) != NULL);
            }
            if(ppm == NULL){

                if(last_piece_finish_at > machines[m]->finish_at){

                    product->prev_finish_at =  machines[m]->finish_at;
                    product->start_at = last_piece_finish_at;
                    if(padding_machines[m]==NULL) {
                        padding_machines[m] = product;
                        padding_last_machines[m] = product;
                    } else {
                        padding_last_machines[m]->next_padding = product;
                        product->prev_padding = padding_last_machines[m];
                        padding_last_machines[m] = product;
                    }

                } else {
                    product->start_at = machines[m]->finish_at;
                }
                product->prev = machines[m];
                machines[m]->next = product;
                machines[m] = product;
//                padding_last_machines[m] = product;
                product->finish_at = product->start_at+product->cost;
            }


        }
    }
    int maxspan = -1;

    for(int i=1;i<=s->machine_count;i++){
        if(machines[i]->finish_at > maxspan) maxspan = machines[i]->finish_at;
    }
    return maxspan;

}
static void decoder_thread_main(DecoderThreadingState* ts) {
    while (true) {
        if (pthread_mutex_lock(&(ts->mp)) == 0) {
            if (ts->working_at == THREADING_PROCESSING) {

                assert(pthread_mutex_unlock(&(ts->mp)) == 0);
                while (ts->gens_count--) {
                    ts->gen_span[ts->gens_count] = decoder_process(ts, ts->gens[ts->gens_count]);

                    if(ts->_output){
//                        for(int i=0;i<ts->mat->machine_count * ts->mat->machine_count;i++) printf("%d ",ts->gens[ts->gens_count][i]);
                        output(ts->mat,ts->machines);
                    }
                    decoder_clean_thread(ts);
                }
                assert(pthread_mutex_lock(&(ts->mp)) == 0);
                ts->working_at = THREADING_FINISHED;
                assert(pthread_mutex_unlock(&(ts->mp)) == 0);
            } else {
                if (ts->working_at == THREADING_EXIT) {
                    assert(pthread_mutex_unlock(&(ts->mp)) == 0);
                    free(ts->mat->table);
                    free(ts->mat->pure_table);
                    free(ts->mat);
                    free(ts->products);
                    free(ts->machines);
                    free(ts->padding_machines);
                    free(ts->padding_last_machines);
                    return;
                }
                assert(pthread_mutex_unlock(&(ts->mp)) == 0);
                usleep(100);
            }
        }
    }
}
DecoderThreadingState* decoder_create_thread(JSSInput *input,bool _output){
    DecoderThreadingState *thread_state = malloc(sizeof(DecoderThreadingState));
    thread_state->_output = _output;
    int machine_count =  input->machine_count,product_count = input->product_count,len = machine_count*product_count;
    DecoderAdjMat *s = calloc(1, sizeof(DecoderAdjMat));
    s->machine_count = machine_count;
    s->product_count = product_count;
    s->table =  calloc(machine_count*product_count,sizeof(Elem));
    s->pure_table = calloc(machine_count*product_count,sizeof(Elem));
    for(int i=1;i<=s->product_count;i++) //product
        for(int j=1;j<=s->machine_count;j++){ //piece
            Elem *el = ELEM(s,i,j);
            el->cost = input->cost[i][j];
            if(el->cost > 0){
                el->machine  = input->order[i][j];
                el->product = i,el->piece = j;
            }
        }
    memcpy(s->pure_table,s->table,machine_count*product_count*sizeof(Elem));

    thread_state->mat = s;
    thread_state->machines = calloc(machine_count+1, sizeof(Elem*));
    thread_state->padding_machines = calloc(machine_count+1, sizeof(Elem*));
    thread_state->padding_last_machines = calloc(machine_count+1, sizeof(Elem*));
    thread_state->products = calloc(product_count+1, sizeof(Elem*));

    thread_state->mp = PTHREAD_MUTEX_INITIALIZER;
    thread_state->working_at = THREADING_WAITING;
    assert(pthread_mutex_init(&(thread_state->mp),NULL)==0);
//    decoder_thread_main(thread_state);
    assert(pthread_create(&(thread_state->thread),NULL,decoder_thread_main,thread_state)==0);
    return thread_state;
}
int *decoder_thread_destroy(DecoderThreadingState *ts){
    assert(pthread_mutex_lock(&(ts->mp))==0);
    ts->working_at = THREADING_EXIT;
    assert(pthread_mutex_unlock(&(ts->mp))==0);
}
#include <time.h>
void output(DecoderAdjMat* s,Elem **machines) {
    static int i = 0;
    int max = 0;
    for(int i=1;i<=s->machine_count;i++){
        int j=0;
        Elem *el =machines[i];
        if(!el) continue;
        while(el->prev != NULL) el = el->prev;
        printf("M%d ",i);
        while(el != NULL) {
            printf(" (%d,%d-%d,%d)",el->start_at,el->product,el->piece,el->finish_at);
            if(max < el->finish_at) max = el->finish_at;
            el=el->next;
        }
        printf("\n");

    }
    printf("Time used %f.",(double)clock()/1000);
    printf("END %d",max);

}



//int main(){
//    JSSInput input;
//    FILE *in_fp;
//    in_fp = fopen("in.txt","r");
//    jss_read_dataset2(&input,in_fp);
//    dump_jss_input(&input);
//    int j,i=0,arr[100];
//    while(fscanf(in_fp,"%d",&j) > 0) arr[i++] = j+1;
//    optim(&input,arr);
//    return 0;
//}