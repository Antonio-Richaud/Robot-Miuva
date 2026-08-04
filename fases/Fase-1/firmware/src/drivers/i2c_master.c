#include <xc.h>
#include "i2c_master.h"

#define I2C_TIMEOUT 60000u

static bool i2c_wait_idle(void)
{
    uint16_t timeout = I2C_TIMEOUT;

    while (((SSP1CON2 & 0x1Fu) != 0u) || SSP1STATbits.R_nW)
    {
        if (--timeout == 0u)
        {
            return false;
        }
    }

    return true;
}

void i2c_master_initialize(void)
{
    /* RB0 = SDA1 y RB1 = SCL1. En I2C deben permanecer como entradas. */
    ANSELBbits.ANSB0 = 0;
    ANSELBbits.ANSB1 = 0;
    TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;

    SSP1CON1 = 0x00;
    SSP1CON2 = 0x00;
    SSP1STAT = 0x80; /* Slew rate deshabilitado para 100 kHz. */

    /* Fscl = Fosc / (4 * (SSP1ADD + 1))
     * 100 kHz con Fosc = 12 MHz -> SSP1ADD = 29.
     */
    SSP1ADD = 29u;

    PIR1bits.SSP1IF = 0;
    SSP1CON1bits.SSPM = 0b1000; /* I2C Master mode. */
    SSP1CON1bits.SSPEN = 1;
}

bool i2c_master_start(void)
{
    uint16_t timeout = I2C_TIMEOUT;

    if (!i2c_wait_idle())
    {
        return false;
    }

    PIR1bits.SSP1IF = 0;
    SSP1CON2bits.SEN = 1;

    while (!PIR1bits.SSP1IF)
    {
        if (--timeout == 0u)
        {
            return false;
        }
    }

    PIR1bits.SSP1IF = 0;
    return true;
}

bool i2c_master_stop(void)
{
    uint16_t timeout = I2C_TIMEOUT;

    if (!i2c_wait_idle())
    {
        return false;
    }

    PIR1bits.SSP1IF = 0;
    SSP1CON2bits.PEN = 1;

    while (!PIR1bits.SSP1IF)
    {
        if (--timeout == 0u)
        {
            return false;
        }
    }

    PIR1bits.SSP1IF = 0;
    return true;
}

bool i2c_master_write(uint8_t data)
{
    uint16_t timeout = I2C_TIMEOUT;

    if (!i2c_wait_idle())
    {
        return false;
    }

    PIR1bits.SSP1IF = 0;
    SSP1BUF = data;

    while (!PIR1bits.SSP1IF)
    {
        if (--timeout == 0u)
        {
            return false;
        }
    }

    PIR1bits.SSP1IF = 0;

    /* ACKSTAT = 0 significa que el esclavo reconocio el byte. */
    return (SSP1CON2bits.ACKSTAT == 0u);
}
