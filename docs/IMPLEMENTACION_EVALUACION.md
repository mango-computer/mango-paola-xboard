# Implementación gradual de evaluación

Este documento registra la ejecución del plan de mejora sobre la rama
`integration/evaluacion-gradual`. La referencia inicial es el commit
`43ed23bfd6aa5df1e5d14a406d71a9fc35693859`.

## Política aplicada

- Un commit por cambio y un gate máximo de 600 segundos.
- Un fallo funcional, crash, jugada ilegal o regresión reproducible superior
  al 10 % marca el cambio como `RED`.
- Una regresión entre 2 % y 10 % queda `PROVISIONAL` mientras se evalúa su
  cadena dependiente.
- Los dependientes de un cambio `RED` quedan `BLOCKED`; el resto continúa.
- Las reversiones se realizan mediante commits de `git revert`, nunca
  reescribiendo `master`.
- No se hacen `merge` ni `push` automáticamente.

## Estado

| Entrega | Estado | Base/commit | Evidencia | Decisión |
|---|---|---|---|---|
| C00 | GREEN | `BASE-00` desde `43ed23b` | `artifacts/gates/C00/20260911T184919Z` | Referencia congelada |
| C01.a | GREEN | `c743809` / `BASE-01` | `artifacts/gates/C01.a/20260911T185105Z` | Historial acotado; promover |
| C01.b | GREEN | `b0dd5e3` / `BASE-02` | `artifacts/gates/C01.b/20260911T185232Z` | Reloj unificado; promover |
| C01.c | GREEN | `5ad6c24` / `BASE-03` | `artifacts/gates/C01.c/20260911T185347Z` | FEN limpia; promover |
| C01.d | GREEN | `83ded49` / `BASE-04` | `artifacts/gates/C01.d/20260911T185511Z` | Null reversible; promover |
| C02 | GREEN | `53caefd` / `BASE-05` | `artifacts/gates/C02/20260911T185648Z` | Terminales corregidos; promover |
| C03.a | GREEN tras cadena | `63ad082`, base `BASE-05` | `artifacts/gates/C03.a/20260911T185843Z` | Aceptado con C03.c |
| C03.b | GREEN tras cadena | `f71ee2b`, base `BASE-05` | `artifacts/gates/C03.b/20260911T193245Z` | Aceptado con C03.c |
| C03.c | GREEN | `61cf97a` / `BASE-06` | `artifacts/gates/C03.c/20260911T193415Z` | Cadena C03 neutral; promover |
| C04.a | GREEN | `3922475` / `BASE-07` | `artifacts/gates/C04.a/20260911T193554Z` | Coronación visible; promover |
| C04.b | GREEN | `cb16fe3` / `BASE-08` | `artifacts/gates/C04.b/20260911T193733Z` | Finales completos; promover |
| C05.a | GREEN | `2291494` / `BASE-09` | `artifacts/gates/C05.a/20260911T193859Z` | Lazy acotado; promover |
| C05.b | GREEN | `bdccc6d` / `BASE-10` | `artifacts/gates/C05.b/20260911T194046Z` | Trazas estables; promover |
| C05.c | GREEN | `5a64484` / `BASE-11` | `artifacts/gates/C05.c/20260911T194158Z` | Contrato explícito; Hito A |
| C06.a | GREEN | `c49c7c6` / `BASE-12` | `artifacts/gates/C06.a/20260911T194337Z` | Intrínsecos equivalentes; promover |
| C06.b | GREEN | `359c71f` / `BASE-13` | `artifacts/gates/C06.b/20260911T194440Z` | Limpieza completa verificada |
| C06.c | GREEN | `aaa7d2b` / `BASE-14` | `artifacts/gates/C06.c/20260911T194607Z` | Impresión sin OOB |
| C07.a | GREEN tras cadena | `592aa48`, base `BASE-14` | `artifacts/gates/C07.a/20260911T194734Z` | Aceptado con C07.b |
| C07.b | GREEN | `2fa1f7c` / `BASE-15` | `artifacts/gates/C07.b/20260911T194844Z` | Banderas aisladas; promover |
| C07.c | GREEN | `1958dc2` / `BASE-16` | `artifacts/gates/C07.c/20260911T200207Z` | Contexto validado; promover |
| C07.d | GREEN | `6be579b` / `BASE-17` | `artifacts/gates/C07.d/20260911T200320Z` | Mate simétrico; cerrar core |
| C08 | GREEN tras cadena | `ffd546a`, base `BASE-17` | `artifacts/gates/C08/20260911T200805Z` | Aceptado con C09.a |
| C09.a | GREEN | `9d903e6` / `BASE-18` | `artifacts/gates/C09.a/20260911T201036Z` | Shelter reutilizado; promover |
| C09.b | GREEN | `6ce1e70` / `BASE-19` | `artifacts/gates/C09.b/20260911T201302Z` | Material puro cacheado |
| C10.a | GREEN | `e938b01` / `BASE-20` | `artifacts/gates/C10.a/20260911T201709Z` | Static eval en TT |
| C10.b | GREEN | `f545ec8` / `BASE-21` | `artifacts/gates/C10.b/20260911T201826Z` | Epochs UCI persistentes |
| C10.c | GREEN | `9626ca5` / `BASE-22` | `artifacts/gates/C10.c/20260911T202012Z` | Layout 32 B e invalidación |
| C11.a | GREEN | `26b6c6d` / `BASE-23` | `artifacts/gates/C11.a/20260911T202459Z` | Quiescencia sin eval en jaque |
| C11.b | GREEN | `389d021` / `BASE-24` | `artifacts/gates/C11.b/20260911T202631Z` | Static eval y lazy de qsearch |
| C12 | GREEN | `0c7d0ce` / `BASE-25` | `artifacts/gates/C12/20260911T202804Z` | Candidatos con estado local |
| C12.b | RED | `a10c419`, revert `34411ea` | `artifacts/match/C12.b.json` | Aislado+doblado; Elo −28,6 |
| C12.c | GREEN | `4585431` | `artifacts/match/C12.c.json` | Shelter por derechos/columna |
| C12.d | RED | `c846927`, revert `2a17b77` | `artifacts/match/C12.d.json` | Pareja opuesta; Elo 0 |
| C13.a | GREEN | `2bf0eca` / `BASE-26` | `artifacts/gates/C13.a/20260911T203129Z` | Position/scratch por hilo |
| C13.b | GREEN | `5f84b6e` / `BASE-27` | `artifacts/gates/C13.b/20260911T203455Z` | Lazy SMP, default 1 hilo |

