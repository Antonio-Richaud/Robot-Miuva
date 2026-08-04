/**
 * Robot-Miuva - Fase 1
 * Control manual de dos SG90 con modulo analogico HW-504.
 *
 * Conexiones:
 *   VRx -> RA0 / AN0 -> servo del canal 0
 *   VRy -> RA1 / AN1 -> servo del canal 1
 *   SW  -> RB2 con resistencia pull-up externa de 10 kohm
 *
 * El sistema calibra el centro durante el arranque, aplica zona muerta,
 * promedio, filtrado y limite de velocidad. Esta prueba utiliza el rango
 * estandar inicial de 205 a 409 cuentas del PCA9685, aproximadamente
 * 1000 a 2000 us con una frecuencia PWM nominal de 50 Hz.
 *
 * Indicadores:
 *   Azul fijo  -> calibracion inicial; no mover el mando.
 *   Verde fijo -> control manual activo.
 *   Azul fijo  -> boton presionado; ambos servos vuelven al centro.
 *   Rojo rapido -> error de inicializacion o comunicacion.
 */

#define _XTAL_FREQ 12000000UL

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#include "drivers/adc.h"
#include "drivers/i2c_master.h"
#include "drivers/pca9685.h"

#define LED_GREEN_LAT LATEbits.LATE0
#define LED_RED_LAT   LATEbits.LATE1
#define LED_BLUE_LAT  LATEbits.LATE2

#define LED_ON  0u
#define LED_OFF 1u

#define ADC_CHANNEL_X 0u
#define ADC_CHANNEL_Y 1u
#define ADC_MAX_VALUE 1023u

#define SERVO_CHANNEL_0       0u
#define SERVO_CHANNEL_1       1u
#define SERVO_MIN_COUNT     205u
#define SERVO_CENTER_COUNT  307u
#define SERVO_MAX_COUNT     409u
#define SERVO_RANGE_COUNTS  102u

#define JOYSTICK_STARTUP_SAMPLES 32u
#define JOYSTICK_LOOP_SAMPLES     4u
#define JOYSTICK_DEADZONE        40u
#define JOYSTICK_CENTER_MIN     256u
#define JOYSTICK_CENTER_MAX     768u
#define FILTER_DIVISOR            4u
#define CONTROL_PERIOD_MS        20u

#define SERVO_0_INVERTED 0u
#define SERVO_1_INVERTED 0u

static void system_initialize(void)
{
    ANSELE = 0x00;

    LED_GREEN_LAT = LED_OFF;
    LED_RED_LAT = LED_OFF;
    LED_BLUE_LAT = LED_OFF;

    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;

    /* RB2 recibe SW. El modulo cierra el pin hacia GND al presionarlo. */
    ANSELBbits.ANSB2 = 0;
    TRISBbits.TRISB2 = 1;
}

static void show_calibrating(void)
{
    LED_GREEN_LAT = LED_OFF;
    LED_RED_LAT = LED_OFF;
    LED_BLUE_LAT = LED_ON;
}

static void show_normal(void)
{
    LED_RED_LAT = LED_OFF;
    LED_BLUE_LAT = LED_OFF;
    LED_GREEN_LAT = LED_ON;
}

