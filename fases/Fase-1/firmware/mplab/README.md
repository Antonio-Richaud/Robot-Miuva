# Proyecto MPLAB X

El proyecto de MPLAB X se creará posteriormente en Windows dentro de esta carpeta:

```text
mplab/RobotMiuvaFase1.X
```

Configuración prevista:

- Family: 8-bit MCUs.
- Device: PIC18F45K50.
- Compiler: MPLAB XC8.
- Hardware Tool inicial: Simulator.
- Project Encoding: UTF-8.
- Set as Main Project: activado.
- MCC: no se utilizará durante la prueba mínima.

Después de crear el proyecto se deben agregar como archivos existentes:

```text
../src/main.c
../src/config_bits.c
```

Los archivos fuente no deben copiarse dentro del directorio `.X`. El proyecto debe referenciarlos desde `src/` para evitar duplicados y mantener una sola fuente de verdad.

Los archivos públicos de `nbproject/` podrán versionarse cuando el proyecto sea creado y validado. Las preferencias privadas, archivos temporales y resultados de compilación están excluidos mediante `.gitignore`.
