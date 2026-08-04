#!/usr/bin/env bash
set -u

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../../.." && pwd)"
cd "$ROOT" || exit 1

XC8="/c/Program Files/Microchip/xc8/v3.10/bin/xc8-cc.exe"
MPLAB_PACKS="/c/Program Files/Microchip/MPLABX/v6.30/packs"
USER_PACKS="$HOME/.mchp_packs"
SRC="fases/Fase-1/firmware/src"
BUILD="fases/Fase-1/firmware/build/adc-hw504"
DIST="fases/Fase-1/firmware/dist"
TARGET="RobotMiuvaHW504Control"

printf '%s\n' "============================================================"
printf '%s\n' " COMPILACION: HW-504 Y DOS SG90"
printf '%s\n' "============================================================"

if [ ! -f "$XC8" ]; then
    echo "[ERROR] No se encontro XC8 en: $XC8"
    exit 1
fi

HEADER="$({
    [ -d "$MPLAB_PACKS" ] && find "$MPLAB_PACKS" -type f -iname "pic18f45k50.h" 2>/dev/null
    [ -d "$USER_PACKS" ] && find "$USER_PACKS" -type f -iname "pic18f45k50.h" 2>/dev/null
} | sort -V | tail -n 1)"

if [ -z "$HEADER" ]; then
    echo "[ERROR] No se encontro el Device Family Pack del PIC18F45K50."
    exit 1
fi

DFP="${HEADER%%/pic/include/proc/*}"

echo "[OK] XC8: $XC8"
echo "[OK] DFP: $DFP"

rm -rf "$BUILD"
mkdir -p "$BUILD" "$DIST"

"$XC8" \
    -mcpu=18F45K50 \
    -mdfp="$DFP" \
    -O0 \
    -I"$SRC" \
    -I"$SRC/drivers" \
    -o "$BUILD/$TARGET.elf" \
    "$SRC/main.c" \
    "$SRC/config_bits.c" \
    "$SRC/drivers/adc.c" \
    "$SRC/drivers/i2c_master.c" \
    "$SRC/drivers/pca9685.c" \
    2>&1 | tee "$BUILD/compile.log"

STATUS=${PIPESTATUS[0]}

if [ "$STATUS" -ne 0 ]; then
    echo "[ERROR] La compilacion fallo con codigo $STATUS."
    exit "$STATUS"
fi

HEX="$BUILD/$TARGET.hex"
if [ ! -f "$HEX" ]; then
    echo "[ERROR] XC8 termino sin generar el archivo HEX."
    exit 1
fi

cp "$HEX" "$DIST/$TARGET.hex"

echo
echo "[OK] HEX generado:"
ls -lh "$DIST/$TARGET.hex"

echo
echo "SHA-256:"
sha256sum "$DIST/$TARGET.hex"
