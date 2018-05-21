/*
 * @Author: Nihil Eon
 * @Date: 2018-05-20 15:48:14
 * @Last Modified by: Nihil Eon
 * @Last Modified time: 2018-05-20 15:49:31
 */
#include "GARandGenerate.h"
/* The state word must be initialized to non-zero */
/**
 * 生成小数时处以的倍数
 */
#define DIVIDE_TIMES 100
///随机加入的三个非零数
static unsigned long x =241312, y=213423 , z =12412343;
/**
 * 得到一个随机整数
 */
 unsigned long rand_shift(void) {          //period 2^96-1
    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;
    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;
    return z;
}
/**
 * 得到一个0,1之间的小数
 */
double rand_digit() {
    return (double)(rand_shift() % DIVIDE_TIMES) / DIVIDE_TIMES;
}