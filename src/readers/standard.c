/**
 * @file standard.c
 *
 * 交互窗口读入、样例文件读入的解析reader
 *
 */


#include <string.h>
#include "standard.h"

/**
 * 在fp中依照《开发要求v1.4》的输入格式，将输入的文本内容填充到JSSInput结构体中
 * @param target JSSInput结构体指针
 * @param fp 文本内容来源，可以使一个fopen的fp，还可以是stdin (实现中未使用fseek等，可以安全的使用stdin做输入)
 * @return 是否读取成功，若成功`target`会被读取结果覆盖
 */
bool jss_read_standard(JSSInput *target,FILE *fp){
    memset(target,0, sizeof(JSSInput));
    fscanf(fp,"%d%d",&target->product_count,&target->machine_count);
    int product,cost,machine,piece;
    while(fscanf(fp,"%d",&product) > 0 && product != -1){
        piece = 1;
        while(fscanf(fp,"%*[ ](%d,%d)",&cost,&machine) > 0){
            target->cost[product][piece] = cost;
            target->order[product][piece] = machine;
            piece++;
        }
    }
    return true;
}