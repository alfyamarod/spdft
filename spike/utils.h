#ifndef UTILS_SPIKE_H
#define UTILS_SPIKE_H

#include <stdint.h>
#include <stdio.h>

#define GLOBAL_VAR static

typedef uint32_t u32;
typedef uint8_t u8;

extern FILE *error_log;

#define StopIf(arg, err_ac, ...)                                               \
  if (arg) {                                                                   \
    fprintf(error_log ? error_log : stderr, __VA_ARGS__);                      \
    fprintf(error_log ? error_log : stderr, "\n");                             \
    err_ac;                                                                    \
  }

#endif
