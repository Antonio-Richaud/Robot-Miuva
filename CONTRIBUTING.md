# Guía de contribución

## Flujo de trabajo

1. Partir de `main` actualizado.
2. Crear una rama descriptiva.
3. Mantener cada cambio enfocado en una sola tarea.
4. Compilar y probar antes de confirmar cambios de firmware.
5. Documentar cualquier modificación de cableado, voltaje o pinout.
6. Abrir un pull request con evidencia de las pruebas realizadas.

## Convención de ramas

```text
feature/descripcion
fix/descripcion
docs/descripcion
experiment/descripcion
```

## Convención de commits

Ejemplos:

```text
feat: agrega driver I2C maestro
feat: controla primer servo con PCA9685
fix: corrige calculo del prescaler PWM
docs: documenta calibracion del joystick
test: agrega prueba de limites de pulso
```

## Reglas para firmware

- Usar C compatible con MPLAB XC8.
- Evitar valores mágicos; usar constantes con nombre.
- Indicar unidades en nombres o comentarios: `_us`, `_ms`, `_hz`.
- Validar índices de canal y rangos antes de escribir al PCA9685.
- Incluir timeout en operaciones que puedan bloquearse.
- Evitar retardos largos dentro de la lógica principal.
- Mantener separados drivers, configuración y control de movimiento.
- Documentar cualquier dependencia de versión de MPLAB X o XC8.

## Archivos que sí deben versionarse

- Código fuente y cabeceras.
- Archivos públicos necesarios para abrir y compilar el proyecto MPLAB X.
- Documentación de cableado y calibración.
- Configuración de reloj y bits de configuración.
- Evidencias pequeñas y útiles para reproducir pruebas.

## Archivos que no deben versionarse

- Preferencias privadas de MPLAB X o NetBeans.
- Directorios `build`, `dist`, `debug` o `release`.
- Binarios y archivos generados: `.hex`, `.elf`, `.o`, `.map`, `.lst`.
- `.DS_Store`, `Thumbs.db` y archivos temporales del editor.
- Copias comprimidas o respaldos locales.
- Credenciales, variables privadas o datos sensibles.

## Cambios eléctricos

Todo cambio que afecte alimentación o cableado debe incluir:

- Voltaje utilizado.
- Fuente y corriente nominal.
- Pines modificados.
- Estado de la tierra común.
- Modelo exacto del módulo o servo.
- Riesgos conocidos.
- Procedimiento de validación.

## Pull requests

La descripción debe indicar:

- Qué cambió.
- Por qué cambió.
- Cómo se probó.
- Qué hardware se utilizó.
- Qué queda pendiente.
- Fotografías, mediciones o videos cuando sean relevantes.

No se debe mezclar en un mismo pull request una modificación eléctrica grande con una rutina compleja sin pruebas intermedias.
