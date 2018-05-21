/*
 * @Author: Nihil Eon
 * @Date: 2018-05-20 15:48:29
 * @Last Modified by:   Nihil Eon
 * @Last Modified time: 2018-05-20 15:48:29
 */
#include "GAfile.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*!
* @file GAfile.c * jobs begin wtih 0 machines begin with 1 so that we can check whether the job has been finished
* by search for the machine number in the process.
*
*/


/**
 * @brief 输入坐标,得到位置
 *
 * @param i 横坐标
 * @param j 纵坐标
 * @param data 元数据
 * @return int 返回一维坐标
 */
static int ga_get_position(int i, int j, GAState *data) {
	return i * data->m + j;
}

/**
 * @brief Get the time cost in data
 *
 * @param position 当前位置
 * @param data 储存输入数据的结构
 * @return 所需的时间
 */
static int ga_get_time_cost(int position, GAState *data) {
	return data->T[position];
}

/**
 * @brief 得到当前机器号
 *
 * @param position 当前位置
 * @param data 储存输入数据的结构
 * @return 机器号
 */
static int ga_get_machine_num(int position, GAState *data) {
	return data->P[position];
}

/**
 * @brief Set the start time object
 *
 * @param value 输入的时间
 * @param position 当前位置
 * @param data 储存输入数据的结构
 */
static void ga_set_start_time(int value, int position, GAState* data) {
	data->S[position] = value;
}

/**
 * @brief 对数据进行排序
 *
 * @param temp_position 当前坐标
 * @param data 元数据
 * @param process_num 该机器所进行的工序数
 */
void ga_sort(int* temp_position[2], const GAState* data, int process_num) {
	int i;
	for (i = 0; i < process_num; i++) {
		int j;
		for (j = process_num - 2; j >= i; j--) {
			int this_position = ga_get_position(temp_position[j][0], temp_position[j][1], data);
			int next_position = ga_get_position(temp_position[j + 1][0], temp_position[j + 1][1], data);
			if (data->S[this_position] > data->S[next_position]) {
				int x = temp_position[j][0];
				int y = temp_position[j][1];
				temp_position[j][0] = temp_position[j + 1][0], temp_position[j][1] = temp_position[j + 1][1];
				temp_position[j + 1][0] = x;
				temp_position[j + 1][1] = y;
			}
		}
	}
}

/**
 * @brief 读取文件数据
 * 时间和机器号间隔输入
 * @param fp 文件指针
 * @return GAState* 返回得到的元数据
 */
GAState* ga_load_from_file(FILE* fp) {
	int total_process_num = 0;
	int n, m; ///m machines n jobs
	rewind(fp);
	GAState* data = (GAState*)malloc(sizeof(GAState));
	fscanf(fp, "%d %d\n", &n, &m);
	data->n = n;
	data->m = m;
	data->T = (int*)calloc(sizeof(int), m * n);
	data->P = (int*)calloc(sizeof(int), m * n);
	data->S = (int*)calloc(sizeof(int), m * n);
	data->process_num = calloc(sizeof(int), n);
	int i, j;
	for (i = 0; i < n; i++)
		for (j = 0; j < m; j++) {
			fscanf(fp, "%d", &data->P[i*m + j]);
			fscanf(fp, "%d", &data->T[i*m + j]);
			data->P[i*m + j]++;
		}
	for (i = 0; i<n; i++)
		data->process_num[i] = m;
	data->length = n * m;
	return data;
}

//T P分别输入
// GAState* ga_load_from_file(FILE* fp) {
// 	int total_process_num = 0;
// 	int n, m; ///m machines n jobs
// 	rewind(fp);
// 	GAState* data = (GAState*)malloc(sizeof(GAState));
// 	fscanf(fp,"%d %d\n", &n, &m);
// 	data->n = n;
// 	data->m = m;
// 	data->T = (int*)calloc(sizeof(int), m * n);
// 	data->P = (int*)calloc(sizeof(int), m * n);
// 	data->S = (int*)calloc(sizeof(int), m * n);
// 	data->process_num = calloc(sizeof(int), n);
// 	int i,j;
// 	for(i=0;i<n;i++)
// 		for (j = 0; j < m; j++)
// 			fscanf(fp, "%d", &data->T[i*m+j]);
// 	for(i=0;i<n;i++)
// 		for(j=0;j<m;j++)
// 			fscanf(fp, "%d", &data->P[i*m + j]);
// 	for(i=0;i<n;i++)
// 		data->process_num[i] = m;
// 	data->length = n * m;
// 	return data;
// }

