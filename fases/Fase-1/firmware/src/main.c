/**
 * Robot-Miuva - Fase 1
 * Prueba mínima de la Miuva RevJ con PIC18F45K50.
 *
 * Hace parpadear el canal verde del LED RGB integrado cada 500 ms.
 * Esta prueba valida el reloj externo de 12 MHz, la compilación con XC8,
 * la generación del HEX y la programación de la tarjeta.
 */

#define _XTAL_FREQ 12000000UL

#include <xc.h>
#include <stdint.h>

#define LED_GREEN_TRIS TRISEbits.TRISE0
#define LED_GREEN_LAT  LATEbits.LATE0

#define LED_ON  0u
#define LED_OFF 1u

static void system_initialize(void)
{
    /* RE0, RE1 y RE2 se utilizarán como salidas digitales. */
    ANSELE = 0x00;

    /* El LED RGB integrado es activo en nivel bajo. */
    LATEbits.LATE0 = LED_OFF;
    LATEbits.LATE1 = LED_OFF;
    LATEbits.LATE2 = LED_OFF;

    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;
    TRISEbits.TRISE2 = 0;
}

int main(void)
{
    system_initialize();

    while (1)
    {
        LED_GREEN_LAT = LED_ON;
        __delay_ms(500);

        LED_GREEN_LAT = LED_OFF;
        __delay_ms(500);
    }
}
