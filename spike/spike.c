#include "spike.h"
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void spk_set_sim(spk_params_t *spk_params) {

  u32 pops = spk_params->num_pops;

  for (u32 i = 0; i < pops; i++) {
  }

  // TODO each population has its own process. the threading and simd to perform
  // the neuron simulation

  /* for (u32 i = 0; i < spk_params->num_pops; i++) { */
  /*   int pid = fork(); */
  /*   StopIf(pid < 0, exit(EXIT_FAILURE), "spike fork() error"); */
  /*   if (pid > 0) { */

  /*   } */
  /* } */
}

// void spk_run_sim() {}
