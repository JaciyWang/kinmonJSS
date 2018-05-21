/**
 * @file cli.c
 * @brief Command Line Entry File
 *
 * 命令行交互的可执行程序
 *
 */
#include <stdbool.h>
#include <string.h>
#include "../readers/reader.h"
#include "../readers/standard.h"
#include "../readers/dataset.h"
#include "../config.h"
#include "../algos/algo.h"
#include "../cli/cli.h"
#include "../utils/dump_jss_input.h"


int main(int argc,char **argv){

    bool succ = 0;
    CLIParseResult params;
    if(!cli_parse(&params,argc,argv)){
        printf("Wrong params.");
        return 0;
    }
    FILE *in_fp,*out_fp;
    if(strcmp(params.in,"stdin")==0){
        in_fp = stdin;
    } else {
        in_fp = fopen(params.in,"r");
        if(!in_fp) {printf("Read file &d err.",params.in);
        return 0;}
    }
    if(strcmp(params.out,"stdout")==0){
        out_fp = stdout;
    } else {
        out_fp = fopen(params.out,"w+");
        if(!out_fp) {printf("Open output file &d err.",params.out);
            return 0;}
    }

    JSSInput input;
    if(params.in_type == CLI_STANDARD_FILE_INPUT){
        jss_read_standard(&input,in_fp);
    } else {
        jss_read_dataset(&input,in_fp);
    }
    dump_jss_input(&input);

    const JSSAlgo *algo = &(kinmon_algos[params.use_algo]);
    void *algo_state = algo->init(&input,NULL);
    int time,max_count = 1000;
    while(time != -1 && max_count--){
        algo->iter_once(algo_state);
    }
    algo->output(algo_state,out_fp);
    algo->destroy(algo_state);

    return 0;
}