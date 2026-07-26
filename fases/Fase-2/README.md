# Fase 2 — Rutinas de movimiento del robot

## Objetivo

Escalar la plataforma validada en la Fase 1 para controlar seis servomotores y ejecutar movimientos coordinados mediante posturas, keyframes e interpolación temporal.

Esta fase comenzará únicamente cuando la alimentación, el bus I²C, el PCA9685, la calibración y la parada segura hayan quedado validados en la Fase 1.

## Alcance

- Configuración de seis canales del PCA9685.
- Perfil independiente por servo.
- Posiciones de inicio y reposo.
- Posturas nombradas.
- Rutinas formadas por keyframes.
- Interpolación suave entre posiciones.
- Ejecución no bloqueante.
- Pausa, cancelación y parada de emergencia.
- Validación gradual de consumo y carga mecánica.

## Asignación propuesta

| Canal PCA9685 | Articulación |
|---:|---|
| 0 | Base |
| 1 | Hombro |
| 2 | Codo |
| 3 | Muñeca 1 |
| 4 | Muñeca 2 |
| 5 | Garra |

La asignación definitiva dependerá del cableado y de la geometría final del robot.

## Modelo de configuración por servo

Cada articulación deberá disponer de una configuración independiente:

```c
typedef struct {
    uint8_t channel;
    uint16_t min_us;
    uint16_t center_us;
    uint16_t max_us;
    int16_t min_angle_deg;
    int16_t max_angle_deg;
    uint16_t max_step_us;
    bool inverted;
} servo_config_t;
```

Los valores no se copiarán ciegamente entre servos. Cada articulación tendrá límites eléctricos y mecánicos propios.

## Modelo de postura y keyframe

```c
#define ROBOT_SERVO_COUNT 6U

typedef struct {
    uint16_t pulse_us[ROBOT_SERVO_COUNT];
} robot_pose_t;

typedef struct {
    robot_pose_t target;
    uint16_t duration_ms;
    uint16_t hold_ms;
} robot_keyframe_t;
```

Una rutina será una secuencia de keyframes con duración explícita. El motor de movimiento calculará posiciones intermedias sin utilizar retardos largos que bloqueen el programa.

## Rutinas iniciales previstas

- `HOME`: postura segura de inicio.
- `REST`: postura de bajo esfuerzo mecánico.
- `OPEN_GRIPPER`: apertura gradual de garra.
- `CLOSE_GRIPPER`: cierre limitado de garra.
- `GREETING`: secuencia demostrativa.
- `PICK_AND_PLACE_DEMO`: toma y depósito con posiciones previamente calibradas.
- `EMERGENCY_RELEASE`: deshabilitación o repliegue seguro según la condición.

## Motor de movimiento

El motor deberá:

1. Conservar la posición actual estimada de cada servo.
2. Recibir una postura objetivo.
3. Calcular incrementos según la duración solicitada.
4. Actualizar los seis canales en intervalos regulares.
5. Respetar límites y velocidad máxima.
6. Permitir cancelar una rutina.
7. Atender permanentemente la parada de emergencia.
8. Reportar errores de I²C o parámetros inválidos.

## Estrategia de implementación

### 1. Validación de canales

Añadir un servo a la vez, comenzando por el canal 0. No conectar los seis servos hasta verificar individualmente:

- Centro.
- Sentido de giro.
- Límites.
- Consumo.
- Ausencia de colisión mecánica.

### 2. Posturas estáticas

Definir y probar `HOME` y `REST`. Cada transición deberá realizarse lentamente.

### 3. Interpolación

Implementar primero interpolación lineal. Las curvas de aceleración y desaceleración se añadirán después de comprobar que la temporización básica sea estable.

### 4. Secuenciador

Ejecutar listas de keyframes y permitir pausa, reanudación y cancelación.

### 5. Rutinas completas

Crear rutinas pequeñas y verificables antes de programar secuencias largas.

## Seguridad

- Nunca iniciar una rutina si la posición actual es desconocida.
- No asumir que deshabilitar PWM sostiene una articulación.
- Evitar que una pérdida de torque deje caer una carga.
- Validar el espacio libre alrededor del robot.
- Limitar la garra para no sobrecargar el servo ni dañar objetos.
- Cancelar la rutina ante fallas repetidas de I²C.
- Mantener accesible la desconexión física de potencia.

## Criterios de aceptación

- [ ] Los seis servos tienen configuración y límites documentados.
- [ ] `HOME` y `REST` pueden ejecutarse sin golpes.
- [ ] Las transiciones usan interpolación y no saltos directos.
- [ ] El sistema puede cancelar una rutina en curso.
- [ ] La parada por `OE` funciona durante cualquier movimiento.
- [ ] No hay reinicios de la lógica durante movimientos simultáneos.
- [ ] El consumo y la caída de tensión se mantienen dentro de límites.
- [ ] Cada rutina está documentada con propósito, postura inicial y riesgos.

## Posibles ampliaciones posteriores

- Grabación manual de trayectorias.
- Comunicación USB con una aplicación de control.
- Sensores de límite o corriente.
- Cinemática inversa.
- Control coordinado desde una Jetson Nano o Raspberry Pi.
- Almacenamiento de rutinas en memoria no volátil.
