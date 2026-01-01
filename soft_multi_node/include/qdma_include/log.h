#pragma once
#include <cassert>
#define LOG_FATAL 0
#define LOG_ERROR 1
#define LOG_WARNING 2
#define LOG_INFO 3
#define LOG_DEBUG 4
#define MAX_LOGMSG_LEN    1024 
void print_stack_trace();
