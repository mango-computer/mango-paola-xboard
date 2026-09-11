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
| C00 | Pendiente | `43ed23b` | Pendiente | Congelar referencia actual |

## Regresiones y bloqueos

Todavía no se han observado regresiones durante esta ejecución.
