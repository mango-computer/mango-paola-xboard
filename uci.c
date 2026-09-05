/* 
	Mango AC Ajedrez 1.0
	Licencia: GPLv3
	Copyright (c) 2012-2013 - Mango Computer c.a

	Protocolo UCI (Universal Chess Interface).
*/

#ifndef UCI_C
#define UCI_C

#define UCI_LINE_INITIAL_CAPACITY 5120u
#define UCI_LINE_MAX_CAPACITY (256u * 1024u)

enum {
	UCI_LINE_EOF = 0,
	UCI_LINE_OK = 1,
	UCI_LINE_TOO_LONG = -1,
	UCI_LINE_NO_MEMORY = -2
};

static void uciDescartarRestoLinea(FILE *entrada)
{
	int c;

	while ((c = fgetc(entrada)) != '\n' && c != EOF)
		;
}

static int uciLeerLinea(FILE *entrada, char **linea, size_t *capacidad)
{
	size_t usados = 0;

	if (!*linea) {
		*linea = (char *)malloc(UCI_LINE_INITIAL_CAPACITY);
		if (!*linea)
			return UCI_LINE_NO_MEMORY;
		*capacidad = UCI_LINE_INITIAL_CAPACITY;
	}

	(*linea)[0] = '\0';
	for (;;) {
		size_t disponibles = *capacidad - usados;
		char *fragmento;
		size_t recibidos;

		fragmento = fgets(*linea + usados, (int)disponibles, entrada);
		if (!fragmento)
			return usados ? UCI_LINE_OK : UCI_LINE_EOF;

		recibidos = strlen(*linea + usados);
		usados += recibidos;
		if (usados && (*linea)[usados - 1] == '\n')
			return UCI_LINE_OK;
		if (feof(entrada))
			return UCI_LINE_OK;

		if (*capacidad >= UCI_LINE_MAX_CAPACITY) {
			uciDescartarRestoLinea(entrada);
			(*linea)[0] = '\0';
			return UCI_LINE_TOO_LONG;
		}

		{
			size_t nueva_capacidad = *capacidad * 2u;
			char *ampliada;

			if (nueva_capacidad > UCI_LINE_MAX_CAPACITY)
				nueva_capacidad = UCI_LINE_MAX_CAPACITY;
			ampliada = (char *)realloc(*linea, nueva_capacidad);
			if (!ampliada)
				return UCI_LINE_NO_MEMORY;
			*linea = ampliada;
			*capacidad = nueva_capacidad;
		}
	}
}

static char *uciSiguienteToken(char *p, char *tok, int toksize)
{
	int n = 0;

	while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
		p++;

	if (*p == '\0') {
		tok[0] = '\0';
		return p;
	}

	while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' && n < toksize - 1)
		tok[n++] = *p++;

	tok[n] = '\0';
	return p;
}

static void uciIdentificarse(void)
{
	printf("id name %s %s\n", NOMBRE_MANGO_AC, VERSION_MANGO_AC);
	printf("id author Jose Andres Morales Linares\n");
	printf("option name MultiPV type spin default 1 min 1 max 3\n");
	printf("uciok\n");
	fflush(stdout);
}

static void uciSetOption(char *linea)
{
	char tok[64];
	char *p = linea;
	int value;

	p = uciSiguienteToken(p, tok, (int)sizeof(tok));
	p = uciSiguienteToken(p, tok, (int)sizeof(tok));
	if (strcmp(tok, "name"))
		return;
	p = uciSiguienteToken(p, tok, (int)sizeof(tok));
	if (strcmp(tok, "MultiPV"))
		return;
	p = uciSiguienteToken(p, tok, (int)sizeof(tok));
	if (strcmp(tok, "value"))
		return;
	p = uciSiguienteToken(p, tok, (int)sizeof(tok));
	value = atoi(tok);
	if (value < 1)
		value = 1;
	if (value > 3)
		value = 3;
	uciMultiPV = value;
}

