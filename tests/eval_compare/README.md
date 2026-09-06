# Ciclo HCE reproducible

El profesor es Stockfish en el commit
`6a8767a0d5d9502e6d4de1bef97468b5d6fab80a`, ejecutado con
`Use NNUE=false`. No se debe sustituir por `master`, porque ya no contiene la
evaluación clásica.

Compilación:

```sh
gcc -O2 -Wall bitmma3.c -lm -o tests/mangoac_candidato
git clone https://github.com/official-stockfish/Stockfish.git /tmp/stockfish-hce
git -C /tmp/stockfish-hce checkout 6a8767a0d5d9502e6d4de1bef97468b5d6fab80a
make -C /tmp/stockfish-hce/src -j build ARCH=x86-64-modern
```

Corpus reproducible desde las partidas PGN incluidas en el proyecto:

```sh
python3 tests/eval_compare/build_corpus.py \
  --output tests/eval_corpus.json --target 500 --stride 4
```

El corpus actual contiene 500 posiciones legales sin jaque. Las posiciones
procedentes de una misma partida permanecen juntas en uno de tres conjuntos:
calibración, validación o test. El comparador informa intervalos bootstrap del
95 % para MAE y aborta si el candidato no es determinista.

Comparación docente:

```sh
python3 tests/eval_compare/compare.py \
  --base tests/mangoac_base \
  --candidate tests/mangoac_candidato \
  --stockfish /tmp/stockfish-hce/src/stockfish \
  --output tests/eval_compare/report.json
```

Puerta de rendimiento (medianas, calentamiento y afinidad):

```sh
python3 tests/eval_compare/performance.py \
  --base tests/mangoac_base --candidate tests/mangoac_candidato \
  --depth 8 --repetitions 21 --cpu 0
```

Partidas paralelas:

```sh
python3 tests/eval_compare/selfplay.py \
  --base tests/mangoac_base --candidate tests/mangoac_candidato \
  --games 1000 --concurrency 10 --depth 5 --max-plies 100 \
  --openings tests/eval_corpus.json
```

Una mejora se promueve solo si pasa regresión/perft, no pierde más de 2 % de
NPS, mejora la validación docente no vista y no da señal negativa en partidas.
Los scripts solo miden; nunca modifican pesos ni código del motor.

`selfplay.py` informa W/D/L, puntuación del candidato, error estándar, hashes de
los binarios y toda la configuración. El modo integrado acepta el corpus JSON
como suite de posiciones y empareja cada FEN con colores invertidos. Sin
`cutechess-cli` sigue siendo un smoke test de señal de fuerza, no una medición
Elo reglamentaria.

Variantes de ablación disponibles al compilar:

```sh
gcc -O2 -Wall -DDESHABILITAR_LAZY_EVAL bitmma3.c -lm -o /tmp/mango-no-lazy
gcc -O2 -Wall -DMOVILIDAD_ESCALA=125 bitmma3.c -lm -o /tmp/mango-mob125
gcc -O2 -Wall -DDESHABILITAR_INTERACCIONES_MATERIAL bitmma3.c -lm -o /tmp/mango-no-material
gcc -O2 -Wall -DDESHABILITAR_NUEVOS_SCALERS bitmma3.c -lm -o /tmp/mango-no-scalers
```

Decisiones de esta iteración:

- amenazas: contabilidad corregida y pesos explícitos al 150 %; obtuvo 50,75 %
  en 1.000 partidas smoke diversificadas contra el control pre-informe;
- lazy OFF: rechazado por aumentar aproximadamente 5 % el tiempo medido;
- SEE dentro de amenazas: rechazado por empeorar MAE/RMSE de validación;
- movilidad 75/125 %: señales contradictorias y no significativas; se mantiene
  100 %;
- interacciones materiales: se mantienen porque retirarlas empeoró validación;
- nuevos draw scalers: ablación inconclusa; se conservan con pesos pequeños;
- detección correcta del pasado con peón enemigo detrás: se conserva por
  correctitud y señal docente ligeramente favorable.

Resultado final reproducible:

- regresión: 38/38 pruebas;
- perft inicial a profundidad 4: 197.281 nodos;
- profesor, validación: MAE 55,281 → 53,900 y RMSE 83,141 → 81,649;
- profesor, test: MAE 46,144 → 44,723 y RMSE 57,629 → 56,382;
- evaluación por lote: 1,019 veces la velocidad del control;
- búsqueda a profundidad fija: tiempo relativo 0,9995; el NPS no es comparable
  porque el candidato recorrió 63,9 % menos nodos;
- smoke A/B diversificado: 257/502/241, 50,80 % ± 1,12 % para el candidato.

Los datos consolidados están en `final-summary.json`. La decisión es promover
el binario candidato a validación de torneo; aún hace falta un match
reglamentario con `cutechess-cli` para estimar Elo.
