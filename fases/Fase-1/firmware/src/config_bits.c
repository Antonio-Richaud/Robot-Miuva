/*
 * Robot-Miuva - Fase 1
 * Bits de configuración para PIC18F45K50.
 *
 * Reloj principal: cristal externo de 12 MHz.
 * PLL: desactivado en esta primera prueba.
 * Frecuencia de CPU esperada: 12 MHz.
 *
 * Antes de la primera compilación en Windows, estos valores deben compararse
 * con Window > Target Memory Views > Configuration Bits de MPLAB X para la
 * versión instalada de XC8. El archivo debe seguir siendo la única fuente de
 * pragmas de configuración del proyecto.
 */

#include <xc.h>

// CONFIG1L
#pragma config PLLSEL = PLL4X
#pragma config CFGPLLEN = OFF
#pragma config CPUDIV = NOCLKDIV
#pragma config LS48MHZ = SYS24X4

// CONFIG1H
#pragma config FOSC = HSM
#pragma config PCLKEN = ON
#pragma config FCMEN = OFF
#pragma config IESO = OFF

// CONFIG2L
#pragma config nPWRTEN = OFF
#pragma config BOREN = SBORDIS
#pragma config BORV = 190
#pragma config nLPBOR = OFF

// CONFIG2H
#pragma config WDTEN = OFF
#pragma config WDTPS = 32768

// CONFIG3H
#pragma config CCP2MX = RC1
#pragma config PBADEN = OFF
#pragma config T3CMX = RC0
#pragma config SDOMX = RB3
#pragma config MCLRE = ON

// CONFIG4L
#pragma config STVREN = ON
#pragma config LVP = OFF
#pragma config ICPRT = OFF
#pragma config XINST = OFF

// CONFIG5L
#pragma config CP0 = OFF
#pragma config CP1 = OFF
#pragma config CP2 = OFF
#pragma config CP3 = OFF

// CONFIG5H
#pragma config CPB = OFF
#pragma config CPD = OFF

// CONFIG6L
#pragma config WRT0 = OFF
#pragma config WRT1 = OFF
#pragma config WRT2 = OFF
#pragma config WRT3 = OFF

// CONFIG6H
#pragma config WRTC = OFF
#pragma config WRTB = OFF
#pragma config WRTD = OFF

// CONFIG7L
#pragma config EBTR0 = OFF
#pragma config EBTR1 = OFF
#pragma config EBTR2 = OFF
#pragma config EBTR3 = OFF

// CONFIG7H
#pragma config EBTRB = OFF
