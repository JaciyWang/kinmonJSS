/*
 * @Author: Nihil Eon
 * @Date: 2018-05-20 00:02:55
 * @Last Modified by: Nihil Eon
 * @Last Modified time: 2018-05-20 15:48:40
 */
#pragma once
#include <stdio.h>
#include <stdbool.h>
/**
 * @brief 源数据
 *
 */
typedef struct {
    /**
     * m个机器,n个工件
     */
    int m,n;
    /**
     * 储存每一个工序所花费时间
     */
    int* T;
    /**
     * 储存每一个工序的所在机器号
     */
    int* P;
    /**
     * 储存没一个工序的开始时间
     */
    int* S;
    /**
     * 储存每一个工件的工序数
     */
    int* process_num;
    /**
     * 所有工序总和
     */
    int length;
} GAState;

GAState* ga_load_from_file(FILE* fp);

bool ga_write_to_file(const GAState* data, FILE* fp);

void ga_sort(int* a[2], const GAState* data, int process_num);

void ga_destory(GAState* ex);
