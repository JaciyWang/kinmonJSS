/**
 * @file bigant.c
 * @brief Customized Ant Colony Algorithm (Deprecated)
 *
 * 定制版蚁群算法(已弃用)
 */
#include "bigant.h"
#include "../algo.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>


#define BIG_ANT_MAX_DUR (9000)
#define BIG_ANT_DEFAULT_PHEROMONE_VOLATILE (1)
#define BIG_ANT_DEFAULT_INSPIRE_VOLATILE (10)
#define BIG_ANT_DEFAULT_ENCOURAGE_RATE (1)
#define BIG_ANT_DEFAULT_DISCOURAGE_RATE (1)


typedef struct {
    int pheromone_volatile, inspire_volatile, encourage_rate, discourage_rate;
    int machine_count, product_count, iter_per_epoch, average_cost;
    int inspire_epoch;
    int min_duration, average_duration,average_duration_base,average_duration_change, iter_total_duration, now_iter, epoch;
    int *cost_table, *order_table, *pheromone_table, *inspire_table,*record;
} BigAntState;

static BigAntState* init(const JSSInput *input,const char *params){
    printf("!!!!!请勿使用此算法，此算法存在内存泄露且已经弃用。\n");
    printf("!!!!!请勿使用此算法，此算法存在内存泄露且已经弃用。\n");
    printf("!!!!!请勿使用此算法，此算法存在内存泄露且已经弃用。\n");
    //Step2.1 分配状态结构体内存&初始状态
    BigAntState *state = calloc(1, sizeof(BigAntState));
    state->pheromone_volatile = BIG_ANT_DEFAULT_PHEROMONE_VOLATILE;
    state->inspire_volatile = BIG_ANT_DEFAULT_INSPIRE_VOLATILE;
    state->encourage_rate = BIG_ANT_DEFAULT_ENCOURAGE_RATE;
    state->discourage_rate = BIG_ANT_DEFAULT_DISCOURAGE_RATE;
    int machine_count =input->machine_count, product_count= input->product_count;
    state->machine_count = input->machine_count;
    state->product_count = input->product_count;
    state->record = calloc( input->machine_count * input->product_count, sizeof(int));
    state->cost_table = calloc(machine_count * product_count, sizeof(int));
    state->order_table = calloc(machine_count * product_count, sizeof(int));
    state->pheromone_table = calloc(BIG_ANT_MAX_DUR * machine_count * product_count, sizeof(int));
    state->inspire_table = calloc(BIG_ANT_MAX_DUR * machine_count * product_count, sizeof(int));

    //Step2.2 拷贝`input`数据。如果算法抽象与输入抽象一直可直接memcpy
    for (int i = 0; i < machine_count; i++)
        for (int j = 0; j < product_count; j++) {
            state->cost_table[j * machine_count + i] = input->cost[j+1][i+1];
            state->average_cost += state->cost_table[j * machine_count + i];
        }
    state->average_cost /= machine_count * product_count;
    for (int i = 0; i < machine_count; i++)
        for (int j = 0; j < product_count; j++)
            state->order_table[j * machine_count + i] = input->order[j+1][i+1];

    state->iter_per_epoch = 10;
    state->inspire_epoch = 10;
    //Step2.3 返回结构体指针
    return state;
}


