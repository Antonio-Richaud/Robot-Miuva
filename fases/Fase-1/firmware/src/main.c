/**
 * Robot-Miuva - Fase 1
 * Control incremental de dos SG90 con modulo analogico HW-504.
 *
 * Conexiones:
 *   VRx -> RA0 / AN0 -> servo del canal 0
 *   VRy -> RA1 / AN1 -> servo del canal 1
 *   SW  -> RB2; el modulo cierra el pin hacia GND al presionarlo
 *
 * Comportamiento:
 *   - Cada eje del joystick incrementa o decrementa la posicion almacenada.
 *   - Al volver el joystick al centro, el servo conserva su ultima posicion.
 *   - Una inclinacion pequena produce ajustes finos.
 *   - Una inclinacion grande produce movimientos mas rapidos.
 *   - El rango se limita entre 111 y 492 cuentas del PCA9685,
 *     aproximadamente 544 a 2400 us a 50 Hz nominales.
 *
 * Pulsador:
 *   - Mientras esta presionado, ambos servos regresan suavemente al centro.
 *   - Mientras esta presionado, el LED RGB se muestra blanco.
 *   - Al soltarlo, cambia una sola vez el color operativo del LED RGB.
 *   - Secuencia: verde, azul, cian, magenta, amarillo, blanco.
 *
 * Indicadores:
 *   Azul fijo durante el arranque -> calibracion; no mover el mando.
 *   Color seleccionado            -> control manual activo.
 *   Blanco mientras SW se pulsa   -> pulsador detectado y centrado activo.
 *   Rojo rapido                   -> error de inicializacion o comunicacion.
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

#define LED_COLOR_GREEN   0u
#define LED_COLOR_BLUE    1u
#define LED_COLOR_CYAN    2u
#define LED_COLOR_MAGENTA 3u
#define LED_COLOR_YELLOW  4u
#define LED_COLOR_WHITE   5u
#define LED_COLOR_COUNT   6u

#define ADC_CHANNEL_X 0u
#define ADC_CHANNEL_Y 1u
#define ADC_MAX_VALUE 1023u

#define SERVO_CHANNEL_0             0u
#define SERVO_CHANNEL_1             1u
#define SERVO_MIN_COUNT           111u
#define SERVO_CENTER_COUNT        307u
#define SERVO_MAX_COUNT           492u
#define SERVO_MAX_INCREMENT_COUNTS  8u
#define SERVO_CENTER_STEP_COUNTS    8u

#define JOYSTICK_STARTUP_SAMPLES 32u
#define JOYSTICK_LOOP_SAMPLES     4u
#define JOYSTICK_DEADZONE        40u
#define JOYSTICK_CENTER_MIN     256u
#define JOYSTICK_CENTER_MAX     768u
#define FILTER_DIVISOR            4u
#define BUTTON_DEBOUNCE_MS       25u
#define CONTROL_PERIOD_MS        20u

#define SERVO_0_INVERTED 0u
#define SERVO_1_INVERTED 0u

static void set_led_rgb(bool red_on, bool green_on, bool blue_on)
{
    LED_RED_LAT = red_on ? LED_ON : LED_OFF;
    LED_GREEN_LAT = green_on ? LED_ON : LED_OFF;
    LED_BLUE_LAT = blue_on ? LED_ON : LED_OFF;
}

static void system_initialize(void)
{
    ANSELE = 0x00;

    set_led_rgb(false, false, false);

    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;

    /* RB2 como entrada digital para SW. */
    ANSELBbits.ANSB2 = 0;
    TRISBbits.TRISB2 = 1;

    /* Pull-up interno adicional; la resistencia externa puede permanecer. */
    WPUBbits.WPUB2 = 1;
    INTCON2bits.RBPU = 0;
}

static void show_calibrating(void)
{
    set_led_rgb(false, false, true);
}

static void show_button_active(void)
{
    set_led_rgb(true, true, true);
}

static void show_operating_color(uint8_t color)
{
    switch (color)
    {
        case LED_COLOR_BLUE:
            set_led_rgb(false, false, true);
            break;

        case LED_COLOR_CYAN:
            set_led_rgb(false, true, true);
            break;

        case LED_COLOR_MAGENTA:
            set_led_rgb(true, false, true);
            break;

        case LED_COLOR_YELLOW:
            set_led_rgb(true, true, false);
            break;

        case LED_COLOR_WHITE:
            set_led_rgb(true, true, true);
            break;

        case LED_COLOR_GREEN:
        default:
            set_led_rgb(false, true, false);
            break;
    }
}