static void uciLegal(void)
{
	int i;
	BOOLEANO esJaque;
	BOOLEANO first = VERDADERO;

	juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
	printf("legalmoves");
	for (i = juego.Buffer_MOV_INDEXCAPAS[0]; i < juego.Buffer_MOV_INDEXCAPAS[1]; i++)
	{
		hacerMovimiento(juego.Buffer_MOV[i]);
		if (juego.colorTurno)
			esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
		else
			esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
		desHacerMovimiento(juego.Buffer_MOV[i]);
		if (esJaque)
			continue;
		printf(" ");
		imprimirMovimiento(juego.Buffer_MOV[i]);
		first = FALSO;
	}
	(void)first;
	printf("\nlegalok\n");
	fflush(stdout);
}

static char uciTipoLetra(int pieza)
{
	switch (pieza) {
	case PEON_BLANCO:
	case PEON_NEGRO:
		return 'P';
	case CABALLO_BLANCO:
	case CABALLO_NEGRO:
		return 'N';
	case ALFIL_BLANCO:
	case ALFIL_NEGRO:
		return 'B';
	case TORRE_BLANCO:
	case TORRE_NEGRO:
		return 'R';
	case DAMA_BLANCO:
	case DAMA_NEGRO:
		return 'Q';
	case REY_BLANCO:
	case REY_NEGRO:
		return 'K';
	default:
		return '?';
	}
}

static char uciVictimLetra(int pieza)
{
	char letra = uciTipoLetra(pieza);

	if (pieza >= PEON_NEGRO && letra >= 'A' && letra <= 'Z')
		letra = (char)(letra - 'A' + 'a');
	return letra;
}

static int uciPiezaBlanca(int pieza)
{
	return pieza >= PEON_BLANCO && pieza <= DAMA_BLANCO;
}

static uint64 uciAtaquesPieza(int sq, int pieza)
{
	switch (pieza) {
	case DAMA_BLANCO:
	case DAMA_NEGRO:
		return genTorreMOVAtaqueTablero(sq, juego) | genAlfilMOVAtaqueTablero(sq, juego);
	case TORRE_BLANCO:
	case TORRE_NEGRO:
		return genTorreMOVAtaqueTablero(sq, juego);
	case ALFIL_BLANCO:
	case ALFIL_NEGRO:
		return genAlfilMOVAtaqueTablero(sq, juego);
	case CABALLO_BLANCO:
	case CABALLO_NEGRO:
		return mascaraCaballo[sq];
	case REY_BLANCO:
	case REY_NEGRO:
		return mascaraRey[sq];
	case PEON_BLANCO:
		return mascaraCapturarPeon[sq][BLANCO];
	case PEON_NEGRO:
		return mascaraCapturarPeon[sq][NEGRO];
	default:
		return 0;
	}
}

static int uciEsDeslizante(int pieza)
{
	return pieza == DAMA_BLANCO || pieza == DAMA_NEGRO ||
	       pieza == TORRE_BLANCO || pieza == TORRE_NEGRO ||
	       pieza == ALFIL_BLANCO || pieza == ALFIL_NEGRO;
}

static void uciImprimirRayo(int from, int to)
{
	int df = (to % 8) - (from % 8);
	int dr = (to / 8) - (from / 8);
	int sf;
	int sr;
	int step;
	int sq;
	int empty = 0;

	if (from == to)
		return;
	if (df && dr && abs(df) != abs(dr))
		return;
	sf = (df > 0) - (df < 0);
	sr = (dr > 0) - (dr < 0);
	step = sf + sr * 8;
	if (!step)
		return;

	printf(" ray");
	for (sq = from; ; sq += step) {
		printf(" %s", NOMBRE_ESCAQUES[sq]);
		if (sq == to)
			break;
		if (sq < 0 || sq > 63)
			return;
	}
	printf(" empty");
	for (sq = from + step; sq != to && sq >= 0 && sq <= 63; sq += step) {
		if (ESCAQUES[sq] == VACIO) {
			printf(" %s", NOMBRE_ESCAQUES[sq]);
			empty = 1;
		}
	}
	if (!empty)
		printf(" -");
}