static inline double pheromone(BigAntState *state, int time, int machine, int product) {
    static int last_time = -1, machine_count;
    static int *pheromone_time_block_ptr = NULL;
    static size_t block_size = 0;
    static int pheromone_volatile;
    if (block_size == 0) {
        block_size = (size_t) state->machine_count * state->product_count;
        machine_count = state->product_count;
        pheromone_volatile = state->pheromone_volatile;
    }
    if (time != last_time) {
        pheromone_time_block_ptr = state->pheromone_table + time * block_size;
        last_time = time;
    }
    int *pheromone_ptr =
            pheromone_time_block_ptr + machine_count * (machine - 1) + product - 1, pheromone = *pheromone_ptr;
    if (pheromone == 0 || (pheromone > 0 && pheromone - pheromone_volatile * state->epoch < 0) ||
        (pheromone < 0 && pheromone + pheromone_volatile * state->epoch > 0))
        return 1.0;

    if(pheromone>0) pheromone = pheromone - pheromone_volatile * state->epoch; else pheromone =pheromone + pheromone_volatile * state->epoch;
    //printf("|%d|",pheromone);
    return 2 / (1+pow(2, (-0.005 * pheromone )));
}
static inline void update_pheromone(BigAntState *state, int time, int machine, int product,int encourage) {
    static int last_time = -1, machine_count;
    static int *pheromone_time_block_ptr = NULL;
    static size_t block_size = 0;
    static int pheromone_volatile;
    if (block_size == 0) {
        block_size = (size_t) state->machine_count * state->product_count;
        machine_count = state->product_count;
        pheromone_volatile = state->pheromone_volatile;
    }
    if (time != last_time) {
        pheromone_time_block_ptr = state->pheromone_table + time * block_size;
        last_time = time;
    }
    int *pheromone_ptr =
            pheromone_time_block_ptr + machine_count * (machine - 1) + product - 1, pheromone = *pheromone_ptr;
    if (pheromone == 0 || (pheromone > 0 && pheromone - pheromone_volatile * state->epoch < 0) ||
        (pheromone < 0 && pheromone + pheromone_volatile * state->epoch > 0))
        pheromone = 0;
    if(pheromone>0) pheromone = pheromone - pheromone_volatile * state->epoch; else pheromone =pheromone + pheromone_volatile * state->epoch;
    pheromone+=encourage;
    if(pheromone>0)  *pheromone_ptr = pheromone + pheromone_volatile * state->epoch; else *pheromone_ptr = pheromone - pheromone_volatile * state->epoch;
    //  printf("((%d))",*pheromone_ptr);
}


static inline double inspire(const BigAntState *state, int time, int finish_at, int product, int piece) {
    return 2 /(1+pow(2, (-0.005 * (500 * (state->cost_table[(product - 1) * (state->product_count) + piece - 1] /
                                          state->average_cost) - state->inspire_volatile * state->epoch))));
}

#define GET(x, i, j) ((x)[(i-1)*product_count+(j-1)])
#define SET(x, i, j, v) ((x)[(i-1)*product_count+(j-1)] = (v))