//课程输入(文件版本)
// GAState* ga_load_from_file(FILE* fp) {
//    int total_process_num = 0;
// 	int n, m; ///m machines n jobs
// 	rewind(fp);
// 	GAState* data = (GAState*)malloc(sizeof(GAState));
// 	fscanf(fp,"%d %d\n", &n, &m);
//     data->n = n;
//     data->m = m;
// 	data->T = (int*)calloc(sizeof(int), m * n);
// 	data->P = (int*)calloc(sizeof(int), m * n);
// 	data->S = (int*)calloc(sizeof(int), m * n);
// 	data->process_num = calloc(sizeof(int), n);
// 	int ordinal;
// 	fscanf(fp,"%d", &ordinal);
// 	while (ordinal != -1) {
// 		int i = 0;
// 		while (fgetc(fp) != '\n') {
// 			fscanf(fp,"(%d,%d)", &data->T[(ordinal -1) * m + i], &data->P[(ordinal -1) * m + i]);
// 			i++;
// 		}
//        total_process_num += i;
// 		data->process_num[ordinal - 1] = i;
// 		/// ordinal= ga_change_to_num(fp);
// 		fscanf(fp,"%d", &ordinal);
// 	}
// 	return data;
// }

//输入(命令行版本)
// GAState* ga_load_from_file(FILE* fp) {
//    int total_process_num = 0;
// 	int n, m; ///m machines n jobs
// 	rewind(fp);
// 	GAState* data = (GAState*)malloc(sizeof(GAState));
// 	scanf("%d %d\n", &n, &m);
//     data->n = n;
//     data->m = m;
// 	data->T = (int*)calloc(sizeof(int), m * n);
// 	data->P = (int*)calloc(sizeof(int), m * n);
// 	data->S = (int*)calloc(sizeof(int), m * n);
// 	data->process_num = calloc(sizeof(int), n);
// 	int ordinal;
// 	scanf("%d", &ordinal);
// 	while (ordinal != -1) {
// 		int i = 0;
// 		while (getchar()!= '\n') {
// 			scanf("(%d,%d)", &data->T[(ordinal -1) * m + i], &data->P[(ordinal -1) * m + i]);
// 			i++;
// 		}
//        total_process_num += i;
// 		data->process_num[ordinal - 1] = i;
// 		/// ordinal= ga_change_to_num(fp);
// 		scanf("%d", &ordinal);
// 	}
// 	return data;
// }

/**
 * @brief 输出到文件
 *
 * @param data 元数据
 * @param fp 文件指针
 * @return true 成功
 */
void ga_write_to_file(const GAState* data, FILE* fp) {
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
		fprintf(fp,"M%d", i);
		for (j = 0; j < process_num; j++) {
			int row = process_location[j][0];
			int col = process_location[j][1];
			int current_position = ga_get_position(row, col, data);
			int start_time = data->S[current_position];
			int end_time = start_time + data->T[current_position];
			if (end_time > max_time) max_time = end_time;
			fprintf(fp," (%d,%d-%d,%d)",
				start_time,
				row + 1,
				col + 1,
				end_time
			);
		}
		fputc('\n',fp);
	}
	//free(process_location);
	fprintf(fp,"END %d", max_time);
}

/**
 * @brief 删除元数据
 *
 * @param ex 结束时的元数据
 */
void ga_destory(GAState* ex) {
	free(ex->T);
	free(ex->P);
	free(ex->S);
	free(ex->process_num);
	free(ex);
}

