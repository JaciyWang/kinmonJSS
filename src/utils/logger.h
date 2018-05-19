/**
 * @file logger.h
 *
 * @author heymind
 * @date 2018-05-14
 */
#ifndef KINMON_LOGGER_H

#include <stdio.h>

#ifdef LOG_LEVEL_INFO
#define NEED_LOGGER_INFO (1)
#define NEED_LOGGER_DEBUG (1)
#define NEED_LOGGER_ASSERT (1)
#else
#ifdef LOG_LEVEL_DEBUG
#define NEED_LOGGER_DEBUG (1)
#define NEED_LOGGER_ASSERT (1)
#else
#define NEED_LOGGER_ASSERT (1)
#endif
#endif

#ifdef NEED_LOGGER_INFO
/**
 * 输出运行时信息使用方法类似于printf，只不过可以通过宏来控制是否进行输出
 * @param format 格式
 * @param valist 参数列表
 */
#define LOGGER_INFO(fmt,...) printf("[I|%s:%s(%d)]:" fmt "\n",  __FILE__,__FUNCTION__, __LINE__,__VA_ARGS__)
#undef NEED_LOGGER_INFO
#endif

#ifdef NEED_LOGGER_DEBUG
/**
 * 输出调试信息
 * @param format 格式
 * @param valist 参数列表
 */
#define LOGGER_DEBUG(fmt,...) printf("[DEBUG|%s:%s(%d)]:" fmt "\n",  __FILE__,__FUNCTION__, __LINE__,__VA_ARGS__)
#undef NEED_LOGGER_DEBUG
#endif


#ifdef NEED_LOGGER_ASSERT
/**
 * 断言表达式为真，用于提前发现潜在问题
 * @param expr 表达式
 * @param format 格式
 * @param valist 参数列表
 */
#define LOGGER_ASSERT(expr, fmt, ...) if(!(expr)) printf("[ASSERT FAILED|%s:%s(%d)]:" fmt "\n", __FILE__,__FUNCTION__, __LINE__,__VA_ARGS__)
#undef NEED_LOGGER_ASSERT
#endif


#define KINMON_LOGGER_H

#endif //KINMON_LOGGER_H
