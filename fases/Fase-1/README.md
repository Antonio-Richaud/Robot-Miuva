# Fase 1 — Control manual con joystick

## Objetivo

Construir y validar el control manual de dos servomotores mediante un joystick analógico de dos ejes, utilizando la Miuva RevJ con PIC18F45K50 y el PCA9685.

La fase se considera terminada cuando ambos servos respondan de forma estable, gradual y limitada a movimientos seguros, sin reinicios de la lógica ni comportamiento errático por ruido o caídas de voltaje.

## Alcance

- Configuración del PIC18F45K50 con MPLAB X y XC8.
- Inicialización del bus I²C por hardware.
- Detección y configuración del PCA9685.
- Generación de PWM para servos a aproximadamente 50 Hz.
- Movimiento inicial de un solo servo.
- Calibración de límites de pulso.
- Lectura ADC de los ejes X y Y del joystick.
- Control de dos servos.
- Zona muerta y filtrado del joystick.
- Movimiento gradual y límites por canal.
- Parada segura mediante `OE`.

## Fuera de alcance

- Rutinas coordinadas de seis servos.
- Cinemática directa o inversa.
- Control desde computadora, red o aplicación móvil.
- Memorización de trayectorias.
- Control de fuerza o retroalimentación de posición externa.

## Etapas de implementación

### 1. Proyecto mínimo de la Miuva

- Crear el proyecto MPLAB X para PIC18F45K50.
- Definir bits de configuración y frecuencia de reloj.
- Verificar compilación y programación de un firmware mínimo.
- Probar una señal de diagnóstico, por ejemplo LED o salida serial si está disponible.

### 2. Driver I²C

- Inicializar MSSP en modo maestro a 100 kHz.
- Implementar `start`, `restart`, `write`, `read`, `stop` y manejo de errores.
- Añadir timeout para evitar bloqueos permanentes.
- Comprobar respuesta de la dirección `0x40`.

### 3. Driver PCA9685

- Reiniciar el dispositivo a un estado conocido.
- Configurar auto-incremento.
- Configurar frecuencia cercana a 50 Hz.
- Mantener salidas deshabilitadas durante la inicialización.
- Implementar escritura de un canal y conversión de microsegundos a cuentas.

API prevista:

```c
bool pca9685_init(void);
bool pca9685_set_pwm(uint8_t channel, uint16_t on_count, uint16_t off_count);
bool pca9685_set_pulse_us(uint8_t channel, uint16_t pulse_us);
void pca9685_disable_outputs(void);
void pca9685_enable_outputs(void);
```

### 4. Prueba de un servo

- Conectar únicamente el canal 0.
- Cargar `1500 µs` con el servo sin acoplar al mecanismo.
- Probar pequeños cambios alrededor del centro.
- Determinar el rango seguro real.
- Confirmar ausencia de vibración, calentamiento y caídas de voltaje.

### 5. Lectura del joystick

- Leer AN0 y AN1.
- Medir el centro real durante el arranque.
- Definir una zona muerta.
- Aplicar promedio móvil o filtro exponencial.
- Normalizar cada eje a un rango firmado.

### 6. Control de dos servos

Se recomienda comenzar con control por posición:

```text
Eje X del joystick ──> posición del servo 0
Eje Y del joystick ──> posición del servo 1
```

Después se evaluará control por velocidad, donde el joystick modifica gradualmente la posición y el servo conserva su último ángulo al regresar el joystick al centro.

### 7. Seguridad y estabilidad

- Limitar posición mínima y máxima por servo.
- Limitar la variación máxima por ciclo.
- Atender continuamente el botón de parada.
- Deshabilitar `OE` si falla I²C o se detecta una condición inválida.
- Evitar retardos bloqueantes largos.

## Estructura de firmware prevista

```text
fases/Fase-1/
├── README.md
└── firmware/
    ├── README.md
    ├── main.c
    ├── config/
    │   ├── clock.c
    │   ├── clock.h
    │   └── config_bits.c
    ├── drivers/
    │   ├── adc.c
    │   ├── adc.h
    │   ├── i2c_master.c
    │   ├── i2c_master.h
    │   ├── pca9685.c
    │   └── pca9685.h
    └── control/
        ├── joystick.c
        ├── joystick.h
        ├── servo_control.c
        └── servo_control.h
```

La estructura podrá ajustarse al crear el proyecto MPLAB X, pero se mantendrá la separación entre periféricos, drivers y lógica de control.

## Criterios de aceptación

- [ ] El firmware se compila con XC8 sin errores.
- [ ] La Miuva puede programarse de forma reproducible en macOS o Windows.
- [ ] El PCA9685 responde en la dirección configurada.
- [ ] Un servo puede centrarse sin golpes ni recorrido inesperado.
- [ ] Cada servo tiene límites documentados.
- [ ] Los dos ejes del joystick se leen de forma estable.
- [ ] Existe zona muerta alrededor del centro.
- [ ] Los servos no tiemblan perceptiblemente con el joystick en reposo.
- [ ] Los movimientos son graduales y no exceden límites.
- [ ] La parada mediante `OE` funciona.
- [ ] El sistema no reinicia la Miuva al mover ambos servos.

## Evidencias que se documentarán

- Versión de Miuva y del PIC.
- Modelo exacto de PCA9685.
- Modelo de fuente y voltaje configurado.
- Modelo de cada servo.
- Valores ADC mínimo, centro y máximo del joystick.
- Pulsos mínimo, centro y máximo de cada servo.
- Fotografías del cableado.
- Video o registro de la prueba final.
