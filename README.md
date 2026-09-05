# Mango AC Ajedrez

Soy Jose Andres Morales Linares (Mango Computer). Este es el motor de ajedrez que escribí para competir entre motores y para cualquier interfaz que hable **UCI** o **xboard/WinBoard** (CECP).

Lo llamo **Mango AC** en honor a mis hijos, Alexander y Camila.

El núcleo —búsqueda, evaluación y generación de jugadas— es el motor que creé en **2012**. No lo he reescrito. Lo que he cambiado es cómo habla con la interfaz.

Licencia: [GPLv3](LICENSE).  
Contacto: comprasmangocomputer@gmail.com  
Blog oficial: [Mango AC Ajedrez](http://mangocomputerca.blogspot.com/2013/03/mango-computer-ca.html)

## Jugar

Es un **motor** (UCI y xboard), no una ventana de tablero. Se engancha a una interfaz: [Cute Chess](https://github.com/cutechess/cutechess), Arena, WinBoard, o la consola.

1. Compila (abajo) o, cuando haya paquetes, baja el zip de [Releases](https://github.com/mango-computer/mango-paola-xboard/releases).
2. Deja juntos el binario (`mangoac` o `mangoac.exe`), `mangoac.ini` y la carpeta `libros/`.
3. En la GUI, añade el motor como UCI (o xboard/WinBoard). En consola: `./mangoac`, luego `ayuda`.

## Sugerir mejoras

Abre un [Issue](https://github.com/mango-computer/mango-paola-xboard/issues) o un Pull Request. Cómo hacerlo está en [CONTRIBUTING.md](CONTRIBUTING.md). No hace falta cuenta de colaborador: el repo es público y `master` no se pisa desde fuera.

---

## El motor de 2012 permanece intacto

La representación del tablero, la generación de movimientos, la evaluación estática, el árbol de búsqueda y las tablas hash son las que diseñé entonces. No he tocado el algoritmo de juego ni los pesos de evaluación para “modernizar” el estilo.

Lo único que he añadido o adaptado es el **protocolo de E/S**: cómo una GUI (Cute Chess, Arena, mi propia interfaz, etc.) coloca una posición, pide una jugada y lee el resultado. El bucle de consola original sigue ahí; `xboard` y `uci` son puertas hacia el mismo `pensarRapido()`.

---

## Cambios en el protocolo de comunicación

En 2012 el motor solo hablaba **CECP/xboard**. Esa ruta la conservo. Ahora he añadido **UCI** (`uci.c`) y he actualizado el nombre con el que se presenta.

### Arranque

El binario entra en consola interactiva (`mangoac [blancas]>`). Desde ahí:

| Comando | Efecto |
|---|---|
| `uci` | Entra en el bucle UCI (handshake `id` / `uciok`) |
| `xboard` | Entra en el bucle CECP |
| `ayuda` / `help` | Comandos de consola |
| `salir` / `quit` | Cierra libro, bitbases (si hay) y termina |

Dejo `stdout` sin buffer (`setbuf(stdout, NULL)`) para que las GUI no se cuelguen esperando un flush.

### UCI (nuevo)

Lo implementé en `uci.c`. El motor responde al subconjunto que usan las GUI habituales:

**Handshake**

- `uci` → `id name Mango AC 1.1`, `id author Jose Andres Morales Linares`, `uciok`
- `isready` → `readyok`
- `ucinewgame` → `nuevo_juego()` (tablero inicial, hash, generador)
- `quit` → cierra recursos y `exit(0)`
- `setoption name MultiPV value N` (1–3). En análisis la GUI pide 3; en partida 1.
- `legal` → `legalmoves e2e4 …` y `legalok` (UCI largo).
- `facts d1f3 g1f3` (posición ya fijada con `position`) → líneas `fact …` y `factsok`. Tokens, no frases:
  - `fact move d1f3 piece Q color w from d1 to f3 capture -`
  - `fact attack d1f3 to d5 victim q ray f3 e4 d5 empty e4`
  - `fact attack g1f3 none`
  - `fact material w 32 b 34`
- `stop` durante `go`: el sondeo de reloj ve stdin y corta la búsqueda (`tiempoVencido`), luego `bestmove`.

**Posición** — `position startpos [moves …]` o `position fen <fen> [moves …]`

- `startpos` llama a `nuevo_juego()`.
- El FEN lo paso a `setupFen()` (casillas, color, enroques, al paso, regla de 50, número de jugada).
- Cada token de `moves` lo valido con `esUnMovimiento()` / `parse_mov()`, lo aplico con `hacerMovimiento()` y lo descarto si deja al propio rey en jaque.

**Búsqueda** — `go` con:

| Token | Uso |
|---|---|
| `depth` | Tope de profundidad (`juego.profundidadBusquedad`) |
| `movetime` | Tiempo fijo en ms |
| `wtime` / `btime` | Reloj del bando que mueve |
| `winc` / `binc` | Incremento Fischer |
| `movestogo` | Jugadas hasta el siguiente control |
| `infinite` | Tiempo efectivo ilimitado (`1<<25` ms) |
| `nodes`, `mate`, `searchmoves` | Los consumo y los ignoro |

La asignación de tiempo (`uciAsignarTiempo`) reparte el reloj: `remaining / movestogo + inc`, o `remaining / 25 + inc` si no hay control. Recorto para no gastar más de la mitad del restante y no bajo de 30 ms.

Al terminar, imprimo `bestmove <lan>` (o `bestmove 0000` si no hay jugada) y **aplico** esa jugada en el tablero interno, igual que hago en xboard con `move`.

`info score cp` / `score mate` es la valoración del **bando que mueve** (STM), no siempre POV blancas. La GUI de Mango AC lo convierte a POV blancas para el LLM. Otras GUI no deben asumir POV fijo.

`facts` lee el tablero **actual**. Tras un `go` el tablero ya tiene el `bestmove`; hay que volver a enviar `position` antes de `facts`.

**Salida durante la búsqueda** (`esUCI`)

En cada iteración emito una línea `info`, no el formato xboard `post`:

```
info depth D score cp N time T nodes N nps N pv e2e4 e7e5 …
info depth D score mate M …
```

Las valoraciones de mate internas (`VALOR_JAQUE_MATE` = 99000, centésimas de peón) las convierto a **mate en M jugadas** para UCI:

`(99000 − score) / 2 + 1` (y el simétrico para mate en contra).

Un movimiento de libro no imprime la línea xboard `0 0 0 0 Book move`; en UCI solo se ve el `bestmove` final.

### xboard / WinBoard (original, con identificación nueva)

El bucle de `xboard()` es el de 2012: turno de la máquina, ponderación opcional sobre la PV, y comandos CECP (`new`, `force`, `go`, `white`/`black`, `undo`/`remove`, `setboard`, `level`, `st`, `sd`, `time`, `otim`, `hint`, `post`/`nopost`, `ping`, `quit`, `?`).

En `protover` declaro:

```
feature setboard=1 analyze=0 ping=1 colors=0 draw=0 sigint=0 sigterm=0 usermove=0
         variants="normal" myname="Mango AC 1.1"
feature done=1
```

El único cambio de protocolo aquí es **`myname`**: ya no me identifico como Mango Paola, sino como **Mango AC**. El diálogo (`move e2e4`, `post` con `profundidad score tiempo nodos pv`) sigue siendo CECP.

La GUI envía jugadas en notación algebraica larga; yo las parseo, compruebo legalidad (rey en jaque) y, si me toca, busco y respondo `move`.

---

## Técnicas del motor (2012)

### Representación: bitboards de 64 bits

Cada tipo de pieza y cada bando es un `uint64`. Occupancy, destinos y ataques los combino con operaciones de bits (AND, XOR, shifts), no recorriendo a ciegas un array de 64 casillas.

Los deslizantes (torre, alfil, dama) usan **magic bitboards**: la occupancy de fila, columna o diagonal se multiplica por una constante mágica y se indexa una tabla precalculada (`FILEMAGIC`, `DIAGA8H1MAGIC`, `DIAGA1H8MAGIC`, desplazamiento 57). Peones, caballos y rey usan mapas fijos.

Empaqueto el movimiento en 28 bits: origen (6), destino (6), pieza (4), captura (4), promoción (4), código (4). El código distingue silencio, doble avance de peón, enroques, captura, *en passant* y las ocho promociones (con o sin captura).

### Generación de movimientos

`generarTodosMov()` produce pseudo-legales por bitboard. La legalidad (no dejar el rey en jaque) la filtro al hacer y deshacer. Con Perft (`perft.c`) cuento ese árbol para validar el generador.

### Búsqueda: Negamax alfa-beta con PVS

La rutina central es `alfabetaNegado()` (Negamax: el hijo se llama con `−beta, −alfa`).

- **Profundidad iterativa** desde 1 hasta `MAX_CAPAS_BUSQUEDAD` (24), con PV triangular.
- **Ventana de aspiración** de ±33 cp alrededor de la puntuación anterior. Si falla por arriba o por abajo, reabro con margen 66 y, si hace falta, con ventana infinita.
- **PVS (Principal Variation Search)**: el primer movimiento lo busco a ventana completa; el resto con ventana nula (`−alfa−1, −alfa`) y re-búsqueda si entra en la ventana.
- **Búsqueda de tranquilidad** (`busquedadTranquilidad`) en las hojas: solo capturas y jaques, para no evaluar en medio de un recambio.
- **Poda de distancia a mate**: acoto alfa/beta con `VALOR_JAQUE_MATE ± capa` para no explorar mates peores que uno ya conocido.
- **Único movimiento legal** en la raíz: lo devuelvo sin buscar.

### Podas y reducciones

- **Movimiento nulo** (`REDUCCION_MOV_NULL = 2`): si no hay jaque, no es nodo PV y hay material suficiente (`LIMITE_MOV_NULL`, un caballo), paso el turno y busco con reducción. Un fallo alto (`V ≥ beta`) corta. En profundidades 1–3 aplico *razoring* comparando la evaluación estática con márgenes fijos.
- **Reducción de movimiento tardío (LMR)**: a partir del cuarto candidato, en profundidad mayor que 2, si no es captura, jaque, promoción, hash move ni *killer*, reduzco 1 o 2 plies según la heurística de historia. Si mejora alfa, re-busco a profundidad completa.
- **Futility / late-move pruning**: omito candidatos “silenciosos” tardíos cuando la posición es podable.
- **Regla de 50 movimientos** y **repetición** de la clave hash: tablas o corte a empate (`reglaCincuentaMov > 99`, `ES_REPETICION_TABLERO`).
- **Extensiones** de un ply: jaque, amenaza de mate, recaptura de igual valor, peón a 7.ª (o 6.ª en final), peón pasado peligroso.

### Ordenación de movimientos

Sin un buen orden, alfa-beta no poda. `ponderarMovimientos()` prioriza:

1. Jugada de la tabla de transposición  
2. Jugada de la PV  
3. Promociones  
4. Capturas (MVV/LVA: valor capturado × 10 − valor que captura)  
5. Heurística de historia (casilla origen→destino, incrementada con `profundidad²`)  
6. Movimientos *killer* (dos por capa: `historicoMovMatadores`)

Con EET (`eet.c`) estimo el intercambio en una casilla (SEE) para no perseguir capturas perdedoras.

### Tablas hash (transposición)

Uso Zobrist: pieza×casilla, turno, enroques y *en passant*. La entrada guarda puntuación, profundidad, mejor movimiento y bandera (`EXACTO`, `ARRIBA`/`ABAJO` = cota, `EVITAR_NULL`). El tamaño se elige en `mangoac.ini` (`TamanioTablaHash` 1–9, de 18 a 26 bits; por defecto 22 bits ≈ 96 MB). Hay una tabla aparte de evaluaciones estáticas (`LARGO_HASH_EVAL`).

### Evaluación

`evaluacionTablero()` combina material (con fase de juego: dama 9, torre 5, ligera 3), estructura de peones (pasados, aislados, doblados, *outside passed pawn*), seguridad del rey (tropismo × vector de ataques) y tablas de pieza-casilla interpoladas por fase. Lazy eval: si el material solo ya queda fuera de `[alfa, beta]` ± `LIMITE_MOV_NULL`, no calculo el resto.

El libro de aperturas es binario (`libro2.c`, estilo Polyglot: random/main/tour en `mangoac.ini`). Las bitbases de finales (EGBB/Nalimov) son opcionales en compilación (`COMPILAR_CON_EGBB`); por defecto las dejo desactivadas.

---

## Compilar y ejecutar

Hace falta `gcc` (o MinGW en Windows). Una sola unidad de traducción: `bitmma3.c` incluye el resto de `.c`. La configuración está en `mangoac.ini` (hash, libro, rutas).

Linux / macOS:

```bash
gcc -lm bitmma3.c -Wall -O2 -o mangoac
./mangoac
```

Windows (MinGW):

```bash
gcc -lm bitmma3.c -Wall -O2 -o mangoac.exe
mangoac.exe
```

O `./compilar.sh` en Linux: compila y arranca si no hay errores.

Los binarios no van en este git. Cuando publique una versión, estarán en [Releases](https://github.com/mango-computer/mango-paola-xboard/releases).

Desde consola: `uci` o `xboard` para enganchar una GUI. Ejemplo UCI mínimo:

```
uci
isready
position startpos
go depth 8
```

---

## Apoya Mango AC

Si valoras este motor y quieres ayudar a que siga creciendo, puedes apoyar su
desarrollo. Las aportaciones ayudan a mantener las pruebas, preparar versiones
para distintas plataformas y continuar investigando la evolución del motor.

[Apoyar con 10 € en PayPal](https://paypal.me/JoseMoralesLinares/10)

La aportación es voluntaria. Mango AC seguirá siendo software libre. También
puedes ayudar comunicando errores, proponiendo mejoras, compartiendo el proyecto
o contribuyendo con código.

---

## Licencia

GPLv3: el texto íntegro está en [LICENSE](LICENSE). Ofrezco el código sin garantía. Investigación y autoría: Jose Andres Morales Linares, Mango Computer. Motor de 2012–2013; el protocolo, después.
