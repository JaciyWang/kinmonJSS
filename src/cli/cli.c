/**
 * @file cli.c
 * @brief Command Line Interface
 *
 * 命令行交互
 *
 */

#include "cli.h"
#include <stdlib.h>
#include <string.h>
#include "../config.h"

/**
 * @brief 输出所有在可插拔算法数组中注册的算法名称
 */
static void print_available_algos(){
    for(int i = 0;i<kinmon_algos_count;i++) printf("%s ",((JSSAlgo* )*(&kinmon_algos+i))->name);
}
/**
 * @brief 输出帮助
 */
static void print_help(){
    printf("Kinmon JSS Program (made by %s)\n\n"
            "Usages:\n"
            "-h,--help\t\tPrint this document.\n"
            "-i [filename]\t\tSpecify the input file.Default to %s.\n"
            "-o [filename]\t\tSpecify the output file.Default to %s.\n"
            "--dataset\t\tParse the input file with dataset format.Please read the document for more details.\n"
            "--algo [algoname]\tDefault to %s.Available algos:"
            ,KINMON_AUTHORS,CLI_DEFAULT_IN,CLI_DEFAULT_OUT,((JSSAlgo* )(kinmon_algos+CLI_DEFAULT_ALGO))->name);
    print_available_algos();
    printf(".\n\n");
    exit(0);
}
/**
 * @brief 将CLIParseResult结构体设置为默认值
 * @param[in] result CLIParseResult结构体指针
 */
static void set_as_default(CLIParseResult *result){
    result->in = "stdin";
    result->out = "stdout";
    result->in_type = CLI_STANDARD_FILE_INPUT;
    result->use_algo = CLI_DEFAULT_ALGO;
}

/**
 * @brief 解析cli输入
 * @param[out] result 解析结果
 * @param argc
 * @param argv
 * @return 是否解析成功
 */
bool cli_parse(CLIParseResult *result,int argc,char** argv){
    set_as_default(result);
    int i = 1;
    while(i < argc){
        if(strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"--help") == 0){
            print_help();
        } else if(strcmp(argv[i],"-i") == 0){
            result->in = argv[++i];
        } else if(strcmp(argv[i],"-o") == 0){
            result->out = argv[++i];
        } else if(strcmp(argv[i],"--dataset") == 0){
            result->in_type = CLI_DATASET_FILE_INPUT;
        } else if(strcmp(argv[i],"--algo") == 0){
            char *algo_name = argv[++i];
            for(int j = 0;j<kinmon_algos_count;j++){
                if(strcmp(((JSSAlgo* )*(&kinmon_algos+j))->name,algo_name)==0) {
                    result->use_algo = j;
                    i++;
                    continue;
                }
            }
            printf("ERR:Unknown algo.");
            return  false;
        }else{
            print_help();
            return  false;
        }
        i++;
    }
    return true;
}