static int uciMaterialBando(COLOR lado)
{
	return cuentaBit(juego.tablero[lado][PEON]) * 1 +
	       cuentaBit(juego.tablero[lado][CABALLO]) * 3 +
	       cuentaBit(juego.tablero[lado][ALFIL]) * 3 +
	       cuentaBit(juego.tablero[lado][TORRE]) * 5 +
	       cuentaBit(juego.tablero[lado][DAMA]) * 9;
}

static void uciFactsUnMov(char *uciTok)
{
	MOVIMIENTO m;
	int origen;
	int destino;
	int pieza;
	int captura;
	int ocupante;
	BOOLEANO esJaque;
	uint64 ataques;
	uint64 enemy;
	int to;
	int printed = 0;

	if (!esUnMovimiento(uciTok))
		return;
	m = parse_mov(uciTok);
	if (m == (MOVIMIENTO)-1)
		return;

	origen = OBT_MOV_ORIGEN(m);
	destino = OBT_MOV_DESTINO(m);
	pieza = ESCAQUES[origen];
	captura = ESCAQUES[destino];
	if (captura == VACIO && ES_MOV_CAPTURA(m))
		captura = OBT_MOV_CAPTURA(m);

	hacerMovimiento(m);
	if (juego.colorTurno)
		esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
	else
		esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
	if (esJaque) {
		desHacerMovimiento(m);
		return;
	}

	printf("fact move %s piece %c color %c from %s to %s capture ",
	       uciTok,
	       uciTipoLetra(pieza),
	       uciPiezaBlanca(pieza) ? 'w' : 'b',
	       NOMBRE_ESCAQUES[origen],
	       NOMBRE_ESCAQUES[destino]);
	if (captura == VACIO)
		printf("-\n");
	else
		printf("%c\n", uciVictimLetra(captura));

	ocupante = ESCAQUES[destino];
	enemy = juego.colorTurno ? juego.negros : juego.blancos;
	ataques = uciAtaquesPieza(destino, ocupante) & enemy;
	if (!ataques)
		printf("fact attack %s none\n", uciTok);
	else {
		while (ataques) {
			to = bitScanForwardBruijn(ataques);
			printf("fact attack %s to %s victim %c",
			       uciTok,
			       NOMBRE_ESCAQUES[to],
			       uciVictimLetra(ESCAQUES[to]));
			if (uciEsDeslizante(ocupante))
				uciImprimirRayo(destino, to);
			printf("\n");
			ataques ^= BITSET[to];
			printed = 1;
		}
		(void)printed;
	}

	desHacerMovimiento(m);
}

static void uciFacts(char *linea)
{
	char tok[16];
	char *p = linea;

	juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
	p = uciSiguienteToken(p, tok, (int)sizeof(tok));
	while (1) {
		p = uciSiguienteToken(p, tok, (int)sizeof(tok));
		if (tok[0] == '\0')
			break;
		uciFactsUnMov(tok);
	}
	printf("fact material w %d b %d\n", uciMaterialBando(BLANCO), uciMaterialBando(NEGRO));
	printf("factsok\n");
	fflush(stdout);
}

static void uciPrintAlts(MOVIMIENTO best)
{
	int k;
	int rank = 2;

	if (uciMultiPV < 2)
		return;
	for (k = 0; k < uciAltCount && rank <= uciMultiPV; k++)
	{
		if (!uciAltMov[k] || uciAltMov[k] == best)
			continue;
		printf("info multipv %d depth 1 score cp %d pv ", rank, uciAltScore[k]);
		imprimirMovimiento(uciAltMov[k]);
		printf("\n");
		rank++;
	}
	fflush(stdout);
}

