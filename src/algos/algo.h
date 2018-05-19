/**
 * @file algo.h
 *
 * 可插拔算法抽象
 *  *可插拔*
 *  本程序设计可同时执行多种算法，通过命令行参数或者GUI选择，因此特将每种算法用结构体表示，每个算法结构体代表一个算法。
 *  本程序会在`config.h`设置一个`kinmon_algos`数组，通过遍历该数组可以得到所有算法。
 *
 *  *易扩展*
 *  考虑到智能算法的迭代次数多，运行时间长。在命令行交互或图形界面交互式会阻塞线程，造成程序卡顿，影响用户操作体验。所以对算法的抽象进行改进，
 *  由于算法执行的过程是不断地迭代以寻找最优解，且每次迭代迭代运算量少，速度快故将其每次迭代的过程单独抽象成`JSSAlgoIterOnce`函数，这样无论是
 *  命令交互还是图形界面交互都可以控制算法的进行，从而避免算法执行起来阻塞线程无法相应其他事件。
 */

#ifndef KINMON_PLUGABLE_ALGO_H
#define KINMON_PLUGABLE_ALGO_H

#include <stdio.h>
#include "../readers/reader.h"


/**
 * @struct JSSAlgo
 * @brief 可插拔算法结构
 * 具体使用方式见文档《如何加入一种调度算法》
 *
 */
typedef struct {
/**
 * @brief 算法名称，字符串常量
 **/
    char *name;
/**
 * @brief 算法初始化函数类型
 * 请在这里初始化算法实例，分配内存和初始化相关结构
 * @param input JSSInput结构体指针，即算法的输入数据
 * @param params 算法执行的参数(由cli或gui提供)需要算法内部解析
 * @return 该算法实例的指针，算法需要仅通过该指针保存该实例的数据，此指针会在调用其他算法函数的时候作为首参数传入，调用者会保证在调用结束后调用JSSAlgoDestroy函数
 */
    void *(*init)(const JSSInput *input, const char *params);

/**
 * @brief 算法执行后调度结果的输出函数类型
 * 注：该方法会在算法未完全迭代前调用，亦会在完全迭代后调用。以动态获得算法调度过程中调度方式和调度时间的动态变化。故需注意对中间调度结果的保存。
 * @param payload 该算法实例的指针
 * @param fp 输出的目标文件指针(格式为《项目需求v1.4》中规定的输出格式)
 */
    void (*output)(void *payload, FILE *fp);

/**
 * @brief 算法迭代一次的函数类型
 * 请保证该函数不会长时间进行，调用者会多次调用该函数，直至该函数返回`-1`
 * @param payload 该算法实例的指针
 * @return 此时刻的最优解，如果算法执行完毕，请返回`-1`
 */
    int (*iter_once)(void *payload);

/**
 * @brief 算法实例的清理函数类型
 * 请在这里释放分配的内存
 * @param payload 该算法实例的指针
 */
    void (*destroy)(void *payload);
} JSSAlgo;


#endif //KINMON_PLUGABLE_ALGO_H
