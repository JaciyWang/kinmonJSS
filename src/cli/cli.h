/**
 * @file cli.h
 * @brief Command Line Interface
 *
 * 命令行交互
 */

#ifndef KINMON_CLI_H

#include <stdbool.h>

/**
 * @brief  输入文件的类型
 */
typedef enum {
    /**
     * @brief  《开发要求v1.4》的输入格式
     */
    CLI_STANDARD_FILE_INPUT = 0,
    /**
     * @brief  测试集的输入格式
     */
    CLI_DATASET_FILE_INPUT
} CLIInputFileType;

/**
 * @brief 命令行参数解析结果结构体
 */
typedef struct {
    /**
     * @brief 输入文件，可以是stdin
     */
    char *in;
    /**
     * @brief 输出文件，可以是stdout
     */
    char *out;
    /**
     * @brief 输入文件的类型
     */
    CLIInputFileType in_type;
    /**
     * @brief 使用第几个算法
     */
    int use_algo;
} CLIParseResult;

bool cli_parse(CLIParseResult *result, int argc, char **argv);

#define KINMON_CLI_H

#endif //KINMON_CLI_H
