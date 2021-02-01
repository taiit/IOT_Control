#ifndef __LOG_H__
#define __LOG_H__

#define INFO_STR "Info"

#define log_helper(level, fmt, ...) printf("[%s] %s:%d " fmt, #level , __FILE__, __LINE__, ##__VA_ARGS__)

#define log_debug(fmt, ...) log_helper(INFO, fmt, ##__VA_ARGS__)

#endif
