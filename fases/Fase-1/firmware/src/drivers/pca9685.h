#ifndef PCA9685_H
#define PCA9685_H

#include <stdbool.h>
#include <stdint.h>

bool pca9685_probe(void);
bool pca9685_initialize_50hz(void);
bool pca9685_set_channel_pulse(uint8_t channel, uint16_t pulse_count);

#endif
