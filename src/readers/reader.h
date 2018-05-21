/**
 * @file reader.h
 * @brief Common header across all readers.
 *
 * 从交互窗口读入、样例文件读入、测试集读入共用的头文件
 */

#ifndef KINMON_READER_H
/**
 * @struct JSSInput
 * @brief 输入数据，所有reader的读取结果
 *
 * 所有reader的读取结果
 */
typedef struct {
    int machine_count,product_count;
/** @brief 工序表
 * 工序表(order[产品ID][工序数] = 机器ID)
 * 注: 产品ID和工序数均从1开始，若某产品在某项工序后没有进一步加工需求，令后续的工序的机器ID为0
 */
int order[100][100];
/** @brief 工序耗时表
 * 工序耗时表(cost[产品ID][工序数] = 耗时)
 */
int cost[100][100];
} JSSInput;

#define KINMON_READER_H

#endif //KINMON_READER_H
