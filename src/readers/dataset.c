/**
 * @file dataset.c
 *
 * 从测试集读入解析的reader
 *  http://mistic.heig-vd.ch/taillard/problemes.dir/ordonnancement.dir/ordonnancement.html
 */
#include "dataset.h"
#include <string.h>
/**
 * 依据http://mistic.heig-vd.ch/taillard/articles.dir/Taillard1993EJOR.pdf文件定义的格式读入
 * @param target JSSInput结构体指针
 * @param fp 文本内容来源，可以使一个fopen的fp，还可以是stdin (实现中未使用fseek等，可以安全的使用stdin做输入)
 * @return 是否读取成功，若成功`target`会被读取结果覆盖
 */
bool jss_read_dataset(JSSInput *target,FILE *fp){
    memset(target,0, sizeof(JSSInput));
    fscanf(fp,"%d%d",&target->product_count,&target->machine_count);
    for(int i=1;i<=target->product_count;i++)
        for(int j=1;j<=target->machine_count;j++)
            fscanf(fp,"%d",&target->cost[i][j]);
    for(int i=1;i<=target->product_count;i++)
        for(int j=1;j<=target->machine_count;j++)
            fscanf(fp,"%d",&target->order[i][j]);
    return true;
}