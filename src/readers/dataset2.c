//
// Created by heymi on 2018/5/21.
//

#include "dataset2.h"
bool jss_read_dataset2(JSSInput *target,FILE *fp){
    memset(target,0, sizeof(JSSInput));
    fscanf(fp,"%d%d",&target->product_count,&target->machine_count);
    for(int i=1;i<=target->product_count;i++)
        for(int j=1;j<=target->machine_count;j++) {
            fscanf(fp, "%d", &target->order[i][j]);
            target->order[i][j]++;
            fscanf(fp, "%d", &target->cost[i][j]);
        }
    return true;
}