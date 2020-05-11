#include <stdint.h>

#define LOG(x) fprintf(stderr, x);

#define LOG_FMT(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__);

#define DEBUG_LOG(x) LOG_FMT("[%s|%s|%i] %s\n", __FILE__, __FUNCTION__, __LINE__, x);

#define DEBUG_EMPTY_LOG DEBUG_LOG("")
