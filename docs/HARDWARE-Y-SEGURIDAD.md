# Hardware, alimentación y seguridad

## Alcance

Este documento define las conexiones eléctricas y las reglas mínimas para trabajar con la Miuva RevJ, el PIC18F45K50, el PCA9685 y hasta seis servomotores.

## Separación entre lógica y potencia

El PCA9685 maneja dos dominios distintos:

- `VCC`: alimentación de la lógica interna y de la interfaz I²C.
- `V+`: alimentación de potencia destinada a los servomotores en los módulos que distribuyen esta línea a los conectores de salida.

No se deben confundir. La fuente de potencia de los servos no debe conectarse al pin lógico `VCC`.

## Alimentación recomendada

### Miuva

La Miuva se alimentará por su método normal de trabajo, por ejemplo USB, respetando las especificaciones de la tarjeta.

### PCA9685, lado lógico

- Conectar `VCC` del PCA9685 al nivel lógico compatible con la Miuva.
- Conectar `GND` del PCA9685 a `GND` de la Miuva.
- Verificar con multímetro la tensión real antes de conectar el PIC.

### Servomotores

- Usar una fuente independiente y regulada.
- Ajustar el voltaje al rango admitido por el modelo exacto de servo.
- No asumir que todos los servos admiten 6 V.
- Dimensionar la corriente para arranque, bloqueo y movimientos simultáneos.
- Incorporar protección contra inversión de polaridad y, de ser posible, fusible.

Para servos comunes de 5 V o 6 V, el valor final debe decidirse con la hoja de datos del modelo utilizado. Los picos de corriente pueden ser varias veces mayores que el consumo sin carga.

## Tierra común obligatoria

```text
GND Miuva ───── GND PCA9685 ───── GND fuente de servos
```

Sin tierra común, las señales PWM no tendrán una referencia válida y el comportamiento puede ser errático.

## Desacoplo y picos de corriente

Aunque el sistema ya disponga de acondicionamiento de fuente, deben conservarse estas reglas:

- Colocar capacitancia de reserva cerca del bloque de alimentación de servos.
- Mantener cortos y gruesos los conductores de potencia.
- Evitar que la corriente de los servos circule por pistas o cables destinados a la lógica.
- Separar físicamente, cuando sea posible, el cableado de potencia y el de señal.
- Comprobar caída de voltaje durante movimientos simultáneos.

La capacitancia no sustituye una fuente insuficiente. Sirve para amortiguar transitorios, no para crear corriente que la fuente no puede entregar.

## Pin OE como parada de hardware

Se recomienda conectar `OE` del PCA9685 a un GPIO del PIC.

- `OE = 0`: salidas habilitadas.
- `OE = 1`: salidas deshabilitadas.

Añadir una resistencia de pull-up permite que las salidas permanezcan deshabilitadas durante el arranque o reinicio del PIC. El firmware deberá habilitarlas únicamente después de completar la inicialización y cargar posiciones seguras.

## Secuencia segura de encendido

1. Desconectar mecánicamente los servos o retirar carga peligrosa.
2. Verificar polaridad y voltajes con multímetro.
3. Encender primero la lógica.
4. Inicializar I²C y el PCA9685.
5. Cargar pulsos centrales conservadores.
6. Habilitar `OE`.
7. Encender o conectar la potencia de servos.
8. Probar un solo canal.
9. Avanzar al resto de canales de forma gradual.

## Secuencia segura de apagado

1. Llevar el robot a una postura segura cuando sea posible.
2. Deshabilitar las salidas mediante `OE`.
3. Desconectar la potencia de servos.
4. Desconectar la lógica.

## Calibración inicial de servos

No se debe iniciar con el rango máximo teórico. Para una señal cercana a 50 Hz:

- Centro inicial: aproximadamente `1500 µs`.
- Mínimo conservador: aproximadamente `1000 µs`.
- Máximo conservador: aproximadamente `2000 µs`.

Cada servo y articulación deberá almacenar:

- Pulso mínimo seguro.
- Pulso central.
- Pulso máximo seguro.
- Sentido normal o invertido.
- Ángulo mecánico mínimo y máximo.
- Velocidad máxima permitida.

Si el servo zumba, se calienta, golpea un tope o aumenta notablemente su consumo, se debe detener la prueba y reducir el rango.

## Riesgos que deben evitarse

- Alimentar servos desde el USB de la computadora.
- Invertir `VCC` y `V+`.
- Omitir la tierra común.
- Conectar o desconectar servos bajo carga sin una secuencia controlada.
- Enviar posiciones extremas antes de calibrar.
- Usar retardos largos que impidan atender una parada.
- Probar los seis servos simultáneamente antes de validar uno por uno.
- Mover el robot cerca de personas, cables, herramientas o superficies donde pueda atorarse.

## Lista de verificación previa

- [ ] Voltaje lógico medido.
- [ ] Voltaje de servos medido.
- [ ] Polaridad comprobada.
- [ ] Tierra común conectada.
- [ ] Fuente dimensionada para la corriente esperada.
- [ ] `OE` en estado seguro.
- [ ] Servo sin carga mecánica durante la primera prueba.
- [ ] Límites conservadores cargados en firmware.
- [ ] Medio de desconexión rápida accesible.
