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
/**
 * 储存种群信息
 */
typedef struct
{
    /**
     * 每一步花费的时间
     */
    int *time_cost;
    /**
     * 储存种群中的所有染色体
     */
    int** population;
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
