/*
 * @Author: Nihil Eon
 * @Date: 2018-05-20 15:48:09
 * @Last Modified by: Nihil Eon
 * @Last Modified time: 2018-05-20 15:48:43
 */
#include "geneAlgo.h"
#include "../algo.h"
///最大的工作数
#define MAX_JOB 105
///最大的机器数
#define MAX_MACHINE 25
///染色体的最大长度
#define MAX_LENGTH 1000
///种群中的染色体数
#define POPULATION_SIZE 30
///种群扩大倍数
#define MUTIPLE 5
///染色体改变次数
#define CHROMO_CHANGE_TIMES 3
///每次的种群扩大倍数
#define ENLARGE_TIMES 5
///变异率
#define MUTATION_RATE 0.3
///交叉率
#define CORSSOVER_RATE 0.4
///迭代次数
#define CIRCLE_TIME 1000


/**
 * @brief Get the position
 *
 * @param i 横坐标
 * @param j 纵坐标
 * @param data 储存输入数据的结构
 * @return 坐标
 */
static int get_position(int i, int j, GAState *data) {
    return i * data->m + j;
}

/**
 * @brief Get the time cost in data
 *
 * @param position 当前位置
 * @param data 储存输入数据的结构
 * @return 所需的时间
 */
static int get_time_cost(int position, GAState *data) {
    return data->T[position];
}

/**
 * @brief 得到当前机器号
 *
 * @param position 当前位置
 * @param data 储存输入数据的结构
 * @return 机器号
 */
static int get_machine_num(int position, GAState *data) {
    return data->P[position];
}

/**
 * @brief Set the start time object
 *
 * @param value 输入的时间
 * @param position 当前位置
 * @param data 储存输入数据的结构
 */
static void set_start_time(int value, int position, GAState* data) {
    data->S[position] = value;
}

/**
 * @brief 进行交叉
 *
 * @param parent1 父染色体
 * @param parent2 母染色体
 * @param data 源数据
 * @return int* 新生成的染色体
 */
static int* crossover(int *parent1,int *parent2,GAState* data) {
    int i;
    int* offspring = malloc(sizeof(int)*(data->length + 1));
    memset(offspring, 0xC0, sizeof offspring);
    int process[MAX_JOB] = { 0 };
    for(i=0;i<data->length;i++)
        if(rand_digit()>0.5) {
            offspring[i] = parent1[i];
            process[parent1[i]]++;
        }
    int j=0;
    for(i=0;i<data->length;i++)
        if(process[parent2[i]]<data->process_num[parent2[i]])
            while (offspring[j] >= 0)j++;
            offspring[j] = parent2[i];
            process[parent2[i]]++;
    return offspring;
}


/**
 * @brief 进行交叉互换
 *
 * @param chromo_length 染色体长度
 * @param parent 进行变异的染色体
 * @return int* 返回新的染色体
 */
static int* mutation(int chromo_length, int * parent){
    int position1, position2;
        position1 = rand_shift() % chromo_length;
        position2 = rand_shift() % chromo_length;
    int temp_num = parent[position1];
    parent[position1] = parent[position2];
    parent[position2] = temp_num;
    return parent;
}



/**
 * @brief 对单条染色体进行解码
 *
 * @param chromo 输入的染色体
 * @param data 源数据
 * @return int 返回时间的最大值
 */
static int compute_DAG(int* chromo, GAState* data) {
    int temp_time[MAX_JOB*MAX_MACHINE] = { 0 };
    int traveled[MAX_JOB] = { 0 };
    int m = data->m;
    int i;
    int end_time[MAX_MACHINE] = { 0 };
    for (i = 0; i < data->length; i++) {
        int position = get_position(chromo[i], traveled[chromo[i]], data);
        int current_machine = get_machine_num(position, data);
        int current_time_cost = get_time_cost(position, data);
        //非第一步
        if (traveled[chromo[i]] > 0) {
            int pre_position = get_position(chromo[i], traveled[chromo[i]] - 1, data);
            int pre_time_cost = get_time_cost(pre_position, data);
            int pre_machine = get_machine_num(pre_position, data);
            //如果工序上一步的结束时间大于现在机器的结束时间
            if (temp_time[pre_position] + pre_time_cost> end_time[current_machine]) {
                end_time[current_machine] = temp_time[pre_position] + pre_time_cost + current_time_cost;
                temp_time[position] = temp_time[pre_position] + pre_time_cost;
            }
            else {
                temp_time[position] = end_time[current_machine];
                end_time[current_machine] += current_time_cost;
            }
        }
        else {//为第一步
            temp_time[position] = end_time[current_machine];
            end_time[current_machine] += current_time_cost;
        }
        traveled[chromo[i]]++;
    }
    int max_time = 0;
    for (i = 1; i <= m; i++) {
        if (end_time[i] > max_time)
            max_time = end_time[i];
    }
    return max_time;
}


