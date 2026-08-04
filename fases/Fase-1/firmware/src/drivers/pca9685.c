#include <xc.h>

#include "pca9685.h"
#include "i2c_master.h"

#define PCA9685_ADDRESS_7BIT 0x40u
#define PCA9685_WRITE_BYTE   ((PCA9685_ADDRESS_7BIT << 1) | 0u)

#define PCA9685_MODE1        0x00u
#define PCA9685_MODE2        0x01u
#define PCA9685_LED0_ON_L    0x06u
#define PCA9685_PRESCALE     0xFEu

#define PCA9685_MODE1_SLEEP_AI   0x30u
#define PCA9685_MODE1_WAKE_AI    0x20u
#define PCA9685_MODE1_RESTART_AI 0xA0u
#define PCA9685_MODE2_TOTEM_POLE 0x04u
#define PCA9685_PRESCALE_50HZ    121u

static bool pca9685_write_register(uint8_t reg, uint8_t value)
{
    bool ok = true;

    ok = ok && i2c_master_start();
    ok = ok && i2c_master_write(PCA9685_WRITE_BYTE);
    ok = ok && i2c_master_write(reg);
    ok = ok && i2c_master_write(value);
    ok = i2c_master_stop() && ok;

    return ok;
}

static bool pca9685_write_channel(uint8_t channel, uint16_t on_count, uint16_t off_count)
{
    uint8_t reg;
    bool ok = true;

    if ((channel > 15u) || (on_count > 4095u) || (off_count > 4095u))
    {
        return false;
    }

    reg = (uint8_t)(PCA9685_LED0_ON_L + (4u * channel));

    ok = ok && i2c_master_start();
    ok = ok && i2c_master_write(PCA9685_WRITE_BYTE);
    ok = ok && i2c_master_write(reg);
    ok = ok && i2c_master_write((uint8_t)(on_count & 0xFFu));
    ok = ok && i2c_master_write((uint8_t)((on_count >> 8) & 0x0Fu));
    ok = ok && i2c_master_write((uint8_t)(off_count & 0xFFu));
    ok = ok && i2c_master_write((uint8_t)((off_count >> 8) & 0x0Fu));
    ok = i2c_master_stop() && ok;

    return ok;
}

bool pca9685_probe(void)
{
    bool acknowledged;

    if (!i2c_master_start())
    {
        return false;
    }

    acknowledged = i2c_master_write(PCA9685_WRITE_BYTE);
    (void)i2c_master_stop();

    return acknowledged;
}

bool pca9685_initialize_50hz(void)
{
    if (!pca9685_probe())
    {
        return false;
    }

    if (!pca9685_write_register(PCA9685_MODE1, PCA9685_MODE1_SLEEP_AI))
    {
        return false;
    }

    if (!pca9685_write_register(PCA9685_PRESCALE, PCA9685_PRESCALE_50HZ))
    {
        return false;
    }

    if (!pca9685_write_register(PCA9685_MODE2, PCA9685_MODE2_TOTEM_POLE))
    {
        return false;
    }

    if (!pca9685_write_register(PCA9685_MODE1, PCA9685_MODE1_WAKE_AI))
    {
        return false;
    }

    __delay_ms(1);

    return pca9685_write_register(PCA9685_MODE1, PCA9685_MODE1_RESTART_AI);
}

bool pca9685_set_channel_pulse(uint8_t channel, uint16_t pulse_count)
{
    return pca9685_write_channel(channel, 0u, pulse_count);
}
