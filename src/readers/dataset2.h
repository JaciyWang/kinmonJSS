/**
 * @file dataset.c
 *
 * 另一种测试集的读入
 *  http://mistic.heig-vd.ch/taillard/problemes.dir/ordonnancement.dir/ordonnancement.html
 */

#ifndef KINMON_DATASET2_H
#define KINMON_DATASET2_H

#include <stdio.h>
#include <stdbool.h>
#include "reader.h"
bool jss_read_dataset2(JSSInput *target,FILE *fp);
#endif //KINMON_DATASET2_H
