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
| C07.a | PROVISIONAL | `592aa48`, base `BASE-14` | `artifacts/gates/C07.a/20260911T194734Z` | Mantener para cadena TT |

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

## Regresiones y bloqueos

- C03.a y C03.b mostraron provisionalmente `+2,974 %` y `+4,942 %`.
  C03.c cerró la cadena en `+0,097 %`; no se confirmó regresión y se promovió
  la corrección semántica completa.
- C07.a: `+2,197 %` de tiempo con `-15,922 %` de nodos; se mantiene
  provisional para medir la semántica TT completa.