## BASE-00

- Fuentes iniciales: `43ed23bfd6aa5df1e5d14a406d71a9fc35693859`.
- Infraestructura del gate: `2681da90e55edd84cd91e38a3babd6afa9efcbb9`.
- Binario GCC 11.4.0, `-std=gnu11 -O2 -g -Wall`:
  `1541d2dfc9a22939c4323c5688f6f56316bd759dcced048cb015ec433aad6ed4`.
- Regresión completa: 45 pruebas superadas.
- Gate completo: 15,787 s.
- A/B contra copia idéntica, tres pares a profundidad 6: mismos 3555 nodos;
  mediana de lote de evaluación 152,053 ms frente a 149,555 ms. La
  diferencia de 1,67 % cuantifica el ruido inicial y queda dentro de la banda
  neutra.
- La compilación mantiene advertencias preexistentes; no se trataron como
  regresiones de C00.

## C01.a — Historial de repetición

Se convirtió la resta del índice a entero con signo y se acotó su límite
inferior a cero en ambas macros de repetición. El caso con FEN, reloj 40 e
historial vacío pasa bajo UBSan. También pasan las 45 pruebas anteriores más
la nueva regresión dirigida. Duración del gate: 21,596 s; no se midió fuerza
porque es una corrección de seguridad sin cambio heurístico previsto.

