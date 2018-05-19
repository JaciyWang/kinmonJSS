/**
 * @file dataset.h
 *
 * 从测试集读入解析的reader
 *  http://mistic.heig-vd.ch/taillard/problemes.dir/ordonnancement.dir/ordonnancement.html
 */


#ifndef KINMON_READER_DATASET_H
#define KINMON_READER_DATASET_H

#include <stdio.h>
#include <stdbool.h>
#include "reader.h"

bool jss_read_dataset(JSSInput *target,FILE *fp);

#endif //KINMON_READER_DATASET_H
