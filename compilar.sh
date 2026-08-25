#!/usr/bin/env bash
# Compila mangoac y lo ejecuta si la compilación termina sin errores.

cd "$(dirname "$0")" || exit 1

SRC="bitmma3.c"
BIN="mangoac"
MAX_ERRORES=12

echo "Compilando ${SRC}..."

salida=$(gcc -lm "$SRC" -Wall -O2 -fmax-errors=10 -o "$BIN" 2>&1)
estado=$?

if [ "$estado" -eq 0 ]; then
	echo "Compilación correcta. Ejecutando ${BIN}..."
	echo
	exec "./$BIN"
fi

echo "Falló la compilación:"
echo

errores=$(printf '%s\n' "$salida" | grep -E 'error:|fatal error:|undefined reference|collect2:' | head -n "$MAX_ERRORES")

if [ -n "$errores" ]; then
	printf '%s\n' "$errores"
	total=$(printf '%s\n' "$salida" | grep -cE 'error:|fatal error:|undefined reference' || true)
	if [ "$total" -gt "$MAX_ERRORES" ]; then
		echo
		echo "... y $((total - MAX_ERRORES)) error(es) más."
	fi
else
	printf '%s\n' "$salida" | tail -n 20
fi

exit "$estado"