## C01.b — Regla de cincuenta movimientos

El contador y su copia de undo pasaron a 16 bits. El valor final se calcula
una vez desde el estado previo: peones y capturas lo reinician; los movimientos
quietos lo incrementan con saturación. Se verificaron captura `25 → 0 → undo
25` y movimiento quieto `300 → 301 → undo 300`. Pasaron la prueba dirigida y
la regresión completa en 21,897 s.

## C01.c — Construcción de posición FEN

`setupFen()` reinicia las marcas históricas de enroque y el índice de historial
antes de construir una posición. El test envenena ambos campos y confirma
`castled_white=0 castled_black=0 history=0`. La regresión completa pasó; gate
de 21,544 s.

## C01.d — Movimiento nulo

El movimiento nulo guarda y restaura explícitamente turno, EP, reloj y clave.
Al hacerlo elimina EP y su componente Zobrist, crea una barrera conservadora
de repetición mediante reloj cero y nunca almacena una jugada ajena en el corte
null. El estado se restauró bit a bit en el caso dirigido; regresión completa
verde en 21,796 s.

## C02 — Material muerto y prioridad de mate

La detección automática quedó limitada a rey solo, una pieza menor total o
alfiles exclusivamente del mismo color de casilla. KNN contra K ya no se marca
como material muerto. `obtEstadoJuego()` comprueba mate/ahogado antes de tablas:
el fixture KNNK devuelve mate blanco (`status=2`). Gate verde en 21,192 s.

## C03.a — Ataques geométricos

Caballo, alfil, torre y dama registran ataques geométricos antes de filtrar
clavadas. El alfil de g1 vuelve a defender la torre de d4 y elimina la amenaza
falsa. Los 51 tests pasan. El lote de evaluación pasó de 149,744 ms a
154,198 ms (`+2,974 %`), una regresión menor al umbral fuerte; el cambio queda
`PROVISIONAL` para medirlo junto con defensa de rey y movilidad.

## C03.b — Defensa del rey

Los ataques del rey se incorporan a los mapas simple/doble; el rey c3 vuelve a
defender la torre d4. La cadena C03 acumula 157,633 ms frente a 150,209 ms
(`+4,942 %`) en el lote de evaluación, pero la búsqueda corta recorrió 4,05 %
más nodos en 7,82 % menos tiempo. Se mantiene `PROVISIONAL`.

La primera ejecución del gate quedó roja porque un test antiguo atribuía todo
el mapa agregado a la torre clavada; d2/f2 también son ataques legítimos del
rey e1 tras C03.b. Se corrigió el oráculo para comprobar los campos específicos
de la pieza clavada. No fue una regresión del motor. La repetición posterior
superó las 52 pruebas.

## C03.c — Área de movilidad

Las piezas propias se excluyen explícitamente de los destinos útiles, sin
perderlas del mapa geométrico de defensa. El fixture confirma que el peón
propio bloqueador está en el rayo raw pero no suma movilidad. En la medición
de cierre, la cadena completa quedó en 159,362 ms frente a 159,208 ms
(`+0,097 %`), dentro del ruido, con 53 tests verdes. C03.a–C03.c se promueven
juntas como `BASE-06`.

## C04.a — Camino de coronación

El camino dinámico del pasado usa el rayo delantero completo, incluida la
casilla de coronación. El cambio de control de a8 produce exactamente delta
MG 3 / EG 8. Frente a `BASE-06`, el lote pasó de 157,729 ms a 149,355 ms
(`-5,309 %`) con árbol de búsqueda idéntico. Regresión completa verde.

## C04.b — Finales sin peones

