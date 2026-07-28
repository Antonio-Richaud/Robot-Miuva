# Compilación y carga en Windows

## Software necesario

- MPLAB X IDE.
- MPLAB XC8 para microcontroladores de 8 bits.
- Aplicación compatible con el programador integrado de la Miuva RevJ.

## Crear el proyecto

1. Abrir MPLAB X.
2. Ir a `File > New Project`.
3. Seleccionar `Microchip Embedded > Standalone Project`.
4. Elegir el dispositivo `PIC18F45K50`.
5. Seleccionar `Simulator` como herramienta inicial.
6. Seleccionar la instalación disponible de MPLAB XC8.
7. Nombrar el proyecto `RobotMiuvaFase1`.
8. Guardarlo en:

   ```text
   fases/Fase-1/firmware/mplab
   ```

9. Activar `Set as Main Project`.
10. No abrir MCC durante esta primera prueba.

## Agregar el código existente

Agregar como archivos existentes, sin duplicarlos:

```text
fases/Fase-1/firmware/src/main.c
fases/Fase-1/firmware/src/config_bits.c
```

En MPLAB X pueden añadirse con clic derecho sobre `Source Files` y después `Add Existing Item`.

## Validar los bits de configuración

Antes de compilar por primera vez:

1. Abrir `Window > Target Memory Views > Configuration Bits`.
2. Confirmar que el dispositivo activo es PIC18F45K50.
3. Comparar los valores mostrados con `src/config_bits.c`.
4. Confirmar especialmente:
   - oscilador primario externo de alta velocidad;
   - PLL desactivado;
   - divisor de CPU desactivado;
   - watchdog desactivado;
   - programación de bajo voltaje desactivada;
   - MCLR habilitado.

No deben existir pragmas de configuración duplicados en otro archivo.

## Compilar

Usar `Clean and Build Main Project`.

La compilación debe terminar con:

```text
BUILD SUCCESSFUL
```

MPLAB X generará el `.hex` dentro del directorio `dist` del proyecto `.X`. La ruta exacta depende de la configuración activa, normalmente `production`.

## Primera prueba de carga

Realizarla únicamente con la tarjeta Miuva:

- sin PCA9685;
- sin joystick;
- sin servomotores;
- sin fuente externa para servos.

Después de grabar el `.hex`, el LED verde debe parpadear aproximadamente cada 500 ms.

## Diagnóstico rápido

- No compila: revisar versión de XC8 y nombres de los pragmas de configuración.
- Compila pero no programa: revisar controlador y herramienta del programador de la Miuva.
- Programa pero no parpadea: revisar orientación del firmware, selección del dispositivo y bits de reloj.
- Parpadea demasiado rápido o lento: detener el avance y corregir la frecuencia del reloj.
