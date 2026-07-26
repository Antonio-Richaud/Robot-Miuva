# Pinout inicial

> Este pinout es la propuesta de trabajo inicial. Antes de cablear se debe contrastar con el esquemático y la serigrafía de la revisión exacta de la Miuva utilizada.

## Bus I²C entre Miuva y PCA9685

| Señal | PIC18F45K50 / Miuva | PCA9685 | Notas |
|---|---|---|---|
| SDA | RB0 / SDA | SDA | Línea de datos I²C. |
| SCL | RB1 / SCL | SCL | Línea de reloj I²C. |
| Lógica | Nivel lógico de la Miuva | VCC | No conectar aquí la fuente de servos. |
| Tierra | GND | GND | Debe compartir referencia con la fuente de servos. |
| Habilitación | GPIO por definir | OE | Activo en bajo; se recomienda pull-up. |

## Dirección I²C

Con los pines de dirección `A0` a `A5` en nivel bajo, la dirección de 7 bits esperada del PCA9685 es:

```c
#define PCA9685_I2C_ADDRESS 0x40U
```

Algunas rutinas de bajo nivel solicitan la dirección de 7 bits y otras el byte ya desplazado. El driver del proyecto expondrá únicamente la dirección de 7 bits para evitar ambigüedad.

## Joystick analógico

Propuesta inicial para la Fase 1:

| Señal del joystick | PIC18F45K50 / Miuva | Función |
|---|---|---|
| VRx | RA0 / AN0 | Lectura ADC del eje X. |
| VRy | RA1 / AN1 | Lectura ADC del eje Y. |
| SW | GPIO por definir | Botón activo normalmente en bajo. |
| VCC | Alimentación compatible | Confirmar voltaje del módulo. |
| GND | GND común | Referencia del ADC. |

El firmware medirá los valores mínimo, central y máximo reales del joystick. No se asumirán automáticamente `0`, `512` y `1023`.

## Canales del PCA9685

Asignación inicial prevista:

| Canal | Fase 1 | Fase 2 propuesta |
|---:|---|---|
| 0 | Servo del eje X | Base |
| 1 | Servo del eje Y | Hombro |
| 2 | Sin uso | Codo |
| 3 | Sin uso | Muñeca 1 |
| 4 | Sin uso | Muñeca 2 |
| 5 | Sin uso | Garra |

La asignación de la Fase 2 podrá modificarse cuando se documenten los modelos de servo y la geometría final del robot.

## Conexión de potencia

```text
Miuva                         PCA9685                       Fuente de servos
─────                         ───────                       ─────────────────
Nivel lógico  ──────────────> VCC
GND            ─────────────> GND <─────────────────────── GND
RB0 / SDA      ─────────────> SDA
RB1 / SCL      ─────────────> SCL
GPIO seguro    ─────────────> OE
                              V+  <──────────────────────── +V servos
```

Los conectores de servo deberán respetar el orden real de pines del módulo PCA9685 utilizado. No todos los módulos presentan físicamente señal, positivo y tierra en el mismo orden visual.

## Pull-ups del bus I²C

Muchos módulos PCA9685 ya incluyen resistencias pull-up en SDA y SCL. Antes de añadir otras:

1. Inspeccionar el módulo.
2. Consultar su esquema si está disponible.
3. Medir la resistencia efectiva hacia `VCC` con el sistema apagado.
4. Evitar una resistencia equivalente demasiado baja por pull-ups en paralelo.

## Parámetros iniciales

| Parámetro | Valor inicial |
|---|---:|
| Frecuencia I²C | 100 kHz |
| Frecuencia PWM de servos | Aproximadamente 50 Hz |
| Dirección PCA9685 | `0x40` |
| Pulso central inicial | 1500 µs |
| Rango conservador inicial | 1000–2000 µs |
| Canales activos en primera prueba | Solo canal 0 |

## Puntos pendientes antes del cableado definitivo

- [ ] Confirmar revisión exacta de la Miuva.
- [ ] Confirmar ubicación física de RB0 y RB1 en sus headers.
- [ ] Elegir GPIO para `OE`.
- [ ] Elegir GPIO para el botón `SW` del joystick.
- [ ] Confirmar voltaje lógico y voltaje del joystick.
- [ ] Registrar modelo y rango de voltaje de cada servo.
- [ ] Documentar orden de pines de los conectores del módulo PCA9685 concreto.
