/**
 * Robot-Miuva - Fase 1
 * Movimiento suave y limitado de dos SG90 mediante PCA9685.
 *
 * Canales utilizados: 0 y 1
 * Frecuencia PWM: 50 Hz
 * Rango de prueba: 1450 us a 1550 us aprox.
 * Movimiento: ambos servos avanzan en sentidos opuestos alrededor del centro.
 *
 * Indicadores:
 *   Verde fijo -> PCA9685 inicializado y movimiento activo.
 *   Rojo rapido -> error de comunicacion o configuracion.
 */

#define _XTAL_FREQ 12000000UL

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#include "drivers/i2c_master.h"
#include "drivers/pca9685.h"

#define LED_GREEN_LAT LATEbits.LATE0
#define LED_RED_LAT   LATEbits.LATE1
#define LED_BLUE_LAT  LATEbits.LATE2

#define LED_ON  0u
#define LED_OFF 1u

#define SERVO_CHANNEL_0     0u
#define SERVO_CHANNEL_1     1u
#define SERVO_MIN_COUNT   297u
#define SERVO_CENTER_COUNT 307u
#define SERVO_MAX_COUNT   317u
#define SERVO_STEP_DELAY_MS 30u
#define SERVO_END_PAUSE_MS 500u

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

static bool set_servo_pair(uint16_t pulse_channel_0)
{
    uint16_t pulse_channel_1;

    pulse_channel_1 = (uint16_t)(SERVO_MIN_COUNT + SERVO_MAX_COUNT - pulse_channel_0);

    if (!pca9685_set_channel_pulse(SERVO_CHANNEL_0, pulse_channel_0))
    {
        return false;
    }

    __delay_ms(5);

    return pca9685_set_channel_pulse(SERVO_CHANNEL_1, pulse_channel_1);
}

static bool move_pair_smooth(uint16_t start_count, uint16_t end_count)
{
    uint16_t pulse;

    if (start_count <= end_count)
    {
        for (pulse = start_count; pulse <= end_count; pulse++)
        {
            if (!set_servo_pair(pulse))
            {
                return false;
            }

            __delay_ms(SERVO_STEP_DELAY_MS);
        }
    }
    else
    {
        pulse = start_count;

        while (1)
        {
            if (!set_servo_pair(pulse))
            {
                return false;
            }

            if (pulse == end_count)
            {
                break;
            }

            pulse--;
            __delay_ms(SERVO_STEP_DELAY_MS);
        }
    }

    return true;
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

    if (!set_servo_pair(SERVO_CENTER_COUNT))
    {
        show_error_forever();
    }

    show_success();
    __delay_ms(1000);

    if (!move_pair_smooth(SERVO_CENTER_COUNT, SERVO_MIN_COUNT))
    {
        show_error_forever();
    }

    while (1)
    {
        if (!move_pair_smooth(SERVO_MIN_COUNT, SERVO_MAX_COUNT))
        {
            show_error_forever();
        }

        __delay_ms(SERVO_END_PAUSE_MS);

        if (!move_pair_smooth(SERVO_MAX_COUNT, SERVO_MIN_COUNT))
        {
            show_error_forever();
        }

        __delay_ms(SERVO_END_PAUSE_MS);
    }
}