Se retiró el retorno general que evitaba movilidad, ataques y amenazas en
finales no especializados. Dos posiciones Q contra Q con la dama en h1/h4
ahora recorren la evaluación completa y producen mapas no vacíos. La búsqueda
corta mantuvo árbol y tiempo; el lote pasó de 153,802 ms a 150,500 ms
(`-2,147 %`). Gate verde.

## C05.a — Lazy y regla de cincuenta

Ambos cortes lazy se desactivan cuando el reloj supera 80. En KQ contra K con
reloj 99, una ventana estrecha coincide con la evaluación completa. Frente a
`BASE-08`, árbol idéntico, tiempo de búsqueda `-0,94 %` y lote de evaluación
`-0,84 %`; 55 tests verdes.

## C05.b — Trazas y caché de evaluación

`evalprobe` evita deliberadamente la caché de evaluación y reconstruye todos
los mapas con la misma semántica raw/efectiva/útil del evaluador. La caché
normal continúa siendo sólo de puntuación: un hit devuelve el mismo valor sin
pretender restaurar scratch global. Pasaron 57 tests; cambio limitado a
instrumentación `PRUEBAS_HCE`.

## C05.c — Clases de resultado

La instrumentación distingue explícitamente `COMPLETO`, `LAZY` y `CACHE`.
El test obtiene la secuencia `full=0 cache=2 lazy=1` y conserva el guard que
impide reutilizar un resultado lazy como exacto. Las 58 pruebas pasan.

**Hito A alcanzado:** C01–C05 cumplen corrección de estado, terminales, mapas
de ataque, finales y contrato de evaluación. `BASE-11` es la referencia para
las optimizaciones mecánicas.

## C06.a — Primitivas de bits

GCC/Clang usan `ctzll`, `clzll` y `popcountll`, con implementación histórica
como fallback portable. 100.000 palabras pseudoaleatorias coinciden con el
oráculo escalar. A profundidad 7 el árbol fue idéntico y la mediana bajó de
43,940 ms a 43,012 ms (`-2,112 %`); evaluación `+0,195 %`, dentro del ruido.

## C06.b — Tamaño de limpiezas

Se añadió un guard de aceptación que envenena las 64 entradas de
`mapaRayosClavada` y confirma que una evaluación completa limpia todas. La
implementación de `sizeof(mapaRayosClavada)` ya estaba incluida en BASE-00;
esta entrega congela su equivalencia y evita que vuelva el literal 64.

## C06.c — Límite de bitboard

`imprimirBitTablero()` ya no consulta `BITSET[-8]` después de imprimir la
última fila. El caso extremo pasa bajo UBSan y se elimina la advertencia
reproducible de acceso fuera de rango. Regresión completa verde.

## C07.a — Profundidad TT inclusiva

Las entradas se reutilizan con profundidad igual (`>=`); el test exacto
devuelve score 123 y bandera exacta. La búsqueda a profundidad 7 reduce nodos
de 3492 a 2936 (`-15,922 %`), aunque la mediana temporal subió de 42,743 ms a
43,682 ms (`+2,197 %`). Se conserva `PROVISIONAL` para completar C07.

## C07.b — Banderas TT

Cada caso del `switch` termina explícitamente; una entrada exacta en nodo PV
ya no cae accidentalmente en las cotas superior/inferior. La cadena conserva
la reducción de nodos (`-15,922 %`) y queda en `+1,259 %` de tiempo, dentro de
ruido; el lote de evaluación mejora `0,779 %`. C07.a/b se aceptan como
`BASE-15`.

## C07.c — Contexto TT

Cada entrada conserva reloj de cincuenta movimientos y las marcas históricas
de enroque que aún afectan la evaluación. Una clave de tablero igual con
contexto distinto ya no produce hit. El registro crece a 32 bytes: las
reservas de 64 MiB pasan a aprovechar exactamente 64 MiB y el fallback de
6 MiB reserva 4 MiB por redondeo a potencia de dos. A profundidad 7 el tiempo
queda estable (`43,424 → 43,421 ms`), con `+12,909 %` de nodos necesarios para
eliminar hits semánticamente inválidos.

