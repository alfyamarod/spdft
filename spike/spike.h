#ifndef SPIKE_SPDFT_H
#define SPIKE_SPDFT_H

#include <stdbool.h>
#include <stdint.h>

typedef struct neuron_t neuron_t;
// typedef struct population_params population_params;

void neuron_initialise(neuron_t *params);

void neuron_state_update(uint16_t num_exc_inputs, const double *exc_input,
                         uint16_t num_inh_inputs, const double *inh_input,
                         neuron_t *neuron, uint32_t timer_count, uint32_t time);

bool neuron_has_spiked(neuron_t *neuron);

typedef struct population_t population_t;

void population_initialise(population_t *population);

typedef struct connection_t connection_t;

#endif
