#ifndef ADC_H
#define ADC_H

#include <stdint.h>

void adc_initialize(void);
uint16_t adc_read(uint8_t channel);
uint16_t adc_read_average(uint8_t channel, uint8_t samples);

#endif
