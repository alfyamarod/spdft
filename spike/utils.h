#ifndef UTILS_SPIKE_H
#define UTILS_SPIKE_H

#include <stdio.h>

#define GLOBAL_VAR static

extern FILE *error_log;

#define StopIf(arg, err_ac, ...)                                               \
  if (arg) {                                                                   \
    fprintf(error_log ? error_log : stderr, __VA_ARGS__);                      \
    fprintf(error_log ? error_log : stderr, "\n");                             \
    err_ac;                                                                    \
  }

#endif
