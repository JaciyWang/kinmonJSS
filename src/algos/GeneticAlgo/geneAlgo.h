/*
 * @Author: Nihil Eon
 * @Date: 2018-05-20 15:48:19
 * @Last Modified by:   Nihil Eon
 * @Last Modified time: 2018-05-20 15:48:19
 */
#pragma once
#include "GAfile.h"
#include "GARandGenerate.h"
#include <string.h>
#include <stdint.h>
#define MAX_JOB 105
 ///最大的机器数
#define MAX_MACHINE 25
 ///染色体的最大长度

typedef struct
{
	int genes[MAX_JOB*MAX_MACHINE];
	int time_cost;
}Chromo;

/**
 * 储存种群信息
 */
typedef struct
{
    /**
     * 储存种群中的所有染色体
     */
    Chromo** population;
    /**
     * 种群的k值,即染色体的稳定个数
     */
    int population_size;
    /**
    * 每个染色体的基因长度
    */
    int chromo_size;
    /**
     * 现在种群中染色体的个数
     */
    int current_chromo_num;
}Population;
extern JSSAlgo gene_algo;