static void show_error_forever(void)
{
    set_led_rgb(false, false, false);

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

static bool read_debounced_button(bool stable_pressed)
{
    bool raw_pressed = joystick_button_pressed();

    if (raw_pressed != stable_pressed)
    {
        __delay_ms(BUTTON_DEBOUNCE_MS);
        raw_pressed = joystick_button_pressed();
    }

    return raw_pressed;
}

static uint8_t next_led_color(uint8_t color)
{
    color++;

    if (color >= LED_COLOR_COUNT)
    {
        color = LED_COLOR_GREEN;
    }

    return color;
}

static uint16_t filter_adc(uint16_t previous, uint16_t sample)
{
    uint32_t weighted;

    weighted = ((uint32_t)previous * (FILTER_DIVISOR - 1u)) + sample;
    return (uint16_t)(weighted / FILTER_DIVISOR);
}

static int16_t axis_to_increment(uint16_t value, uint16_t center, uint8_t inverted)
{
    uint16_t magnitude;
    uint16_t usable_span;
    uint16_t step;
    uint32_t scaled;
    int16_t increment = 0;

    if ((uint16_t)(value + JOYSTICK_DEADZONE) < center)
    {
        usable_span = (center > JOYSTICK_DEADZONE)
            ? (uint16_t)(center - JOYSTICK_DEADZONE)
            : 1u;
        magnitude = (uint16_t)(center - JOYSTICK_DEADZONE - value);

        scaled = ((uint32_t)magnitude * SERVO_MAX_INCREMENT_COUNTS)
            + usable_span - 1u;
        step = (uint16_t)(scaled / usable_span);

        if (step == 0u)
        {
            step = 1u;
        }
        else if (step > SERVO_MAX_INCREMENT_COUNTS)
        {
            step = SERVO_MAX_INCREMENT_COUNTS;
        }

        increment = -(int16_t)step;
    }
    else if (value > (uint16_t)(center + JOYSTICK_DEADZONE))
    {
        usable_span = ((uint16_t)(center + JOYSTICK_DEADZONE) < ADC_MAX_VALUE)
            ? (uint16_t)(ADC_MAX_VALUE - center - JOYSTICK_DEADZONE)
            : 1u;
        magnitude = (uint16_t)(value - center - JOYSTICK_DEADZONE);

        scaled = ((uint32_t)magnitude * SERVO_MAX_INCREMENT_COUNTS)
            + usable_span - 1u;
        step = (uint16_t)(scaled / usable_span);

        if (step == 0u)
        {
            step = 1u;
        }
        else if (step > SERVO_MAX_INCREMENT_COUNTS)
        {
            step = SERVO_MAX_INCREMENT_COUNTS;
        }

        increment = (int16_t)step;
    }

    if (inverted != 0u)
    {
        increment = -increment;
    }

    return increment;
}

static uint16_t apply_increment(uint16_t position, int16_t increment)
{
    uint16_t step;

    if (increment > 0)
    {
        step = (uint16_t)increment;

        if (step >= (uint16_t)(SERVO_MAX_COUNT - position))
        {
            return SERVO_MAX_COUNT;
        }

        return (uint16_t)(position + step);
    }

    if (increment < 0)
    {
        step = (uint16_t)(-increment);

        if (step >= (uint16_t)(position - SERVO_MIN_COUNT))
        {
            return SERVO_MIN_COUNT;
        }

        return (uint16_t)(position - step);
    }

    return position;
}

static uint16_t approach_center(uint16_t position)
{
    uint16_t distance;

    if (position < SERVO_CENTER_COUNT)
    {
        distance = (uint16_t)(SERVO_CENTER_COUNT - position);

        if (distance > SERVO_CENTER_STEP_COUNTS)
        {
            return (uint16_t)(position + SERVO_CENTER_STEP_COUNTS);
        }

        return SERVO_CENTER_COUNT;
    }

    if (position > SERVO_CENTER_COUNT)
    {
        distance = (uint16_t)(position - SERVO_CENTER_COUNT);

        if (distance > SERVO_CENTER_STEP_COUNTS)
        {
            return (uint16_t)(position - SERVO_CENTER_STEP_COUNTS);
        }

        return SERVO_CENTER_COUNT;
    }

    return position;
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
    uint16_t position_0 = SERVO_CENTER_COUNT;
    uint16_t position_1 = SERVO_CENTER_COUNT;
    int16_t increment_0;
    int16_t increment_1;
    uint8_t selected_led_color = LED_COLOR_GREEN;
    bool button_pressed = false;
    bool button_was_pressed = false;

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
    show_operating_color(selected_led_color);

    while (1)
    {
        raw_x = adc_read_average(ADC_CHANNEL_X, JOYSTICK_LOOP_SAMPLES);
        raw_y = adc_read_average(ADC_CHANNEL_Y, JOYSTICK_LOOP_SAMPLES);

        filtered_x = filter_adc(filtered_x, raw_x);
        filtered_y = filter_adc(filtered_y, raw_y);

        button_pressed = read_debounced_button(button_pressed);

        if (button_pressed)
        {
            button_was_pressed = true;
            position_0 = approach_center(position_0);
            position_1 = approach_center(position_1);
            show_button_active();
        }
        else
        {
            if (button_was_pressed)
            {
                button_was_pressed = false;
                selected_led_color = next_led_color(selected_led_color);
            }

            increment_0 = axis_to_increment(
                filtered_x,
                center_x,
                SERVO_0_INVERTED);
            increment_1 = axis_to_increment(
                filtered_y,
                center_y,
                SERVO_1_INVERTED);

            position_0 = apply_increment(position_0, increment_0);
            position_1 = apply_increment(position_1, increment_1);

            show_operating_color(selected_led_color);
        }

        if (!set_servo_positions(position_0, position_1))
        {
            show_error_forever();
        }

        __delay_ms(CONTROL_PERIOD_MS);
    }
}