static void uciAplicarMovimientos(char *p)
{
	char mov[16];
	MOVIMIENTO m;
	BOOLEANO esJaque;

	while (1) {
		p = uciSiguienteToken(p, mov, (int)sizeof(mov));
		if (mov[0] == '\0')
			break;
		if (!esUnMovimiento(mov))
			continue;

		m = parse_mov(mov);
		if (m == (MOVIMIENTO)-1)
			continue;

		hacerMovimiento(m);
		if (juego.colorTurno)
			esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
		else
			esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);

		if (esJaque) {
			desHacerMovimiento(m);
			continue;
		}
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
	}
}

static void uciPosition(char *linea)
{
	char tok[256];
	char fen[128];
	char color[8];
	char enroque[16];
	char ep[8];
	int hm = 0;
	int fm = 1;
	char *p = linea;

	p = uciSiguienteToken(p, tok, (int)sizeof(tok));
	p = uciSiguienteToken(p, tok, (int)sizeof(tok));

	if (!strcmp(tok, "startpos")) {
		nuevo_juego();
		p = uciSiguienteToken(p, tok, (int)sizeof(tok));
		if (!strcmp(tok, "moves"))
			uciAplicarMovimientos(p);
		return;
	}

	if (strcmp(tok, "fen"))
		return;

	p = uciSiguienteToken(p, fen, (int)sizeof(fen));
	p = uciSiguienteToken(p, color, (int)sizeof(color));
	p = uciSiguienteToken(p, enroque, (int)sizeof(enroque));
	p = uciSiguienteToken(p, ep, (int)sizeof(ep));
	p = uciSiguienteToken(p, tok, (int)sizeof(tok));

	if (!strcmp(tok, "moves")) {
		setupFen(fen, color, enroque, ep, 0, 1);
		uciAplicarMovimientos(p);
		return;
	}

	if (tok[0])
		hm = atoi(tok);

	p = uciSiguienteToken(p, tok, (int)sizeof(tok));
	if (!strcmp(tok, "moves")) {
		setupFen(fen, color, enroque, ep, hm, 1);
		uciAplicarMovimientos(p);
		return;
	}

	if (tok[0])
		fm = atoi(tok);

	setupFen(fen, color, enroque, ep, hm, fm);
	p = uciSiguienteToken(p, tok, (int)sizeof(tok));
	if (!strcmp(tok, "moves"))
		uciAplicarMovimientos(p);
}

static int uciAsignarTiempo(int remaining, int inc, int movestogo)
{
	int alloc;

	if (remaining < 0)
		return -1;

	if (movestogo > 0)
		alloc = remaining / movestogo + inc;
	else
		alloc = remaining / 25 + inc;

	if (alloc > remaining - 50)
		alloc = remaining / 2;
	if (alloc < 30)
		alloc = 30;
	if (alloc > remaining)
		alloc = remaining;
	return alloc;
}

