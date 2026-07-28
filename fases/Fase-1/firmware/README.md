# Firmware de la Fase 1

Esta carpeta contiene el firmware inicial de **Robot-Miuva** para la tarjeta Miuva RevJ con PIC18F45K50.

La primera prueba hace parpadear el canal verde del LED RGB integrado. Todavía no utiliza el PCA9685, el joystick ni los servomotores.

## Propósito de esta prueba

Validar, en el orden correcto:

1. que MPLAB X reconoce el PIC18F45K50;
2. que MPLAB XC8 compila el código;
3. que se genera un archivo `.hex` válido;
4. que la Miuva puede programarse desde Windows;
5. que el cristal externo de 12 MHz funciona con los retardos esperados;
6. que el LED integrado responde antes de conectar hardware externo.

## Configuración inicial

- Microcontrolador: PIC18F45K50.
- Tarjeta: Miuva RevJ.
- Oscilador: cristal externo de 12 MHz.
- PLL: desactivado durante la prueba inicial.
- Frecuencia de CPU: 12 MHz.
- LED verde: RE0.
- LED rojo: RE1.
- LED azul: RE2.
- Lógica del LED: activa en nivel bajo.

## Estructura

```text
firmware/
├── README.md
├── src/
│   ├── config_bits.c
│   └── main.c
├── docs/
│   └── BUILD-WINDOWS.md
└── mplab/
    └── README.md
```

## Restricciones de la primera prueba

La primera grabación debe realizarse únicamente con la Miuva conectada por USB:

- sin PCA9685;
- sin joystick;
- sin servomotores;
- sin fuente externa de potencia;
- sin mecanismos conectados.

## Resultado esperado

El LED verde debe encenderse durante aproximadamente 500 ms y apagarse durante aproximadamente 500 ms, repitiendo el ciclo indefinidamente.

Si el tiempo es claramente distinto, se debe revisar la configuración del reloj antes de continuar.

## Archivos binarios

Los `.hex` y demás salidas de compilación no se versionan como código fuente. Cuando tengamos una compilación validada podremos publicarla como artefacto o adjuntarla a una versión del repositorio.
