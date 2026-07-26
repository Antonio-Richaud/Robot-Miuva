# Robot-Miuva

Control de un brazo robótico de hasta seis servomotores mediante una tarjeta de desarrollo **Miuva RevJ de Intesc**, basada en el **PIC18F45K50**, y un controlador PWM **PCA9685**.

El proyecto se desarrollará en dos fases. Primero se validará el hardware y el control manual de servomotores mediante un joystick analógico. Después se implementará un motor de movimiento capaz de ejecutar posturas y rutinas coordinadas para un robot de seis grados de actuación.

## Objetivos

- Comunicar el PIC18F45K50 con el PCA9685 mediante I²C por hardware.
- Controlar inicialmente dos servomotores con un joystick de dos ejes.
- Calibrar límites eléctricos y mecánicos por articulación.
- Implementar movimientos graduales, filtrados y seguros.
- Escalar el sistema a seis servomotores.
- Programar posturas, interpolaciones y secuencias reutilizables.
- Mantener una base de firmware portable entre macOS y Windows.

## Arquitectura general

```text
Joystick analógico
      │
      ├── Eje X ──> ADC PIC18F45K50
      └── Eje Y ──> ADC PIC18F45K50
                         │
                         │ I²C
                         ▼
                    PCA9685
                         │
                         ├── Canal 0 ──> Servo 1
                         ├── Canal 1 ──> Servo 2
                         ├── Canal 2 ──> Servo 3
                         ├── Canal 3 ──> Servo 4
                         ├── Canal 4 ──> Servo 5
                         └── Canal 5 ──> Servo 6
```

El PIC no genera directamente los seis PWM. Lee entradas, aplica lógica de control y envía posiciones al PCA9685, que produce las señales PWM de los servomotores.

## Fases

### [Fase 1 — Control manual con joystick](fases/Fase-1/README.md)

Validación de alimentación, comunicación I²C, movimiento seguro de uno y después dos servomotores, lectura del joystick, zona muerta, filtrado y límites.

### [Fase 2 — Rutinas del robot](fases/Fase-2/README.md)

Control de seis servomotores mediante posturas, keyframes, interpolación temporal, rutinas y mecanismos de parada segura.

## Estructura inicial

```text
Robot-Miuva/
├── .gitignore
├── README.md
├── CONTRIBUTING.md
├── docs/
│   ├── HARDWARE-Y-SEGURIDAD.md
│   └── PINOUT.md
└── fases/
    ├── Fase-1/
    │   └── README.md
    └── Fase-2/
        └── README.md
```

Los proyectos de MPLAB X se añadirán dentro de cada fase cuando comience la implementación. Los archivos públicos de configuración del proyecto deberán versionarse; las preferencias privadas y salidas de compilación quedan excluidas mediante `.gitignore`.

## Herramientas previstas

- Miuva RevJ de Intesc.
- PIC18F45K50.
- PCA9685 de 16 canales y 12 bits.
- MPLAB X IDE.
- Compilador MPLAB XC8.
- Programador integrado de la Miuva o programador ICSP compatible.
- Fuente independiente para servomotores.
- Joystick analógico de dos ejes.

## Reglas eléctricas esenciales

1. La Miuva y el PCA9685 pueden compartir la alimentación lógica adecuada.
2. Los servomotores deben recibir potencia desde una fuente independiente dimensionada para su corriente.
3. La tierra de la Miuva, la del PCA9685 y la de la fuente de servos deben estar unidas.
4. Nunca se deben alimentar varios servos desde el puerto USB o desde el regulador de la Miuva.
5. Antes de conectar un servo al mecanismo, se debe centrar y calibrar sin carga.
6. Los límites de pulso deben empezar de forma conservadora y ampliarse únicamente después de comprobar el recorrido real.

Consulta [Hardware y seguridad](docs/HARDWARE-Y-SEGURIDAD.md) antes de energizar el sistema.

## Estado

Proyecto en etapa de diseño e inicialización. El siguiente paso será implementar la Fase 1 de manera incremental: prueba I²C, un servo, calibración, segundo servo y joystick.
