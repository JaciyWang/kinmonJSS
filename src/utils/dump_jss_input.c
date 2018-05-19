/**
 * @file dump_jss_input.c
 *
 * 验证reader是否正常工作
 */



#include "dump_jss_input.h"
/**
 * 输出读入的JSSInput看看是否读入正确
 * @param target
 */
void dump_jss_input(const JSSInput *target){
    for(int i=0;i<=target->product_count;i++) {
        for (int j = 0; j <= target->machine_count; j++)
            if (i == 0 && j == 0) printf("COST\t");
            else if (i == 0) printf("P%d\t", j);
            else if (j == 0) printf("J%d\t", i);
            else printf("%d\t", target->cost[i][j]);
        printf("\n");
    }
    for(int i=0;i<=target->product_count;i++) {
        for (int j = 0; j <= target->machine_count; j++)
            if (i == 0 && j == 0) printf("ORDER\t");
            else if (i == 0) printf("P%d\t", j);
            else if (j == 0) printf("J%d\t", i);
            else printf("%d\t", target->order[i][j]);
        printf("\n");
    }
}