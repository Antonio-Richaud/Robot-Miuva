#ifndef _XTAL_FREQ
#define _XTAL_FREQ 12000000UL
#endif

#include <xc.h>
#include <stdint.h>

#include "adc.h"

#define ADC_FIRST_CHANNEL 0u
#define ADC_LAST_CHANNEL  1u

void adc_initialize(void)
{
    /* RA0/AN0 y RA1/AN1 como entradas analogicas. */
    ANSELAbits.ANSA0 = 1;
    ANSELAbits.ANSA1 = 1;
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;

    /* Referencias VDD/VSS. */
    ADCON1 = 0x00;

    /* Resultado a la derecha, adquisicion de 8 TAD y reloj Fosc/16. */
    ADCON2 = 0xA5;

    /* Canal AN0 seleccionado y convertidor habilitado. */
    ADCON0 = 0x01;
}

uint16_t adc_read(uint8_t channel)
{
    if ((channel < ADC_FIRST_CHANNEL) || (channel > ADC_LAST_CHANNEL))
    {
        return 0u;
    }

    ADCON0bits.CHS = channel;
    __delay_us(10);

    ADCON0bits.GO_NOT_DONE = 1;
    while (ADCON0bits.GO_NOT_DONE)
    {
        ;
    }

    return (uint16_t)(((uint16_t)ADRESH << 8) | ADRESL);
}

uint16_t adc_read_average(uint8_t channel, uint8_t samples)
{
    uint8_t index;
    uint32_t accumulator = 0u;

    if (samples == 0u)
    {
        return adc_read(channel);
    }

    for (index = 0u; index < samples; index++)
    {
        accumulator += adc_read(channel);
    }

    return (uint16_t)(accumulator / samples);
}
