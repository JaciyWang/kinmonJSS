/*
 * @Author: Nihil Eon
 * @Date: 2018-05-20 15:48:09
 * @Last Modified by: Nihil Eon
 * @Last Modified time: 2018-05-20 15:48:43
 */
#include "geneAlgo.h"
#include <time.h>
 //#include "../algo.h"
#include "../../readers/reader.h"
#include "../gme/gme.h"
#include "../../readers/dataset2.h"
#define MAX_MACHINE 25
 ///染色体的最大长度
#define MAX_LENGTH 1000
///种群中的染色体数
#define POPULATION_SIZE 50
///种群扩大倍数
#define MUTIPLE 20
///染色体改变次数
#define CHROMO_CHANGE_TIMES 10
///每次的种群扩大倍数
#define ENLARGE_TIMES 8
///变异率
#define MUTATION_RATE 0.1
///交叉率
#define CROSSOVER_RATE 0.8
///迭代次数
#define CIRCLE_TIME 20000

DecoderThreadingState* dts[5];
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
 * @return Chromo* 新生成的染色体
 */
static Chromo* crossover(Chromo *parent1, Chromo *parent2, GAState* data) {
	int i;
	Chromo* offspring = malloc(sizeof(Chromo));
	memset(offspring->genes, 0x80, sizeof(offspring->genes));
	offspring->time_cost = 0;
	//memset(offspring->genes, 0xC0, sizeof offspring->genes);
	int process[MAX_JOB] = { 0 };
	for (i = 0; i < data->length; i++)
		if (rand_digit() > 0.5) {
			offspring->genes[i] = parent1->genes[i];
			process[parent1->genes[i]]++;
		}
	int j = 0;
	for (i = 0; i < data->length; i++) {
		if (process[parent2->genes[i]] < data->process_num[parent2->genes[i]])
		{
			while (offspring->genes[j] >= 0)j++;
			offspring->genes[j] = parent2->genes[i];
			process[parent2->genes[i]]++;
		}
	}
	return offspring;
}


/**
 * @brief 进行交叉互换
 *
 * @param chromo_length 染色体长度
 * @param parent 进行变异的染色体
 * @return Chromo* 返回新的染色体
 */
static Chromo* mutation(int chromo_length, Chromo* parent) {
	int position1 = rand() % chromo_length;
	int position2 = rand() % chromo_length;
	int temp_num = parent->genes[position1];
	parent->genes[position1] = parent->genes[position2];
	parent->genes[position2] = temp_num;
	return parent;
}



/**
 * @brief 对单条染色体进行解码
 *
 * @param chromo 输入的染色体
 * @param data 源数据
 * @return int 返回时间的最大值
 */

