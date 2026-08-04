#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include <stdbool.h>
#include <stdint.h>

void i2c_master_initialize(void);
bool i2c_master_start(void);
bool i2c_master_stop(void);
bool i2c_master_write(uint8_t data);

#endif