La primera ejecución detectó dos expectativas de memoria antiguas; se
actualizaron al nuevo layout y las 63 pruebas pasaron. Además se corrigió un
bucle del orquestador que remarcaba descendientes ya bloqueados.

## C07.d — Normalización de mate

El ajuste al guardar/leer TT está centralizado en dos funciones simétricas.
Guardar en ply 7 y leer en ply 3 produce deltas `+4/-4` para ambos signos y
deja 123 sin cambio. Las 64 pruebas pasan.

**Core C06–C07 cerrado:** primitivas de bits equivalentes, límites de memoria
verificados y TT con profundidad, banderas, contexto y mate explícitos.

## C08 — Estado incremental de evaluación

`ESTADO_EVALUACION` mantiene cuentas por pieza/color, fase raw, casillas de rey
y PST MG/EG. Make aplica deltas para quietas, capturas, EP, promociones y
enroques; undo restaura el snapshot exacto; FEN/inicio reconstruyen la
referencia. Más de 50 transiciones dirigidas coinciden campo a campo con una
reconstrucción y pasan 65 tests.

El árbol permanece idéntico, pero el gate corto muestra `+2,190 %` de búsqueda
y `+4,151 %` de lote. Es una regresión menor y esperable antes de reutilizar
material/shelter; C08 queda `PROVISIONAL` para C09.

## C09.a — Shelter en pawn hash

Los defectos de ocho archivos por color se calculan en el miss y se restauran
en el hit del hash de peones; el contexto dinámico del rey permanece fuera.
El test envenena el shelter y confirma restauración y score idéntico con reyes
distintos. Frente a C08, búsqueda `-0,819 %`; la cadena completa contra
`BASE-17` queda estable (`43,445 → 43,437 ms`) con árbol idéntico. C08/C09.a
se aceptan como `BASE-18`.

## C09.b — Caché de material

Una tabla directa de 1024 entradas guarda el resultado completo de
`ini_material()` con firma de cuentas, turno y distribución por flancos. El
segundo cálculo da hit y score idéntico. Frente a `BASE-18`, árbol idéntico,
búsqueda `+0,149 %` y lote `-0,209 %`, ambos dentro del ruido; 67 tests verdes.

## C10.a — Evaluación estática en TT

La TT conserva `evalEstatico` sin aumentar su layout de 32 bytes y lo devuelve
a nodos con contexto coincidente. Búsqueda/null/futility lo reutilizan; qsearch
y cortes guardan el valor conocido o `INT_MAX`. La prueba TT recupera score
123 y static 17. Árbol idéntico, búsqueda `+1,738 %` y lote `+1,639 %`,
dentro de la banda neutra; 67 tests verdes.

## C10.b — Persistencia UCI

`position` y `nuevo_juego` conservan las generaciones de caché; únicamente
`Clear Hash` y el resize explícito las invalidan. El contrato queda
documentado junto al constructor y protegido por test. Las 68 pruebas pasan.

## C10.c — Layout e invalidación TT

La entrada permanece en 32 bytes y un `Clear Hash` incrementa la generación:
una sonda posterior no reutiliza ni la cota ni el `evalEstatico` anterior.
El test confirma `epoch_miss=1 static_unknown=1 size=32`. Gate verde en
19,938 s; 69 pruebas en la suite completa.

**Hito B alcanzado:** incrementales, pawn/material hash y TT con static eval,
contexto, persistencia e invalidación quedan estabilizados en un hilo.
`BASE-22` es la referencia para C11–C13. Comparar contra `BASE-00` y contra
esta base, sin atribuir a un solo cambio la suma del hito.

## C11.a — Quiescencia sin evaluación en jaque