static void uciGo(char *linea)
{
	char tok[64];
	char *p = linea;
	int depth = 0;
	int movetime = -1;
	int wtime = -1;
	int btime = -1;
	int winc = 0;
	int binc = 0;
	int movestogo = 0;
	BOOLEANO infinite = FALSO;
	MOVIMIENTO m;
	int allocated;

	p = uciSiguienteToken(p, tok, (int)sizeof(tok));

	while (1) {
		p = uciSiguienteToken(p, tok, (int)sizeof(tok));
		if (tok[0] == '\0')
			break;

		if (!strcmp(tok, "depth")) {
			p = uciSiguienteToken(p, tok, (int)sizeof(tok));
			depth = atoi(tok);
		} else if (!strcmp(tok, "movetime")) {
			p = uciSiguienteToken(p, tok, (int)sizeof(tok));
			movetime = atoi(tok);
		} else if (!strcmp(tok, "wtime")) {
			p = uciSiguienteToken(p, tok, (int)sizeof(tok));
			wtime = atoi(tok);
		} else if (!strcmp(tok, "btime")) {
			p = uciSiguienteToken(p, tok, (int)sizeof(tok));
			btime = atoi(tok);
		} else if (!strcmp(tok, "winc")) {
			p = uciSiguienteToken(p, tok, (int)sizeof(tok));
			winc = atoi(tok);
		} else if (!strcmp(tok, "binc")) {
			p = uciSiguienteToken(p, tok, (int)sizeof(tok));
			binc = atoi(tok);
		} else if (!strcmp(tok, "movestogo")) {
			p = uciSiguienteToken(p, tok, (int)sizeof(tok));
			movestogo = atoi(tok);
		} else if (!strcmp(tok, "infinite")) {
			infinite = VERDADERO;
		} else if (!strcmp(tok, "nodes") || !strcmp(tok, "mate") ||
			   !strcmp(tok, "searchmoves")) {
			p = uciSiguienteToken(p, tok, (int)sizeof(tok));
		}
	}

	esPost = FALSO;
	esConsola = FALSO;
	esUCI = VERDADERO;
	tipoDeBusqueda = TIPO_BUSQUEDA_NORMAL;
	uciAltCount = 0;

	juego.profundidadBusquedad = MAX_CAPAS;
	juego.maxTiempo = 2000;

	if (infinite) {
		juego.maxTiempo = 1 << 25;
		juego.profundidadBusquedad = MAX_CAPAS;
	} else if (movetime >= 0) {
		juego.maxTiempo = movetime;
		if (depth > 0)
			juego.profundidadBusquedad = depth;
	} else if (wtime >= 0 || btime >= 0) {
		if (juego.colorTurno == BLANCO)
			allocated = uciAsignarTiempo(wtime < 0 ? 0 : wtime, winc, movestogo);
		else
			allocated = uciAsignarTiempo(btime < 0 ? 0 : btime, binc, movestogo);
		if (allocated > 0)
			juego.maxTiempo = allocated;
		if (depth > 0)
			juego.profundidadBusquedad = depth;
	} else if (depth > 0) {
		juego.profundidadBusquedad = depth;
		juego.maxTiempo = 1 << 25;
	}

	m = pensarRapido();

	if (m && m != (MOVIMIENTO)-1) {
		uciPrintAlts(m);
		printf("bestmove ");
		imprimirMovimiento(m);
		printf("\n");
		hacerMovimiento(m);
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
	} else {
		printf("bestmove 0000\n");
	}
	fflush(stdout);
}

void uci(void)
{
	char *linea = NULL;
	size_t capacidad_linea = 0;
	char cmd[64];
	int estado_linea;

	esPost = FALSO;
	esConsola = FALSO;
	esUCI = VERDADERO;
	setbuf(stdout, NULL);

	uciIdentificarse();

	for (;;) {
		estado_linea = uciLeerLinea(stdin, &linea, &capacidad_linea);
		if (estado_linea == UCI_LINE_EOF)
			break;
		if (estado_linea == UCI_LINE_TOO_LONG) {
			printf("info string error: UCI command too long\n");
			continue;
		}
		if (estado_linea == UCI_LINE_NO_MEMORY) {
			printf("info string error: unable to allocate UCI command buffer\n");
			break;
		}

		uciSiguienteToken(linea, cmd, (int)sizeof(cmd));
		if (cmd[0] == '\0')
			continue;

		if (!strcmp(cmd, "uci")) {
			uciIdentificarse();
		} else if (!strcmp(cmd, "isready")) {
			printf("readyok\n");
			fflush(stdout);
		} else if (!strcmp(cmd, "ucinewgame")) {
			nuevo_juego();
		} else if (!strcmp(cmd, "position")) {
			uciPosition(linea);
		} else if (!strcmp(cmd, "go")) {
			uciGo(linea);
		} else if (!strcmp(cmd, "setoption")) {
			uciSetOption(linea);
		} else if (!strcmp(cmd, "legal")) {
			uciLegal();
		} else if (!strcmp(cmd, "facts")) {
			uciFacts(linea);
		} else if (!strcmp(cmd, "stop")) {
			tiempoVencido = VERDADERO;
		} else if (!strcmp(cmd, "quit")) {
			CLOSE_BOOK();
#ifdef COMPILAR_CON_EGBB
			cerrarBitbases();
#endif
			free(linea);
			exit(0);
		}
	}
	free(linea);
}

#endif
