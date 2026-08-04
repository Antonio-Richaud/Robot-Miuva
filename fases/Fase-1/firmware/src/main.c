/**
 * Robot-Miuva - Fase 1
 * Prueba de deteccion I2C del PCA9685.
 *
 * Cableado esperado:
 *   Miuva USB/5V -> PCA9685 VCC
 *   Miuva GND    -> PCA9685 GND
 *   Miuva RB0    -> PCA9685 SDA
 *   Miuva RB1    -> PCA9685 SCL
 *
 * No conectar V+, servos ni fuente externa durante esta prueba.
 *
 * Indicadores:
 *   Verde lento -> PCA9685 detectado en la direccion 0x40.
 *   Rojo rapido -> no hubo respuesta I2C.
 */

#define _XTAL_FREQ 12000000UL

#include <xc.h>
#include <stdbool.h>

#include "drivers/i2c_master.h"
#include "drivers/pca9685.h"

#define LED_GREEN_LAT LATEbits.LATE0
#define LED_RED_LAT   LATEbits.LATE1
#define LED_BLUE_LAT  LATEbits.LATE2

#define LED_ON  0u
#define LED_OFF 1u

static void system_initialize(void)
{
    /* RE0, RE1 y RE2 se utilizan como salidas digitales. */
    ANSELE = 0x00;

    LED_GREEN_LAT = LED_OFF;
    LED_RED_LAT = LED_OFF;
    LED_BLUE_LAT = LED_OFF;

    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;
}

static void show_pca9685_detected(void)
{
    LED_RED_LAT = LED_OFF;
    LED_BLUE_LAT = LED_OFF;

    LED_GREEN_LAT = LED_ON;
    __delay_ms(700);
    LED_GREEN_LAT = LED_OFF;
    __delay_ms(700);
}

static void show_i2c_error(void)
{
    LED_GREEN_LAT = LED_OFF;
    LED_BLUE_LAT = LED_OFF;

    LED_RED_LAT = LED_ON;
    __delay_ms(120);
    LED_RED_LAT = LED_OFF;
    __delay_ms(120);
}

int main(void)
{
    bool pca9685_detected;

    system_initialize();
    __delay_ms(100);

    i2c_master_initialize();
    pca9685_detected = pca9685_probe();

    while (1)
    {
        if (pca9685_detected)
        {
            show_pca9685_detected();
        }
        else
        {
            show_i2c_error();
        }
    }
}
