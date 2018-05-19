/**
 * @file config.h
 *
 * 配置文件
 */

#ifndef KINMON_CONFIG_H
#define KINMON_CONFIG_H

#include "algos/algo.h"
#include "algos/bigant/bigant.h"

#ifndef CLI_DEFAULT_IN
/**
 * 命令行版本默认输入来源，使用宏定义可在CMakeFileList.txt覆盖，从而用同一套代码生成不同需求的可执行程序
 */
#define CLI_DEFAULT_IN "stdin"
#endif


#ifndef CLI_DEFAULT_OUT
/**
 * 命令行版本默认输出目标
 */
#define CLI_DEFAULT_OUT "stdout"
#endif

#ifndef CLI_DEFAULT_ALGO
/**
 * 命令行版本默认的算法
 */
#define CLI_DEFAULT_ALGO 0
#endif

/**
 *  作者
 *  TODO:放上英文ID，中文会乱码
 */
#define KINMON_AUTHORS "heymind , & "

/**
 *  可插拔算法注册数组
 */
extern JSSAlgo** kinmon_algos;
#define kinmon_algos_count  (1)



#endif //KINMON_CONFIG_H