void compute_DAG(Chromo* chromo, GAState* data) {
	int temp_time[MAX_JOB*MAX_MACHINE] = { 0 };
	int traveled[MAX_JOB] = { 0 };
	int m = data->m;
	int i;
	int end_time[MAX_MACHINE] = { 0 };
	for (i = 0; i < data->length; i++) {
		int position = get_position(chromo->genes[i], traveled[chromo->genes[i]], data);
		int current_machine = get_machine_num(position, data);
		int current_time_cost = get_time_cost(position, data);
		//非第一步
		if (traveled[chromo->genes[i]] > 0) {
			int pre_position = get_position(chromo->genes[i], traveled[chromo->genes[i]] - 1, data);
			int pre_time_cost = get_time_cost(pre_position, data);
			//如果工序上一步的结束时间大于现在机器的结束时间
			if (temp_time[pre_position] + pre_time_cost > end_time[current_machine]) {
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
		traveled[chromo->genes[i]]++;
	}
	int max_timet = -1;
	for (i = 1; i <= m; i++) {
		if (end_time[i] > max_timet)
			max_timet = end_time[i];
	}
	chromo->time_cost = max_timet;

}


/**
 * @brief 初始化种群,生成种群中的size个染色体
 *
 * @param population_size 种群的长度
 * @param data 源数据
 * @return Population* 返回种群
 */
static Population* init_population(GAState * const data, const char *params)
{
	Population* population_data = calloc(sizeof(Population), 1);
	population_data->population_size = POPULATION_SIZE;
	population_data->chromo_size = data->length;
	int i;
	int process_num = 0;
	///make the first ordered array
	int* ordered_list = calloc(sizeof(int), data->m*data->n + 1);
	for (i = 0; i < data->n; i++) {
		int j = 0;
		for (j = 0; j < data->process_num[i]; j++) {
			int position = i * data->m + j;
			ordered_list[process_num] = i;
			process_num++;
		}
	}
	Chromo** population = malloc(sizeof(Chromo*)* population_data->population_size*MUTIPLE);
	for (i = 0; i < population_data->population_size; i++) {
		population[i] = calloc(sizeof(Chromo), 1);
		population[i]->time_cost = 0;
	}
	//全随机生成子染色体
	for (i = 0; i < population_data->population_size; i++) {
		int j;
		for (j = 0; j < CHROMO_CHANGE_TIMES * 1000; j++) {
			int rand1 = (int)(rand() % process_num);
			int rand2 = (int)(rand() % process_num);
			int temp = ordered_list[rand1];
			ordered_list[rand1] = ordered_list[rand2];
			ordered_list[rand2] = temp;
		}
		for (j = 0; j < process_num; j++)
			population[i]->genes[j] = ordered_list[j];
	}
	free(ordered_list);
	population_data->population = population;
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
	int k;
	for (k = 1; k < MUTIPLE; k++) {
		int temp_chromo_num = population_data->current_chromo_num / 2;
		for (i = 0; i < temp_chromo_num / 2; i++) {
			if (rand_digit() < CROSSOVER_RATE) {
				if (rand_digit() < CROSSOVER_RATE) {
					population_data->population[population_data->current_chromo_num] =
						crossover(population_data->population[i], population_data->population[i + population_data->population_size / 2], data);
					population_data->current_chromo_num++;
				}
				if (rand_digit() < MUTATION_RATE) {
					mutation(population_data->chromo_size, population_data->population[population_data->current_chromo_num - 1]);
				}
			}
		}
	}
}


//static void enlarge_population(Population* population_data, GAState* data) {
//	int i;
//	for (i = 0; i < population_data->population_size*(MUTIPLE - 2); i++) {
//		if (rand_digit() < CROSSOVER_RATE) {
//			int chromo1, chromo2;
//			do {
//				chromo1 = (rand() % population_data->population_size);
//				chromo2 = (rand() % population_data->population_size);
//			} while (chromo1 == chromo2);
//			population_data->population[population_data->current_chromo_num] =
//				crossover(population_data->population[chromo1], population_data->population[chromo2], data);
//			population_data->current_chromo_num++;
//		}
//		if (rand_digit() < MUTATION_RATE)
//			mutation(population_data->chromo_size, population_data->population[(rand() % population_data->population_size)]);
//	}
//
//}

/**
 * @brief 缩小种群
 *
 * @param population_data 种群数据
 */
static void lessen_population(Population* population_data) {
	int i = population_data->population_size;
	for (; i < population_data->current_chromo_num; i++) {
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
	int best[100000] = { 0 };
	int i, j;
	Chromo ** population = population_data->population;
	for (i = 0; i < population_data->current_chromo_num; i++)
		for (j = population_data->current_chromo_num - 2; j >= i; j--)
			if (population[j]->time_cost > population[j + 1]->time_cost) {
				Chromo *temparent = population[j];
				population[j] = population[j + 1];
				population[j + 1] = temparent;
			}

	j = population_data->population_size;
	for (i = 0; i < population_data->population_size; i++) {
		best[population[i]->time_cost]++;
		if (best[population[i]->time_cost] > 1) {
			while (j < population_data->current_chromo_num&&best[population[j]->time_cost]>2)
				j++;
			if (j < population_data->current_chromo_num) {
				Chromo *temparent2 = population[i];
				population[i] = population[j];
				population[j] = temparent2;
				best[population[i]->time_cost]++;
				best[population[j]->time_cost]--;
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
int decoder_process(DecoderThreadingState* ts,int *gen);
static void decode(Population* population_data, GAState* data) {
	static  int x = 1;
	int i,count = 0,cc[4] = {0},which[1000][1000] = {0};
	for (i = 0; i < population_data->current_chromo_num; i++)
		if (population_data->population[i]->time_cost == 0){
			decoder_thread_set_gen_count(dts[count % 4],cc[count % 4]+1);
//			decoder_process(dts[4],population_data->population[i]->genes);
			decoder_thread_set_gen(dts[count % 4],cc[count % 4]++,population_data->population[i]->genes);
			which[count % 4][cc[count % 4]] = i;
			count++;
		}
	decoder_thread_tick(dts[0]);
	decoder_thread_tick(dts[1]);
	decoder_thread_tick(dts[2]);
	decoder_thread_tick(dts[3]);
	decoder_thread_wait_finish(dts[0]);
	decoder_thread_wait_finish(dts[1]);
	decoder_thread_wait_finish(dts[2]);
	decoder_thread_wait_finish(dts[3]);
	for(i=0;i<4;i++) for(int j=0;j<cc[i];j++){
			population_data->population[which[i][j]]->time_cost = dts[i]->gen_span[j];
		}



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
	int temp_time = population_data->population[0]->time_cost;

	for (i = 0; i < CIRCLE_TIME; i++) {
		enlarge_population(population_data, data);
		decode(population_data, data);

		find_best_chromo(population_data);
		lessen_population(population_data);
		temp_time = population_data->population[0]->time_cost;
	}

	compute_DAG(population_data->population[0], data);
	printf("%d\n", temp_time);
}

/**
 * @brief 向源数据加入时间
 *
 * @param chromo 输入最佳的染色体
 * @param data 源数据
 */
static void add_time(Chromo* chromo, GAState* data)
{
	memset(data->S, 0, sizeof(int)*data->m*data->n);
	int traveled[MAX_JOB] = { 0 };
	int m = data->m;
	int i;
	int end_time[MAX_MACHINE] = { 0 };
	for (i = 0; i < data->length; i++) {
		int position = get_position(chromo->genes[i], traveled[chromo->genes[i]], data);
		int current_machine = get_machine_num(position, data);
		int current_time_cost = get_time_cost(position, data);
		//非第一步
		if (traveled[chromo->genes[i]] > 0) {
			int pre_position = get_position(chromo->genes[i], traveled[chromo->genes[i]] - 1, data);
			int pre_machine = get_machine_num(pre_position, data);
			int pre_time_cost = get_time_cost(pre_position, data);
			//如果工序上一步的结束时间大于现在机器的结束时间
			if (data->S[pre_position] + pre_time_cost > end_time[current_machine]) {
				end_time[current_machine] = data->S[pre_position] + pre_time_cost + current_time_cost;
				data->S[position] = data->S[pre_position] + pre_time_cost;

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
		traveled[chromo->genes[i]]++;
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
	int** process_location = malloc(sizeof(int)* data->n);
	int j;
	for (j = 0; j < data->n; j++)
		process_location[j] = calloc(sizeof(int), 2);
	for (i = 1; i <= data->m; i++) {
		/// NO.i machine
		for (j = 0; j < data->n; j++)
			memset(process_location[j], 0, sizeof(process_location[0]));
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
	free(population_data);
}

static int epoch_once(GAState* data){
	Population* population_data = init_population(data, NULL);
	circle_to_find_best(population_data, data);
//	add_time(population_data->population[0], data);
	decoder_thread_set_gen_count(dts[4],1);
	decoder_thread_set_gen(dts[4],0,population_data->population[0]->genes);

	decoder_thread_tick(dts[4]);
	decoder_thread_wait_finish(dts[4]);
	population_destroy(population_data);
}

int main() {
	srand(time(NULL));
	FILE* fp = fopen("in.txt", "r");
	GAState* data = ga_load_from_file(fp);
	JSSInput input;
	fseek(fp,SEEK_SET,0);
	jss_read_dataset2(&input,fp);
	init_population(data,NULL);
	dts[0] = decoder_create_thread(&input,0);
	dts[1] = decoder_create_thread(&input,0);
	dts[2] = decoder_create_thread(&input,0);
	dts[3] = decoder_create_thread(&input,0);
	dts[4] = decoder_create_thread(&input,true);
	epoch_once(data);
//	random_algo(data);
	return 0;
}

JSSAlgo gene_algo = {
	.name = "gene",
	.init = &init_population,
	.destroy = ga_destory,
	.iter_once = epoch_once,
	.output = ga_write_to_file
};
