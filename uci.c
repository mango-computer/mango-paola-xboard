/* 
	Mango AC Ajedrez 1.0
	Licencia: GPLv3
	Copyright (c) 2012-2013 - Mango Computer c.a

	Protocolo UCI (Universal Chess Interface).
*/

#ifndef UCI_C
#define UCI_C

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
	printf("uciok\n");
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
	char linea[1024];
	char cmd[64];

	esPost = FALSO;
	esConsola = FALSO;
	esUCI = VERDADERO;
	setbuf(stdout, NULL);

	uciIdentificarse();

	while (fgets(linea, (int)sizeof(linea), stdin)) {
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
			continue;
		} else if (!strcmp(cmd, "stop")) {
			continue;
		} else if (!strcmp(cmd, "quit")) {
			CLOSE_BOOK();
#ifdef COMPILAR_CON_EGBB
			cerrarBitbases();
#endif
			exit(0);
		}
	}
}

#endif