static void iter_once(BigAntState *state) {
    int machine_count = state->machine_count,
            product_count = state->product_count;
    int *product_finish_at = calloc(product_count + 1, sizeof(int)),
            *product_process_at = calloc(product_count + 1, sizeof(int)),
            *machine_process_product = calloc(machine_count + 1, sizeof(int)),
            *machine_finish_at = calloc(machine_count + 1, sizeof(int)),
            *record = state->record;
    memset(record,0,machine_count *product_count * sizeof(int));
    double *machine_process_possibility = calloc(machine_count + 1, sizeof(double));
    int time = 1, next_time = 1;
    int count = 0;


    bool finished = false;

    while (!finished) {
        finished = true;
        for (int i = 1; i <= product_count; i++) {
            if (product_process_at[i] != -1 && product_finish_at[i] <= time) {
                double possibility;
                if (product_process_at[i] == machine_count) {
                    product_process_at[i] = -1;
                    continue;
                }
                int machine = GET(state->order_table, i, product_process_at[i] + 1);
                if (machine == 0) {
                    printf("\nimpossible!\n");
                    product_process_at[i] = -1;
                    continue;
                }
                finished = false;
                if (state->epoch < state->inspire_epoch) {
                    possibility = pow(fabs(pheromone(state, time, machine, i)), -10) *
                                  inspire(state, time, 0, i, product_process_at[i] + 1);
                } else {
                    possibility = pow(pheromone(state, time, machine, i), state->epoch * 0.1) *
                                  inspire(state, time, 0, i, product_process_at[i] + 1);
                    //printf("[%f,%f,%f]\n",pheromone(state, time, machine, i), pow(pheromone(state, time, machine, i), state->epoch * 0.1) ,inspire(state, time, 0, i, product_process_at[i] + 1));
                }
                if (machine_finish_at[machine] <= time) {
                    machine_process_possibility[machine] = possibility;
                    machine_finish_at[machine] = time + GET(state->cost_table, i, ++product_process_at[i]);
                    machine_process_product[machine] = i;
                    product_finish_at[i] = machine_finish_at[machine];
                    record[(machine - 1) * (machine_count) + (i - 1)] = time;
                    count++;
                } else if (possibility - machine_process_possibility[machine] > 0.01) {
                    //fallback
                    int origin_product = machine_process_product[machine];
                    product_finish_at[origin_product] =
                            product_finish_at[origin_product] - GET(state->cost_table, origin_product,
                                                                    product_process_at[origin_product]);
                    product_process_at[origin_product]--;
                    //forward
                    machine_process_possibility[machine] = possibility;
                    machine_finish_at[machine] = time + GET(state->cost_table, i, ++product_process_at[i]);
                    machine_process_product[machine] = i;
                    product_finish_at[i] = machine_finish_at[machine];
                    //count++;
                    record[(machine - 1) * (machine_count) + (i - 1)] = time;
                }
            }
        }
        for (int i = 1; i <= product_count; i++)
            if ((product_finish_at[i] > time) && (next_time == time || product_finish_at[i] < next_time))
                next_time = product_finish_at[i];
        time = next_time;
    }


    if(state->min_duration == 0|| time<state->min_duration) state->min_duration = time;

    //处理信息素
    int temp = 0, encourage = 0;
    temp = (100 * (state->min_duration - time));
    if (temp > 0) state->min_duration = time;
    encourage += temp > 0 ? temp : 0;
    if (state->average_duration != 0) encourage +=state->average_duration - time > 0 ? (100 * (state->average_duration - time)) : state->average_duration - time; else encourage = -10;
    state->iter_total_duration += time;
    state->now_iter++;
    for (int i = 1; i <= machine_count; i++)
        for (int j = 1; j <= product_count; j++) {
            int _time = record[(i - 1) * machine_count + (j - 1)];
            update_pheromone(state,_time,i,j,encourage);
//            state->pheromone_table[_time * machine_count * product_count + (i - 1) * machine_count + j -
//                                   1] += encourage;
        }
    //printf("(epoch=%d,iter=%d,total_cost=%d,routine=", state->epoch, state->now_iter, time);
    for (int k = 0; k < machine_count * product_count; k++) {
        temp = INT_MAX;
        int p = 0;
        int m = 0;
        for (int i = 1; i <= machine_count; i++)
            for (int j = 1; j <= product_count; j++) {
                int _time = record[(i - 1) * machine_count + (j - 1)];
                if (_time != 0 && _time < temp) {
                    temp = _time;
                    p = j;
                    m = i;
                }
            }
        record[(m - 1) * machine_count + (p - 1)] = 0;
//        printf("%d", p);
    }
//    printf(")\n");
    free(product_finish_at);
    free(product_process_at);
    free(machine_process_product);
    free(machine_finish_at);
    free(machine_process_possibility);
}

static int epoch_once(BigAntState *state) {
    state->iter_total_duration = 0;
    state->now_iter = 0;
    if (state->epoch == 0) {
        iter_once(state);
        state->average_duration = state->iter_total_duration;
        state->average_duration_base = state->iter_total_duration;
    }
    else {
        for (int i = 0; i < state->iter_per_epoch; i++) iter_once(state);
        state->average_duration_change = (state->average_duration_change * state->epoch * state->iter_per_epoch + state->iter_total_duration -state->average_duration_base* state->iter_per_epoch)/((state->epoch+1)* state->iter_per_epoch);
        state->average_duration =state->average_duration_base +state->average_duration_change;
    }
    printf("=====epoch=%d===now min = %d ====av=%d=======\n",state->epoch,state->min_duration,state->average_duration );
    state->epoch++;
    return state->min_duration;
}

static void output(BigAntState *state,FILE *fp){
    fprintf(fp,"min:%d\n",state->min_duration);
}

static void destory(BigAntState *state){
    //WARNING:此算法未完善已弃用，此处有内存泄露
    free(state);
}


JSSAlgo bigant_algo = {
        .name = "bigant[deprecated]",
        .init = &init,
        .destroy = destory,
        .iter_once = epoch_once,
        .output = output
};