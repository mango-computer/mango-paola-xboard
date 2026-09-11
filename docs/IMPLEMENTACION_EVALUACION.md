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

## Regresiones y bloqueos

Todavía no se han observado regresiones durante esta ejecución.
