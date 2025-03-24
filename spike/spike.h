#ifndef SPIKE_SPDFT_H
#define SPIKE_SPDFT_H

#include "utils.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct neuron_t neuron_t;
// typedef struct population_params population_params;

void neuron_initialise(neuron_t *params);

void neuron_state_update(uint16_t num_exc_inputs, const double *exc_input,
                         uint16_t num_inh_inputs, const double *inh_input,
                         neuron_t *neuron, uint32_t timer_count, uint32_t time);

bool neuron_has_spiked(neuron_t *neuron);

typedef struct {
  u32 num_neurons;
  neuron_t *neurons;
} population_t;

// typedef struct connection_t connection_t;

typedef struct {
  u32 num_pops;
  population_t **populations;
} spk_params_t;

void spk_set_sim(spk_params_t *spk_params);

#endif
