/**
 * Robot-Miuva - Fase 1
 * Prueba de centrado de un SG90 mediante PCA9685.
 *
 * Canal utilizado: 0
 * Frecuencia PWM: 50 Hz
 * Pulso inicial: 1500 us aprox. (307 cuentas de 4096)
 *
 * Indicadores:
 *   Verde fijo -> PCA9685 inicializado y canal 0 centrado.
 *   Rojo rapido -> error de comunicacion o configuracion.
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

#define SERVO_CHANNEL_0       0u
#define SERVO_CENTER_1500_US 307u

static void system_initialize(void)
{
    ANSELE = 0x00;

    LED_GREEN_LAT = LED_OFF;
    LED_RED_LAT = LED_OFF;
    LED_BLUE_LAT = LED_OFF;

    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;
}

static void show_success(void)
{
    LED_RED_LAT = LED_OFF;
    LED_BLUE_LAT = LED_OFF;
    LED_GREEN_LAT = LED_ON;
}

static void show_error_forever(void)
{
    LED_GREEN_LAT = LED_OFF;
    LED_BLUE_LAT = LED_OFF;

    while (1)
    {
        LED_RED_LAT = LED_ON;
        __delay_ms(120);
        LED_RED_LAT = LED_OFF;
        __delay_ms(120);
    }
}

int main(void)
{
    system_initialize();
    __delay_ms(100);

    i2c_master_initialize();

    if (!pca9685_initialize_50hz())
    {
        show_error_forever();
    }

    if (!pca9685_set_channel_pulse(SERVO_CHANNEL_0, SERVO_CENTER_1500_US))
    {
        show_error_forever();
    }

    show_success();

    while (1)
    {
        __delay_ms(1000);
    }
}