static void show_button_pressed(void)
{
    LED_GREEN_LAT = LED_OFF;
    LED_RED_LAT = LED_OFF;
    LED_BLUE_LAT = LED_ON;
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

static bool joystick_button_pressed(void)
{
    return (PORTBbits.RB2 == 0u);
}

static uint16_t filter_adc(uint16_t previous, uint16_t sample)
{
    uint32_t weighted;

    weighted = ((uint32_t)previous * (FILTER_DIVISOR - 1u)) + sample;
    return (uint16_t)(weighted / FILTER_DIVISOR);
}

static uint16_t map_axis_to_pulse(uint16_t value, uint16_t center)
{
    uint16_t magnitude;
    uint16_t usable_span;
    uint16_t offset;
    uint32_t scaled;

    if ((uint16_t)(value + JOYSTICK_DEADZONE) < center)
    {
        usable_span = (center > JOYSTICK_DEADZONE)
            ? (uint16_t)(center - JOYSTICK_DEADZONE)
            : 1u;
        magnitude = (uint16_t)(center - JOYSTICK_DEADZONE - value);
        scaled = ((uint32_t)magnitude * SERVO_RANGE_COUNTS) + (usable_span / 2u);
        offset = (uint16_t)(scaled / usable_span);

        if (offset > SERVO_RANGE_COUNTS)
        {
            offset = SERVO_RANGE_COUNTS;
        }

        return (uint16_t)(SERVO_CENTER_COUNT - offset);
    }

    if (value > (uint16_t)(center + JOYSTICK_DEADZONE))
    {
        usable_span = ((uint16_t)(center + JOYSTICK_DEADZONE) < ADC_MAX_VALUE)
            ? (uint16_t)(ADC_MAX_VALUE - center - JOYSTICK_DEADZONE)
            : 1u;
        magnitude = (uint16_t)(value - center - JOYSTICK_DEADZONE);
        scaled = ((uint32_t)magnitude * SERVO_RANGE_COUNTS) + (usable_span / 2u);
        offset = (uint16_t)(scaled / usable_span);

        if (offset > SERVO_RANGE_COUNTS)
        {
            offset = SERVO_RANGE_COUNTS;
        }

        return (uint16_t)(SERVO_CENTER_COUNT + offset);
    }

    return SERVO_CENTER_COUNT;
}

static uint16_t apply_direction(uint16_t pulse, uint8_t inverted)
{
    if (inverted != 0u)
    {
        return (uint16_t)(SERVO_MIN_COUNT + SERVO_MAX_COUNT - pulse);
    }

    return pulse;
}

static uint16_t approach_target(uint16_t current, uint16_t target)
{
    if (current < target)
    {
        return (uint16_t)(current + 1u);
    }

    if (current > target)
    {
        return (uint16_t)(current - 1u);
    }

    return current;
}

static bool set_servo_positions(uint16_t pulse_0, uint16_t pulse_1)
{
    if (!pca9685_set_channel_pulse(SERVO_CHANNEL_0, pulse_0))
    {
        return false;
    }

    __delay_ms(2);

    return pca9685_set_channel_pulse(SERVO_CHANNEL_1, pulse_1);
}

int main(void)
{
    uint16_t center_x;
    uint16_t center_y;
    uint16_t raw_x;
    uint16_t raw_y;
    uint16_t filtered_x;
    uint16_t filtered_y;
    uint16_t target_0;
    uint16_t target_1;
    uint16_t current_0 = SERVO_CENTER_COUNT;
    uint16_t current_1 = SERVO_CENTER_COUNT;

    system_initialize();
    show_calibrating();

    adc_initialize();
    i2c_master_initialize();

    if (!pca9685_initialize_50hz())
    {
        show_error_forever();
    }

    if (!set_servo_positions(SERVO_CENTER_COUNT, SERVO_CENTER_COUNT))
    {
        show_error_forever();
    }

    /* Dar tiempo para soltar y centrar el mando antes de medirlo. */
    __delay_ms(1500);

    center_x = adc_read_average(ADC_CHANNEL_X, JOYSTICK_STARTUP_SAMPLES);
    center_y = adc_read_average(ADC_CHANNEL_Y, JOYSTICK_STARTUP_SAMPLES);

    if ((center_x < JOYSTICK_CENTER_MIN) || (center_x > JOYSTICK_CENTER_MAX) ||
        (center_y < JOYSTICK_CENTER_MIN) || (center_y > JOYSTICK_CENTER_MAX))
    {
        show_error_forever();
    }

    filtered_x = center_x;
    filtered_y = center_y;
    show_normal();

    while (1)
    {
        raw_x = adc_read_average(ADC_CHANNEL_X, JOYSTICK_LOOP_SAMPLES);
        raw_y = adc_read_average(ADC_CHANNEL_Y, JOYSTICK_LOOP_SAMPLES);

        filtered_x = filter_adc(filtered_x, raw_x);
        filtered_y = filter_adc(filtered_y, raw_y);

        if (joystick_button_pressed())
        {
            target_0 = SERVO_CENTER_COUNT;
            target_1 = SERVO_CENTER_COUNT;
            show_button_pressed();
        }
        else
        {
            target_0 = apply_direction(
                map_axis_to_pulse(filtered_x, center_x),
                SERVO_0_INVERTED);
            target_1 = apply_direction(
                map_axis_to_pulse(filtered_y, center_y),
                SERVO_1_INVERTED);
            show_normal();
        }

        current_0 = approach_target(current_0, target_0);
        current_1 = approach_target(current_1, target_1);

        if (!set_servo_positions(current_0, current_1))
        {
            show_error_forever();
        }

        __delay_ms(CONTROL_PERIOD_MS);
    }
}