/**
 * @brief 初始化种群,生成种群中的size个染色体
 *
 * @param population_size 种群的长度
 * @param data 源数据
 * @return Population* 返回种群
 */
static Population* init_population(GAState * const data,const char *params)
{
    Population* population_data = calloc(sizeof(Population), 1);
    population_data->population_size = POPULATION_SIZE;
    population_data->chromo_size = data->length;
    int i;
    int process_num = 0;
    ///make the first ordered array
    int* ordered_list = calloc(sizeof(int), data->m*data->n);
    for (i = 0; i < data->n; i++) {
        int j = 0;
        for (j = 0; j < data->process_num[i]; j++) {
            int position = i * data->m + j;
            ordered_list[process_num] = i;
            process_num++;
        }
    }
    int *time_cost = calloc(sizeof(int), population_data->population_size*MUTIPLE);
    int ** population = malloc(sizeof(int*)* population_data->population_size*MUTIPLE);
    for (i = 0; i < population_data->population_size; i++) {
        population[i] = calloc(sizeof(int), process_num);
    }
    //全随机生成子染色体
    for (i = 0; i < population_data->population_size; i++) {
        int j;
        for (j = 0; j < CHROMO_CHANGE_TIMES; j++) {
            int rand1 = (int)(rand_shift() % process_num);
            int rand2 = (int)(rand_shift() % process_num);
            int temp = ordered_list[rand1];
            ordered_list[rand1] = ordered_list[rand2];
            ordered_list[rand2] = temp;
        }
        for (j = 0; j < process_num; j++)
            population[i][j] = ordered_list[j];
    }
    free(ordered_list);
    population_data->population = population;
    population_data->time_cost = time_cost;
    population_data->current_chromo_num = population_data->population_size;
    return population_data;
}

/**
 * @brief 扩大种群
 *
 * @param population_data 种群数据
 * @param data 源数据
 */
static void enlarge_population(Population* population_data, GAState* data) {
    int i;
    for (i = 0; i < population_data->population_size*(MUTIPLE - 2); i++) {
        int chromo1, chromo2;
        do {
            chromo1 = (rand_shift() % population_data->population_size);
            chromo2 = (rand_shift() % population_data->population_size);
        } while (chromo1 == chromo2);
        population_data->population[population_data->current_chromo_num] =
            crossover(population_data->population[chromo1], population_data->population[chromo2], data);
        population_data->current_chromo_num++;
    }
    if (rand_digit() < MUTATION_RATE)
        mutation(population_data->chromo_size, population_data->population[(rand_shift() % population_data->population_size)]);
}

/**
 * @brief 缩小种群
 *
 * @param population_data 种群数据
 */
static void lessen_population(Population* population_data) {
    int i = population_data->population_size;
    for (; i < population_data->current_chromo_num; i++) {
        population_data->time_cost[i] = 0;
        free(population_data->population[i]);
    }
    population_data->current_chromo_num = population_data->population_size;
}


/**
 * @brief 找到最好的size个
 *
 * @param population_data 种群数据
 */
static void find_best_chromo(Population* population_data) {
    int best[10000] = { 0 };
    int i, j;
    int ** population = population_data->population;
    int * t = population_data->time_cost;
    for (i = 0; i < population_data->current_chromo_num; i++)
        for (j = population_data->current_chromo_num - 2; j >= i; j--)
            if (t[j] > t[j + 1]) {
                int temp = t[j];
                t[j] = t[j + 1];
                t[j + 1] = temp;
                int *temparent2 = population[j];
                population[j] = population[j + 1];
                population[j + 1] = temparent2;

            }
    j = population_data->population_size;
    for (i = 0; i < population_data->population_size; i++) {
        best[t[i]]++;
        if (best[t[i]] > 2) {
            while (j<population_data->current_chromo_num&&best[t[j]]>2)
                j++;
            if (j < population_data->current_chromo_num) {
                    int temp = t[i];
                    t[i] = t[j];
                    t[j] = temp;
                    int *temparent2 = population[i];
                    population[i] = population[j];
                    population[j] = temparent2;
                    best[t[i]]++;
                    best[t[j]]--;
                    j++;
            }
        }
    }
}

/**
 * @brief 解码
 *
 * @param population_data 种群数据
 * @param data 源数据
 */
static void decode(Population* population_data, GAState* data) {
    int i;
    for (i = 0; i < population_data->current_chromo_num; i++)
        if (population_data->time_cost[i] == 0)
            population_data->time_cost[i] = compute_DAG(population_data->population[i], data);
}

/**
 * @brief 进行迭代
 *
 * @param population_data  种群数据
 * @param data 源数据
 */