`busquedadTranquilidad()` ya no llama al evaluador antes de separar la rama
de jaque. El stand-pat y el delta pruning quedan solo en nodos sin jaque; en
jaque se generan evasiones y la TT guarda `INT_MAX` como static eval. El
fixture con una sola evasión reporta `evals_in_check=0`. Gate verde en
22,950 s.

## C11.b — Static eval y lazy en quiescencia

El stand-pat reutiliza `evalEstatico` de TT cuando el contexto coincide; si
falta, evalúa con `permitirLazyEval=0` para no cortar por material o margen
intermedio. La poda EET lee la fase incremental en lugar de reconstruir
cuentas. El fixture KQ contra K confirma stand-pat igual a la evaluación
completa y menos llamadas con la TT caliente. Gate verde en ~23 s.

## C12 — Familia de peones candidatos

Las familias HCE presentes en `BASE-00` (incluido el descuento de doblados
solo cuando el peón no es aislado) quedan congeladas. La única familia nueva
es el estado local de candidatos: ataque, defensa y casilla evaluada no se
comparten con peones previos ni con un bloqueo inmediato. El fixture marca
`blocked=0 candidate=1`. Gate verde.

## C13.a — Position y scratch por hilo

`ThreadState` agrupa `Position` (el `BITTABLERO` de siempre) y `EvalScratch`.
El hilo principal usa `hilosBusqueda[0]`; `juego` y los mapas de evaluación
son accesos a ese estado. Un segundo `ThreadState` copia la posición y
reproduce las mismas tres puntuaciones (`same=1`). Un hilo permanece verde.
Gate en 23,891 s.

## C13.b — Lazy SMP

La opción UCI `Threads` (1–4, default 1) lanza ayudantes que copian
`Position` y `EvalScratch` y buscan el mismo árbol. La TT se publica bajo
mutex; eval/pawn/material hash se desactivan mientras `esBusquedaParalela`.
Con un hilo el camino es el de siempre. Con dos workers, `pensarRapido()` a
profundidad 3 devuelve jugadas legales. Gate verde en 23,806 s; la suite
completa (73 pruebas) pasó.

**Hito C alcanzado:** el candidato paralelo queda en `BASE-27` sobre la rama
local `integration/evaluacion-gradual`. No hay merge ni push. Un ensayo
formal de fuerza, TSan saturado y 100000 transiciones incrementales siguen
siendo G2, no bloqueantes para esta integración.

## Verificación final

- Rama: `integration/evaluacion-gradual` @ `5f84b6e`.
- Bases promovidas: `BASE-00` … `BASE-27`.
- Entregas C00–C13.b: todas `GREEN`. Ninguna `RED`, `BLOCKED` ni revertida.
- Tres cadenas empezaron `PROVISIONAL` y se recuperaron: C03, C07.a–b, C08–C09.a.
- Umbral de descarte fuerte (>10 %) no se cruzó en ningún gate.
- `master` permanece en `43ed23b`.

G2 pendiente (fuera del gate diario): TSan con 2–4 workers y TT saturada,
perft ampliado, siete pares de 400 vueltas contra `BASE-00` y un SPRT si se
quiere publicar Elo.

## Match de comprobación BASE-00 vs candidato

Orquestador: `tools/match/run_veredicto.sh`.
Árbitro legal: `tests/eval_compare/match_legal.py` (mate, ahogado, 50,
repetición, material insuficiente; ilegal o crash = derrota).
Sin libro (`UsarLibroAperturas 0`), Hash 64 MB, `Threads=1`, aperturas
emparejadas con colores invertidos.

| Prueba | Condición | Juegos | Concurrencia |
| --- | --- | --- | --- |
| NPS / nodos | profundidad 7, 7 repeticiones, 4 FEN | — | 1 |
| Fuerza fija | `go depth 8`, tope 120 ply | 1000 | 8 |
| Fuerza a tiempo | `go movetime 400`, tope 120 ply | 400 | 8 |