static void circle_to_find_best(Population* population_data, GAState* data) {
    int i;
    enlarge_population(population_data, data);
    decode(population_data, data);
    find_best_chromo(population_data);
    lessen_population(population_data);
    int temp_time = population_data->time_cost[0];
    for (i = 0; i < CIRCLE_TIME; i++) {
        enlarge_population(population_data, data);
        decode(population_data, data);
        find_best_chromo(population_data);
        lessen_population(population_data);
        temp_time = population_data->time_cost[0];
    }
    printf("%d\n", temp_time);
}

/**
 * @brief 向源数据加入时间
 *
 * @param chromo 输入最佳的染色体
 * @param data 源数据
 */
static void add_time(int *chromo, GAState* data)
{
    memset(data->S, 0, sizeof(int)*data->m*data->n);
    int traveled[MAX_JOB] = { 0 };
    bool is_empty[MAX_MACHINE];
    int m = data->m;
    int n = data->n;
    int i;
    for (i = 0; i < m + 1; i++)
        is_empty[i] = true;
    int end_time[MAX_MACHINE] = { 0 };
    int first_end_time = 0;
    int last_min_time = 0;
    for (i = 0; i < data->length; i++) {
        int position = get_position(chromo[i], traveled[chromo[i]], data);
        int current_machine = get_machine_num(position, data);
        int current_time_cost = get_time_cost(position, data);
        // if (is_empty[current_machine]) {
        //非第一步
        if (traveled[chromo[i]] > 0) {
            int pre_position = get_position(chromo[i], traveled[chromo[i]] - 1, data);
            int pre_machine = get_machine_num(pre_position, data);
            int pre_time_cost = get_time_cost(pre_position, data);
            //如果工序上一步的结束时间大于现在机器的结束时间
            if (data->S[pre_position]+pre_time_cost > end_time[current_machine]) {
                data->S[position] = data->S[pre_position] + pre_time_cost;
                end_time[current_machine] = data->S[pre_position] + pre_time_cost + current_time_cost;
            }
            else {
                data->S[position] = end_time[current_machine];
                end_time[current_machine] += current_time_cost;
            }
        }
        else {//为第一步
            data->S[position] = end_time[current_machine];
            end_time[current_machine] += current_time_cost;
        }
        traveled[chromo[i]]++;
    }
}

/**
 * @brief 打印时间
 *
 * @param data 源数据
 */
static void print_time(GAState* data)
{
    //机器号 (起始,产品-工序,结束)
    int i;
    int max_time = 0;
    int** process_location = malloc(sizeof(int)* data->m);
    for (i = 1; i <= data->m; i++) {
        /// NO.i machine
        int j;
        memset(process_location, 0, sizeof(int)* data->m);
        for (j = 0; j < data->m; j++)
            process_location[j] = calloc(sizeof(int), 2);
        int process_num = 0;
        for (j = 0; j < data->n; j++) {
            ///in j row k col ,totally n row m col
            int k;
            for (k = 0; k < data->m; k++)
                if (data->P[j * data->m + k] == i) {
                    process_location[process_num][0] = j;
                    process_location[process_num][1] = k;
                    process_num++;
                    break;
                }
        }
        ga_sort(process_location, data, process_num);
        printf("M%d", i);
        for (j = 0; j < process_num; j++) {
            int row = process_location[j][0];
            int col = process_location[j][1];
            int current_position = get_position(row, col, data);
            int start_time = data->S[current_position];
            int end_time = start_time + data->T[current_position];
            if (end_time > max_time) max_time = end_time;
            printf(" (%d,%d-%d,%d)",
                start_time,
                row + 1,
                col + 1,
                end_time
            );
        }
        putchar('\n');
        int k;
        for (k = 0; k < data->m; k++)
            free(process_location[k]);
    }
    //free(process_location);
    printf("END %d", max_time);
}

/**
 * @brief 删除整个种群
 *
 * @param population_data 种群数据
 */
static void population_destroy(Population* population_data) {
    int i;
    for (i = 0; i < population_data->current_chromo_num; i++)
        free(population_data->population[i]);
    free(population_data->population);
    free(population_data->time_cost);
    free(population_data);
}

// int main() {
//     FILE* fp = fopen("../test.in", "r");
//     GAState* data = ga_load_from_file(fp);
//     Population* population_data = init_population(data);
//     circle_to_find_best(population_data, data);
//     add_time(population_data->population[0], data);
//     print_time(data);
//     population_destroy(population_data);
//     //random_algo(data);
//     return 0;
// }

JSSAlgo gene_algo = {
    .name = "gene",
    .init = &init_population,
    .destroy = ga_destory,
    .iter_once = epoch_once,
    .output = ga_write_to_file
};