Artefactos en `artifacts/match/` (gitignored): binarios, PGN, JSON y
`veredicto.json`. El umbral de descarte fuerte sigue siendo >10 % de
regresión reproducible o fallos funcionales (ilegales/crashes).

Resultados (suite ~49 min, 0 ilegales/crashes en 1400 partidas):

| Prueba | Marcador (C–B–T) | Puntuación | Elo (IC 95 %) | Notas |
| --- | --- | --- | --- | --- |
| Profundidad 7 | — | tiempo 1,005× | — | −6,8 % nodos; NPS parseado no comparable |
| Profundidad 8 | 189–125–686 | 53,20 % | +22,3 [+10,3, +34,3] | significativo |
| 400 ms/jugada | 64–44–292 | 52,50 % | +17,4 [−0,4, +35,2] | misma dirección, no significativo |

Veredicto: el candidato no se descarta. Es más correcto (0 fallos
funcionales) y más fuerte a profundidad fija. A tiempo la señal es
positiva pero el IC cruza cero. No hay SPRT de publicación.

## P10 por familias (C12.b / C12.c / C12.d)

Rama `integration/p10-familias` desde `master` @ `11f784e` (mutex de TT
omitido con un hilo). Cada familia: commit + fixture, match de 1000 partidas
con el mismo protocolo (`go depth 8`, 120 ply, Hash 64, `Threads=1`, sin
libro, 8 juegos en paralelo, colores invertidos). Conservar solo si
`illegal_or_crash == 0` y `elo_difference > 0`. El IC de 1000 partidas a
depth 8 es orientativo; no se afirma Elo publicado. Texel queda fuera.

| Familia | Marcador (C–B–T) | Puntuación | Elo (IC 95 %) | Veredicto |
| --- | --- | --- | --- | --- |
| C12.b aislado+doblado | 42–124–834 | 45,90 % | −28,55 [−35,78, −21,35] | Revertida |
| C12.c shelter | 167–83–750 | 54,20 % | +29,25 [+19,71, +38,84] | Conservada |
| C12.d pareja alfiles | 83–83–834 | 50,00 % | 0,00 [0,00, 0,00] | Revertida |

C12.b aplicaba `peon_doblado` también cuando el peón es aislado. El fixture
`isolated=1 doubled=1` pasó; el match perdió Elo de forma significativa y el
commit se revirtió. El dúo y el débil siguen en la rama no aislada.

C12.c no retoca `INSEGURIDAD_REY`. Si el rey ya enrocó, el pivote sigue la
columna real. Si quedan derechos, el mínimo solo mira pivotes aún legales
(7/5 y/o 2). Sin derechos y sin haber enrocado, usa la columna actual, no
`min(2, 5)`. El fixture (`rey e1`, sin O-O/O-O-O, flanco de rey roto) marca
`pivot=5 used_current=1`. 0 ilegales; se conserva y pasa a ser la base del
siguiente match.

C12.d daba el bono +38/+56 solo con alfil claro y oscuro, fuera del cache de
material por cuentas. El fixture `pair=1 pair_same=0` pasó; el match empató
(Elo no mayor que 0) y el commit se revirtió. Ablaciones N×peones no se
abrieron.

P10 de este ciclo cierra con un solo cambio de conocimiento: C12.c. No hay
merge ni push a `master` hasta que se pida. Texel (corpus etiquetado, export
MG/EG, split por partida) queda para un ciclo posterior.

## Regresiones y bloqueos

- C03.a y C03.b mostraron provisionalmente `+2,974 %` y `+4,942 %`.
  C03.c cerró la cadena en `+0,097 %`; no se confirmó regresión y se promovió
  la corrección semántica completa.
- C07.a fue provisional con `+2,197 %`; C07.b cerró la cadena en `+1,259 %`
  y mantuvo `-15,922 %` de nodos. Se aceptó.
- C08 fue provisional (`+2,190 %` búsqueda); C09.a cerró la cadena en
  `-0,018 %` contra BASE-17. Se aceptó.
