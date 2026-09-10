/* 
	Mango AC Ajedrez 1.0
	Licencia: GPLv3
	Copyright (c) 2012-2013 - Mango Computer c.a

	El programa Mango AC Ajedrez está basado en la investigación de Jose Andres Morales Linares.

	Nadie debería verse restringido por el software que utiliza. Hay cuatro libertades que cada usuario debe tener:
	* La libertad de usar el programa para cualquier propósito.
	* La libertad de cambiar el software para satisfacer sus necesidades.
	* La libertad de compartir el software con amigos y vecinos.
	* La libertad de compartir los cambios que realice.

	Cuando un programa ofrece a los usuarios todas estas libertades, lo llamamos software libre.

	Winglet es un motor de ajedrez de código abierto basado en bitboards. El programa es software libre. Usted puede redistribuirlo y/o modificarlo bajo los términos de la Licencia Pública General de GNU publicada por la Free Software Foundation, ya sea la versión 3 de la Licencia o (a su elección) cualquier versión posterior. El programa se distribuye con la esperanza de que sea útil, pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de COMERCIALIZACIÓN o IDONEIDAD PARA UN PROPÓSITO PARTICULAR. Consulte la Licencia Pública General de GNU para más detalles: http://www.gnu.org/licenses/

	Existen muchos avances en la forma de escribir motores de ajedrez. Estos dos sitios web
	fueron de gran ayuda durante el proceso de investigación y desarrollo:
	* http://chessprogramming.wikispaces.com
	* http://www.sluijten.com/winglet/

	Información de contacto:
	comprasmangocomputer@gmail.com

*/

#ifndef COMANDOS_C
#define COMANDOS_C

static void mostrarPrompt(void)
{
	if (juego.colorTurno)
		printf("mangoac [negras]> ");
	else
		printf("mangoac [blancas]> ");
	fflush(stdout);
}

void leerComandos()
{
	int c=0;
	setbuf(stdout, NULL);

	mostrarPrompt();

	while ( (c=getc(stdin)) != EOF  )
	{
		if (c=='\n')
		{
			BUFF_COMANDOS[CONT_BUFF_COMANDOS] = '\0';

			while (CONT_BUFF_COMANDOS)
			{
				if (!ejecutarComando(BUFF_COMANDOS)) return;
			}

			mostrarPrompt();

		} else {
			if (CONT_BUFF_COMANDOS>=LARGO_BUFER_COMANDOS)
			{
				printf("El comando es demasiado largo\n");
				CONT_BUFF_COMANDOS = 0;
			}
			
			BUFF_COMANDOS[CONT_BUFF_COMANDOS++] = c;
		}

	}

}

static void mostrarAyuda(void)
{
	printf("\n");
	printf("  Mango AC Ajedrez — comandos de consola\n");
	printf("  Escriba el comando y pulse <Enter>. Los alias van entre paréntesis.\n");
	printf("  English help: type help\n");
	printf("\n");
	printf("  Partida\n");
	printf("    %-32s %s\n", "nuevo",                    "Inicia una nueva partida");
	printf("    %-32s %s\n", "tablero (d, t)",           "Muestra el tablero");
	printf("    %-32s %s\n", "girar (g, r)",             "Gira el tablero");
	printf("    %-32s %s\n", "deshacer (u, regresar)",   "Deshace el último movimiento");
	printf("    %-32s %s\n", "partida (juego)",          "Muestra las jugadas de la partida");
	printf("    %-32s %s\n", "blancas (blanco)",         "Turno de las blancas");
	printf("    %-32s %s\n", "negras (negro)",           "Turno de las negras");
	printf("    %-32s %s\n", "move e2e4",                "Juega un movimiento (origen-destino)");
	printf("\n");
	printf("  Motor\n");
	printf("    %-32s %s\n", "jugar (dale)",             "El motor realiza su jugada");
	printf("    %-32s %s\n", "auto (cc)",                "Partida de motor contra motor");
	printf("    %-32s %s\n", "facil / medio / fuerte",   "Nivel predefinido del motor");
	printf("    %-32s %s\n", "profundidad n",            "Profundidad máxima de búsqueda");
	printf("    %-32s %s\n", "tiempo n",                 "Tiempo por movimiento, en segundos");
	printf("    %-32s %s\n", "libro si (librosi)",       "Activa el libro de aperturas");
	printf("    %-32s %s\n", "libro no (librono)",       "Desactiva el libro de aperturas");
	printf("\n");
	printf("  Análisis\n");
	printf("    %-32s %s\n", "legales (movimientos)",    "Muestra los movimientos legales");
	printf("    %-32s %s\n", "eval",                     "Evaluación estática de la posición");
	printf("    %-32s %s\n", "fen",                      "Muestra la posición en notación FEN");
	printf("    %-32s %s\n", "leerfen archivo n",        "Carga la posición n de un archivo FEN");
	printf("    %-32s %s\n", "perft n",                  "Cuenta nodos a profundidad n (prueba)");
	printf("    %-32s %s\n", "info (i)",                 "Muestra las variables internas");
	printf("\n");
	printf("  Sistema\n");
	printf("    %-32s %s\n", "version (ver)",            "Muestra la versión y el autor");
	printf("    %-32s %s\n", "xboard",                   "Activa el protocolo WinBoard/xboard");
	printf("    %-32s %s\n", "uci",                      "Activa el protocolo UCI");
	printf("    %-32s %s\n", "salir",                    "Sale del programa");
	printf("    %-32s %s\n", "ayuda (?, h)",             "Muestra esta ayuda");
	printf("\n");
	printf("  Depuración\n");
	printf("    %-32s %s\n", "ver ocupados",             "Mapa de bits de las casillas ocupadas");
	printf("    %-32s %s\n", "ver blancos",              "Mapa de bits de las piezas blancas");
	printf("    %-32s %s\n", "ver negros",               "Mapa de bits de las piezas negras");
	printf("    %-32s %s\n", "ver peones blancos",       "Mapa de bits de los peones blancos");
	printf("    %-32s %s\n", "ver peones negros",        "Mapa de bits de los peones negros");
	printf("    %-32s %s\n", "ver destinos blancos",     "Mapa de bits de destinos blancos");
	printf("    %-32s %s\n", "ver destinos negros",      "Mapa de bits de destinos negros");
	printf("\n");
}

static void mostrarHelp(void)
{
	printf("\n");
	printf("  Mango AC Chess — console commands\n");
	printf("  Type a command and press <Enter>. Aliases are shown in parentheses.\n");
	printf("  Ayuda en español: escriba ayuda\n");
	printf("\n");
	printf("  Game\n");
	printf("    %-32s %s\n", "new",                      "Start a new game");
	printf("    %-32s %s\n", "board (d, t)",             "Display the board");
	printf("    %-32s %s\n", "flip (g, r)",              "Flip the board");
	printf("    %-32s %s\n", "undo (u)",                 "Take back the last move");
	printf("    %-32s %s\n", "game",                     "Show the moves of the game");
	printf("    %-32s %s\n", "white",                    "White to move");
	printf("    %-32s %s\n", "black",                    "Black to move");
	printf("    %-32s %s\n", "move e2e4",                "Play a move (from-to notation)");
	printf("\n");
	printf("  Engine\n");
	printf("    %-32s %s\n", "go",                       "Let the engine make a move");
	printf("    %-32s %s\n", "auto (cc)",                "Engine vs engine game");
	printf("    %-32s %s\n", "sd n",                     "Set maximum search depth");
	printf("    %-32s %s\n", "time n",                   "Set time per move, in seconds");
	printf("    %-32s %s\n", "book on",                  "Enable the opening book");
	printf("    %-32s %s\n", "book off",                 "Disable the opening book");
	printf("\n");
	printf("  Analysis\n");
	printf("    %-32s %s\n", "moves",                    "Show legal moves");
	printf("    %-32s %s\n", "eval",                     "Static evaluation of the position");
	printf("    %-32s %s\n", "fen",                      "Show the position in FEN notation");
	printf("    %-32s %s\n", "readfen file n",           "Load position n from a FEN file");
	printf("    %-32s %s\n", "perft n",                  "Count nodes at depth n (test)");
	printf("    %-32s %s\n", "info (i)",                 "Show internal engine variables");
	printf("\n");
	printf("  System\n");
	printf("    %-32s %s\n", "version",                  "Show version and author");
	printf("    %-32s %s\n", "xboard",                   "Switch to WinBoard/xboard protocol");
	printf("    %-32s %s\n", "uci",                      "Switch to UCI protocol");
	printf("    %-32s %s\n", "quit (exit)",              "Leave the program");
	printf("    %-32s %s\n", "help (h)",                 "Show this help");
	printf("\n");
	printf("  Debug\n");
	printf("    %-32s %s\n", "ver ocupados",             "Bitboard of occupied squares");
	printf("    %-32s %s\n", "ver blancos",              "Bitboard of white pieces");
	printf("    %-32s %s\n", "ver negros",               "Bitboard of black pieces");
	printf("    %-32s %s\n", "ver peones blancos",       "Bitboard of white pawns");
	printf("    %-32s %s\n", "ver peones negros",        "Bitboard of black pawns");
	printf("    %-32s %s\n", "ver destinos blancos",     "Bitboard of white destinations");
	printf("    %-32s %s\n", "ver destinos negros",      "Bitboard of black destinations");
	printf("\n");
}

BOOLEANO ejecutarComando(const char *buff)
{
	BOOLEANO esJaque;
	uint64 tiempo_ms  = 0;  
	MOVIMIENTO m;
	int profundidadSugerida = 2;

	if (!strcmp(buff, ""))
	{
		CONT_BUFF_COMANDOS = '\0';
		return VERDADERO;    
	}

	if ((!strcmp(buff, "ayuda")) || (!strcmp(buff, "?")))
	{
		mostrarAyuda();
		CONT_BUFF_COMANDOS = '\0';
		return VERDADERO;    
	}

	if ((!strcmp(buff, "help")) || (!strcmp(buff, "h")))
	{
		mostrarHelp();
		CONT_BUFF_COMANDOS = '\0';
		return VERDADERO;    
	}

	if (buff[0] == 'p' && buff[1] == 'e' && buff[2] == 'r' && buff[3] == 'f' && buff[4] == 't' && isdigit(buff[6]))  
	{
		uint64 resulPerft = 0;
		tiempo_ms  = 0;  
		uint8  profundidad = buff[6] - 48;
		printf("Iniciando la prueba a profundidad %d\n", profundidad);
		printf("...\n");

		DataPerft.InvCaptura		= 0;
		DataPerft.InvPeonPaso		= 0;
		DataPerft.InvPromocion		= 0;	
		DataPerft.InvEnroqueOO		= 0;
		DataPerft.InvEnroqueOOO		= 0;
		DataPerft.InvJaqueContrario	= 0;

		prepararTiempo();
		iniciarTiempo();
		resulPerft = perft(0,profundidad);
		detenerTiempo();
		tiempo_ms  = obt_msTiempo();
		printf("Nodos %lld, en %lld ms\n",resulPerft,tiempo_ms);

		printf("\n");
		printf("Capturas	: %u\n",DataPerft.InvCaptura);
		printf("Peón al paso	: %u\n",DataPerft.InvPeonPaso);
		printf("Promoción	: %u\n",DataPerft.InvPromocion);
		printf("OO		: %u\n",DataPerft.InvEnroqueOO);
		printf("OOO		: %u\n",DataPerft.InvEnroqueOOO);
		printf("Total Enroque	: %u\n",(DataPerft.InvEnroqueOOO+DataPerft.InvEnroqueOO));
		printf("Jaque enemigo	: %u\n",DataPerft.InvJaqueContrario);
		printf("\n");

		if (tiempo_ms > 0)
		{
			printf("Relación: %lld knodos/s\n", resulPerft/tiempo_ms);
		}
		CONT_BUFF_COMANDOS = '\0';
		return VERDADERO;    
	}




	if (buff[0] == 'm' && buff[1] == 'o' && buff[2] == 'v' && buff[3] == 'e' && buff[4] == ' ')  
	{
		CONT_BUFF_COMANDOS = '\0';
		MOVIMIENTO mov;
		mov ^= mov;
		if (esValidoMovUsuario(buff, &mov))
		{
			hacerMovimiento(mov);

			if (juego.colorTurno) // Si le toca al negro, se comprueba que el rey blanco no haya quedado en jaque
			{
				esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
			} else {
				esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
			}
			
			if (!esJaque)
			{
				juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
				mostrarTablero();
			} else {
				printf("Movimiento ilegal: el rey queda en jaque\n");
				desHacerMovimiento(mov);
			}
		} else {
			printf("Movimiento ilegal: escaque de origen o destino inválido\n");
		}

		return VERDADERO;    
	}

	if ((!strncmp(buff, "profundidad ", 12)) ||
	    (!strncmp(buff, "sd", 2) && (buff[2] == '\0' || buff[2] == ' ')))
	{
		CONT_BUFF_COMANDOS = '\0';
		if (!strncmp(buff, "profundidad ", 12))
			sscanf(buff, "profundidad %d", &profundidadSugerida);
		else
			sscanf(buff, "sd %d", &profundidadSugerida);
		if (profundidadSugerida < 2) profundidadSugerida = 2;
		juego.profundidadBusquedad = profundidadSugerida;
		nivelPC = PC_NIVEL_PERSONALIZADO;
		return VERDADERO;    
	}

	if (buff[0] == 'l' && buff[1] == 'e' && buff[2] == 'e' && buff[3] == 'r' && buff[4] == 'f' && buff[5] == 'e' && buff[6] == 'n')
	{
		CONT_BUFF_COMANDOS = '\0';
		char nombreArchivo[80];
		int numero=0;
		sscanf(buff,"leerfen %s %d", nombreArchivo, &numero);
		readFen(nombreArchivo, numero);
		mostrarTablero();
		return VERDADERO;    
	}


	if (buff[0] == 'r' && buff[1] == 'e' && buff[2] == 'a' && buff[3] == 'd' && buff[4] == 'f' && buff[5] == 'e' && buff[6] == 'n')
	{
		CONT_BUFF_COMANDOS = '\0';
		char nombreArchivo[80];
		int numero=0;
		sscanf(buff,"readfen %s %d", nombreArchivo, &numero);
		readFen(nombreArchivo, numero);
		mostrarTablero();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "undo")) || (!strcmp(buff, "regresar")) || (!strcmp(buff, "u")) ||
	    (!strcmp(buff, "deshacer")))
	{
		CONT_BUFF_COMANDOS = '\0';
		if (juego.indiceHJuego > 0)
		{
			desHacerMovimiento(juego.historicoJuego[juego.indiceHJuego-1].mov);
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			mostrarTablero();
		}
		return VERDADERO;    
	}

	if ((!strcmp(buff, "eval")))
	{
		CONT_BUFF_COMANDOS = '\0';
		int resulEval = evaluacionTablero(-INFINITO,INFINITO);
		printf("Evaluación estática del tablero actual\nEval:	%d\n",resulEval);
		return VERDADERO;    
	}

	if ((!strncmp(buff, "evalwindow ", 11)))
	{
		int alfa = -INFINITO;
		int beta = INFINITO;
		CONT_BUFF_COMANDOS = '\0';
		sscanf(buff, "evalwindow %d %d", &alfa, &beta);
		printf("EvalWindow: %d\n", evaluacionTablero(alfa, beta));
		return VERDADERO;
	}

	if ((!strcmp(buff, "evalstats")))
	{
		double tasa = consultasHashPeones ?
			(100.0 * (double)aciertosHashPeones / (double)consultasHashPeones) : 0.0;
		CONT_BUFF_COMANDOS = '\0';
		printf("EvalStats: phase=%d pawn_hits=%llu pawn_queries=%llu pawn_rate=%.2f\n",
		       FASE,
		       (unsigned long long)aciertosHashPeones,
		       (unsigned long long)consultasHashPeones,
		       tasa);
		return VERDADERO;
	}
#ifdef PRUEBAS_HCE
	if ((!strcmp(buff, "eetstats")))
	{
		CONT_BUFF_COMANDOS = '\0';
		printf("SEEStats: full_calls=%llu threshold_calls=%llu full_branches=%llu "
		       "threshold_branches=%llu threshold_cuts=%llu eetpos_calls=%llu max_depth=%d\n",
		       (unsigned long long)llamadasEETCompleto,
		       (unsigned long long)llamadasEETUmbral,
		       (unsigned long long)ramasEETCompleto,
		       (unsigned long long)ramasEETUmbral,
		       (unsigned long long)cortesEETUmbral,
		       (unsigned long long)llamadasEETPOS,
		       profundidadMaxEET);
		return VERDADERO;
	}

	if ((!strcmp(buff, "eetstatsreset")))
	{
		CONT_BUFF_COMANDOS = '\0';
		reiniciarEstadisticasEET();
		printf("SEEStatsReset: ok\n");
		return VERDADERO;
	}

	if ((!strncmp(buff, "eetbench ", 9)))
	{
		MOVIMIENTO movimientos[256];
		uint32 fin;
		int cantidad;
		int repeticiones = 1;
		int umbral = 0;
		int usarUmbral = 1;
		int i, repeticion;
		uint64 checksum = 0;

		CONT_BUFF_COMANDOS = '\0';
		sscanf(buff, "eetbench %d %d %d", &repeticiones, &umbral, &usarUmbral);
		if (repeticiones < 1) repeticiones = 1;
		fin = generarMovCapPro(0);
		cantidad = (int)MINIMO(fin, 256);
		for (i = 0; i < cantidad; i++) movimientos[i] = juego.Buffer_MOV[i];

		reiniciarEstadisticasEET();
		for (repeticion = 0; repeticion < repeticiones; repeticion++)
		{
			for (i = 0; i < cantidad; i++)
			{
				if (usarUmbral)
					checksum += EETSuperaUmbral(movimientos[i], umbral);
				else
					checksum += EET(movimientos[i]) >= umbral;
			}
		}
		printf("SEEBench: moves=%d repetitions=%d threshold=%d mode=%d checksum=%llu "
		       "full_calls=%llu threshold_calls=%llu full_branches=%llu "
		       "threshold_branches=%llu threshold_cuts=%llu max_depth=%d\n",
		       cantidad, repeticiones, umbral, usarUmbral,
		       (unsigned long long)checksum,
		       (unsigned long long)llamadasEETCompleto,
		       (unsigned long long)llamadasEETUmbral,
		       (unsigned long long)ramasEETCompleto,
		       (unsigned long long)ramasEETUmbral,
		       (unsigned long long)cortesEETUmbral,
		       profundidadMaxEET);
		return VERDADERO;
	}

	if ((!strcmp(buff, "eetall")))
	{
		MOVIMIENTO movimientos[256];
		uint32 fin = generarMovCapPro(0);
		int cantidad = (int)MINIMO(fin, 256);
		int i;

		CONT_BUFF_COMANDOS = '\0';
		for (i = 0; i < cantidad; i++) movimientos[i] = juego.Buffer_MOV[i];
		for (i = 0; i < cantidad; i++)
		{
			MOVIMIENTO mov = movimientos[i];
			BOOLEANO ilegal;
			int exacto;
			int referencia;

			hacerMovimiento(mov);
			if (juego.colorTurno)
				ilegal = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
			else
				ilegal = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
			desHacerMovimiento(mov);
			if (ilegal) continue;

			exacto = EET(mov);
			referencia = EETReferencia(mov);
			printf("SEECompare: origin=%d destination=%d promotion=%d exact=%d reference=%d "
			       "geNeg1000=%d geNeg500=%d geNeg100=%d geNeg15=%d "
			       "ge0=%d ge1=%d ge100=%d ge500=%d ge1000=%d\n",
			       OBT_MOV_ORIGEN(mov), OBT_MOV_DESTINO(mov),
			       OBT_MOV_PROMOCION(mov), exacto, referencia,
			       EETSuperaUmbral(mov, -1000), EETSuperaUmbral(mov, -500),
			       EETSuperaUmbral(mov, -100),
			       EETSuperaUmbral(mov, -15), EETSuperaUmbral(mov, 0),
			       EETSuperaUmbral(mov, 1), EETSuperaUmbral(mov, 100),
			       EETSuperaUmbral(mov, 500), EETSuperaUmbral(mov, 1000));
		}
		return VERDADERO;
	}

	if ((!strncmp(buff, "searchprobe ", 12)))
	{
		int profundidad = 0;
		int alfa = -INFINITO;
		int beta = INFINITO;
		int valor;

		CONT_BUFF_COMANDOS = '\0';
		sscanf(buff, "searchprobe %d %d %d", &profundidad, &alfa, &beta);
		limpiarAntesDeBusqueda();
		tiempoVencido = FALSO;
		contadorDescendente = INFINITO;
		juego.Buffer_MOV_INDEXCAPAS[0] = 0;

		if (profundidad > 0)
		{
			valor = alfabetaNegado(0, profundidad, alfa, beta, VERDADERO);
		} else {
			valor = busquedadTranquilidad(0, alfa, beta);
		}

		printf("SearchProbe: score=%d nodes=%d qnodes=%d eet_prunes=%d eet_extensions=%d\n",
		       valor, contadorNodos, QcontadorNodos,
		       contadorPodasEET, contadorExtensionesEET);
		return VERDADERO;
	}

	if ((!strncmp(buff, "evalprobe", 9)))
	{
		int origen = -1;
		int tipo = -1;
		int color = BLANCO;
		uint64 pseudo = 0;
		uint64 efectivos = 0;
		uint64 utiles = 0;
		int resulEval;

		CONT_BUFF_COMANDOS = '\0';
		sscanf(buff, "evalprobe %d %d %d", &origen, &tipo, &color);
		resulEval = evaluacionTablero(-INFINITO, INFINITO);
		if (origen >= 0 && origen < 64 && color >= BLANCO && color <= NEGRO)
		{
			switch (tipo)
			{
				case PEON:
					pseudo = mascaraCapturarPeon[origen][color];
					break;
				case CABALLO:
					pseudo = genCaballoAtaqueTablero(origen, juego);
					break;
				case ALFIL:
					pseudo = genAlfilAtaqueTablero(origen, juego);
					break;
				case TORRE:
					pseudo = genTorreAtaqueTablero(origen, juego);
					break;
				case DAMA:
					pseudo = genDamaAtaqueTablero(origen, juego);
					break;
			}
			efectivos = ataquesEfectivosEval(color, origen, pseudo);
			utiles = areaMovilidadUtil(color, origen, efectivos);
		}
		printf("EvalProbe: eval=%d weakW=%llu weakB=%llu passedW=%llu passedB=%llu "
		       "pawnW=%llu pawnB=%llu attacksW=%llu attacksB=%llu "
		       "pseudoW=%llu pseudoB=%llu pinsW=%llu pinsB=%llu ray=%llu "
		       "piecePseudo=%llu pieceEffective=%llu pieceUseful=%llu "
		       "threatW=%d threatB=%d oppositionW=%d oppositionB=%d\n",
		       resulEval,
		       (unsigned long long)peonesDebiles[BLANCO],
		       (unsigned long long)peonesDebiles[NEGRO],
		       (unsigned long long)peonesPasados[BLANCO],
		       (unsigned long long)peonesPasados[NEGRO],
		       (unsigned long long)mapaPosAtacadasXPza[BLANCO][PEON],
		       (unsigned long long)mapaPosAtacadasXPza[NEGRO][PEON],
		       (unsigned long long)mapaPosAtacadas[BLANCO],
		       (unsigned long long)mapaPosAtacadas[NEGRO],
		       (unsigned long long)mapaPosAtacadasPseudo[BLANCO],
		       (unsigned long long)mapaPosAtacadasPseudo[NEGRO],
		       (unsigned long long)mapaClavadasRey[BLANCO],
		       (unsigned long long)mapaClavadasRey[NEGRO],
		       (unsigned long long)((origen >= 0 && origen < 64) ? mapaRayosClavada[origen] : 0),
		       (unsigned long long)pseudo,
		       (unsigned long long)efectivos,
		       (unsigned long long)utiles,
		       puntajeAmenazas(BLANCO),
		       puntajeAmenazas(NEGRO),
		       evalOposicionReyes(BLANCO),
		       evalOposicionReyes(NEGRO));
		return VERDADERO;
	}
#endif
#ifdef COMPILAR_CON_EGBB
	if ((!strcmp(buff, "nalimov")))
	{
		CONT_BUFF_COMANDOS = '\0';
		if (usar_egbb && egbb_is_loaded)
		{
			int totalPiezas = 
			cuentaBit(juego.tablero[BLANCO][PEON]) + cuentaBit(juego.tablero[NEGRO][PEON]) +
			cuentaBit(juego.tablero[BLANCO][DAMA]) + cuentaBit(juego.tablero[NEGRO][DAMA]) + 
			cuentaBit(juego.tablero[BLANCO][CABALLO]) + cuentaBit(juego.tablero[NEGRO][CABALLO]) + 
			cuentaBit(juego.tablero[BLANCO][TORRE]) + cuentaBit(juego.tablero[NEGRO][TORRE])   + 
			cuentaBit(juego.tablero[BLANCO][ALFIL])   + cuentaBit(juego.tablero[NEGRO][ALFIL]) + 2;

			if (totalPiezas <= nLimitePiezasBitBases)
			{
				int V_egbb=0;
				if (probe_bitbases(&V_egbb)) 
				{
					printf("V_egbb: %d\n",V_egbb);
				}
			}
		}

		return VERDADERO;    
	}
#endif
	if ((!strcmp(buff, "facil")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.maxTiempo 		= 3000;
		juego.profundidadBusquedad	= 98;
		nivelPC = PC_NIVEL_FACIL;
		return VERDADERO;    
	}

	if ((!strcmp(buff, "medio")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.maxTiempo 		= 7000;
		juego.profundidadBusquedad	= 98;
		nivelPC = PC_NIVEL_MEDIO;
		return VERDADERO;    
	}

	if ((!strcmp(buff, "fuerte")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.maxTiempo 		= 12000;
		juego.profundidadBusquedad	= 98;
		nivelPC = PC_NIVEL_FUERTE;
		return VERDADERO;    
	}

	if ((!strcmp(buff, "fen")))
	{
		CONT_BUFF_COMANDOS = '\0';
		char fen[128];
		if (converTabler2FEN(fen, sizeof(fen)))
			printf("FEN: %s\n",fen);
		return VERDADERO;    
	}


	if ((!strncmp(buff, "tiempo ", 7)) ||
	    (!strncmp(buff, "time", 4) && (buff[4] == '\0' || buff[4] == ' ')))
	{
		CONT_BUFF_COMANDOS = '\0';
		int tiempoUser = juego.maxTiempo/1000;
		if (!strncmp(buff, "tiempo ", 7))
			sscanf(buff, "tiempo %d", &tiempoUser);
		else
			sscanf(buff, "time %d", &tiempoUser);
		if (tiempoUser < 1) tiempoUser = 1;
		juego.maxTiempo = tiempoUser * 1000;
		printf("juego.maxTiempo = %d\n",juego.maxTiempo);
		nivelPC = PC_NIVEL_PERSONALIZADO;
		return VERDADERO;    
	}

	if ((!strcmp(buff, "new")) || (!strcmp(buff, "nuevo")))
	{
		CONT_BUFF_COMANDOS = '\0';
		nuevo_juego();
		printf("Motor reiniciado\n");
		return VERDADERO;    
	}

	if ((!strcmp(buff, "white")) || (!strcmp(buff, "blanco")) || (!strcmp(buff, "blancas")))
	{
		CONT_BUFF_COMANDOS = '\0';
		if (juego.colorTurno)
		{
			juego.colorTurno   = BLANCO;
			juego.llaveHash   ^= arrayHash.lado;
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		}
		return VERDADERO;    
	}

	if ((!strcmp(buff, "black")) || (!strcmp(buff, "negro")) || (!strcmp(buff, "negras")))
	{
		CONT_BUFF_COMANDOS = '\0';
		if (!juego.colorTurno)
		{
			juego.colorTurno   = NEGRO;
			juego.llaveHash   ^= arrayHash.lado;
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		}
		return VERDADERO;    
	}


	if ((!strcmp(buff, "info")) || (!strcmp(buff, "i")))
	{
		CONT_BUFF_COMANDOS = '\0';
		informacionVariables();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "r")) || (!strcmp(buff, "g")) || (!strcmp(buff, "girar")) || (!strcmp(buff, "flip")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.vista = !juego.vista;
		mostrarTablero();
		return VERDADERO;    
	}



	if ((!strcmp(buff, "d")) || (!strcmp(buff, "t")) || (!strcmp(buff, "tablero")) || (!strcmp(buff, "board")))
	{
		CONT_BUFF_COMANDOS = '\0';
		mostrarTablero();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "game")) || (!strcmp(buff, "juego")) || (!strcmp(buff, "partida")))
	{
		CONT_BUFF_COMANDOS = '\0';
		verMovimentosJuego();
		return VERDADERO;    
	}


	if ((!strcmp(buff, "moves")) || (!strcmp(buff, "movimientos")) || (!strcmp(buff, "legales")))
	{
		CONT_BUFF_COMANDOS = '\0';
		verMovimentosLegales();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "genmov")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		verMovimentosLegales();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "genmovcap")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.Buffer_MOV_INDEXCAPAS[1] = generarMovCapPro(0);
		verMovimentosLegales();
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		return VERDADERO;    
	}

	if ((!strcmp(buff, "eetpos")))
	{
		CONT_BUFF_COMANDOS = '\0';
		int ponderacion=0,i=0;
		for(i=0;i<64;i++)
		{
			
			printf("eetpos pos=%d valor=%d\n",i, EETPOS(i, juego.colorTurno));
			ponderacion += EETPOS(i,  juego.colorTurno);
		}
		printf("ponderacion=%d\n",ponderacion);
		return VERDADERO;    
	}

	if ((!strcmp(buff, "genmovcapt")))
	{
		CONT_BUFF_COMANDOS = '\0';
		juego.Buffer_MOV_INDEXCAPAS[1] = generarMovCapPro(0);
		verMovimentosLegales();
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		return VERDADERO;    
	}

//*
	if ((!strncmp(buff, "test",4)))
	{
		CONT_BUFF_COMANDOS = '\0';
		char nombreArchivo[100];
		printf("Por favor, espere mientras se realiza la prueba...\n");
		FILE *fp = freopen("test.txt", "w",stdout);
		sscanf(buff,"test %s", nombreArchivo);

		printf("MODO PRUEBA en %s\n",nombreArchivo);

		int i=1;
		while(readFen(nombreArchivo, i))
		{
			printf("Iteración n.º %d\n",i);
			pensarRapido();
			i++;
		}	

		fclose(fp);
//		stdout = fdopen(2, "w");
		return VERDADERO;    
	}
//*/
	if (buff[0] == 'e' && buff[1] == 'e' && buff[2] == 't')  
	{
		CONT_BUFF_COMANDOS = '\0';
		int escaque=0;
		int ccolor=0;
		sscanf(buff,"eet %d %d", &escaque, &ccolor);
		if (escaque >= 0 && escaque < 64 )
		{
			printf("Escaque=%d EET=%d\n",escaque, EETPOS(escaque, ccolor));
		} else {
			printf("Escaque=%d fuera de rango (0-63)\n",escaque);
		}
		return VERDADERO;    
	}


	if (((!strcmp(buff, "go")) || (!strcmp(buff, "dale")) || (!strcmp(buff, "jugar"))) && STATUS_FIN_JUEGO_NO_TERMINADO)
	{

		CONT_BUFF_COMANDOS = '\0';
		printf("Pensando... profundidad: %d\n", juego.profundidadBusquedad);
		esConsola = VERDADERO;
		m = pensarRapido();
		esConsola = FALSO;
		if (!m)
		{
			printf("\nNo se encontró ningún movimiento válido\n");
			return VERDADERO;    
		} 

		hacerMovimiento(m);

		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);

		imprimirMovimiento(m);	
		mostrarTablero();
		statusFinJuego = obtEstadoJuego();
		mostrarMensajeFinalJuego();

		return VERDADERO;    
	}

	if (!strcmp(buff, "."))
	{

		CONT_BUFF_COMANDOS = '\0';
		printf("Pensando sin búsqueda de tranquilidad... profundidad: %d\n", juego.profundidadBusquedad);
		esVariable = VERDADERO;	
		m = pensarRapido();
		esVariable = FALSO;	

		if (!m)
		{
			printf("\nNo se encontró ningún movimiento válido\n");
			return VERDADERO;    
		} 

		hacerMovimiento(m);
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		imprimirMovimiento(m);	
		mostrarTablero();

		return VERDADERO;    
	}

	if ((!strcmp(buff, "uci")))
	{
		CONT_BUFF_COMANDOS = '\0';
		uci();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "xboard")))
	{
		CONT_BUFF_COMANDOS = '\0';
		xboard();
		return VERDADERO;    
	}


	if ((!strcmp(buff, "exit")) || (!strcmp(buff, "quit")) || (!strcmp(buff, "salir")))
	{
		CONT_BUFF_COMANDOS = '\0';
		cerrarLibro3();
		cerrarTablas();
#ifdef COMPILAR_CON_EGBB
		cerrarBitbases();
#endif
		return FALSO;    
	}

	if ((!strcmp(buff, "librosi")) || (!strcmp(buff, "libro si")) || (!strcmp(buff, "libro on")))
	{
		CONT_BUFF_COMANDOS = '\0';
		usarLibroAperturas = VERDADERO;
		nFueraLibro = 0;
		if (!esActivoLibro)
			abrirLibro3();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "librono")) || (!strcmp(buff, "libro no")) || (!strcmp(buff, "libro off")))
	{
		CONT_BUFF_COMANDOS = '\0';
		usarLibroAperturas = FALSO;
		return VERDADERO;    
	}

	if ((!strcmp(buff, "ver")) || (!strcmp(buff, "version")))
	{
		CONT_BUFF_COMANDOS = '\0';
		printf("%s\n",VERSION_MANGO_AJEDREZ);
		return VERDADERO;    
	}
	

	if (((!strcmp(buff, "cc")) || (!strcmp(buff, "auto"))) && STATUS_FIN_JUEGO_NO_TERMINADO)
	{
		CONT_BUFF_COMANDOS = '\0';
		statusFinJuego = obtEstadoJuego();

		while (STATUS_FIN_JUEGO_NO_TERMINADO && (juego.indiceHJuego < MAX_BUFF_MOV))
		{
			tiempo_ms  = 0;  
			prepararTiempo();
			iniciarTiempo();
			m = pensarRapido();
			detenerTiempo();
			tiempo_ms  = obt_msTiempo();
			printf("Tiempo de proceso: %lld ms ",tiempo_ms);
			if (!m)
			{
				printf("\nNo se encontró ningún movimiento válido\n");
				break;
			} 
			hacerMovimiento(m);
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			imprimirMovimiento(m);	
			mostrarTablero();
			statusFinJuego = obtEstadoJuego();
		}

		mostrarMensajeFinalJuego();
		return VERDADERO;    
	}

	if ((!strcmp(buff, "ver ocupados")))
	{
		CONT_BUFF_COMANDOS = '\0';
		imprimirBitTablero(juego.ocupados);
		return VERDADERO;    
	}

	if ((!strcmp(buff, "ver blancos")))
	{
		CONT_BUFF_COMANDOS = '\0';
		imprimirBitTablero(juego.blancos);
		return VERDADERO;    
	}

	if ((!strcmp(buff, "ver negros")))
	{
		CONT_BUFF_COMANDOS = '\0';
		imprimirBitTablero(juego.negros);
		return VERDADERO;    
	}

	if ((!strcmp(buff, "ver peones blancos")))
	{
		CONT_BUFF_COMANDOS = '\0';
		imprimirBitTablero(juego.tablero[BLANCO][PEON]);
		return VERDADERO;    
	}

	if ((!strcmp(buff, "ver peones negros")))
	{
		CONT_BUFF_COMANDOS = '\0';
		imprimirBitTablero(juego.tablero[NEGRO][PEON]);
		return VERDADERO;    
	}

	if ((!strcmp(buff, "ver destinos blancos")))
	{
		CONT_BUFF_COMANDOS = '\0';
		imprimirBitTablero((~juego.ocupados | juego.negros));
		return VERDADERO;    
	}

	if ((!strcmp(buff, "ver destinos negros")))
	{
		CONT_BUFF_COMANDOS = '\0';
		imprimirBitTablero((~juego.ocupados | juego.blancos));
		return VERDADERO;    
	}


	printf("Comando desconocido. Escriba \"ayuda\" para ver la lista de comandos.\n");
	CONT_BUFF_COMANDOS = '\0';
	return VERDADERO;    
}

void mostrarMensajeFinalJuego()
{
			if (statusFinJuego == STATUS_FIN_JUEGO_MATE_GANAN_B){

				printf("result 1-0 {White mates}\n");

			} else if (statusFinJuego == STATUS_FIN_JUEGO_MATE_GANAN_N){

				printf("result 0-1 {Black mates}\n");

			} else if (statusFinJuego == STATUS_FIN_JUEGO_TABLAS_REPETICION) {

				printf("result 1/2-1/2 {Draw by repetition}\n");

			} else if (statusFinJuego == STATUS_FIN_JUEGO_TABLAS_INSUFI_MATERIAL) {

				printf("result 1/2-1/2 {Draw by insufficient material}\n");
			}

}


void mostrarTablero()
{
	int rank, file;
	printf("\n");

	if (!juego.vista)
        {
		for (rank = 8; rank >= 1; rank--)
                {
			printf("    +---+---+---+---+---+---+---+---+\n");
			printf("  %d |",rank);
                        for (file = 1; file <= 8; file++)
                        {
				printf(" %s|",NOMBREPIEZA[ESCAQUES[FILERANK2POS[file][rank]]]);
                        }
			printf("\n"); 	  
		}
		printf("    +---+---+---+---+---+---+---+---+\n");
		printf("      a   b   c   d   e   f   g   h\n");
	} else {
		printf("      h   g   f   e   d   c   b   a\n");
		for (rank = 1; rank <= 8; rank++)
		{
			printf("    +---+---+---+---+---+---+---+---+\n");
			printf("    |");
                        for (file = 8; file >= 1; file--)
                        {
				printf(" %s|",NOMBREPIEZA[ESCAQUES[FILERANK2POS[file][rank]]]);
                        }
			printf(" %d \n",rank);
                 }
		printf("    +---+---+---+---+---+---+---+---+\n");
	}

	return;

}



void verMovimentosLegales()
{
	int i=0;
	BOOLEANO esJaque;	

	if (juego.Buffer_MOV_INDEXCAPAS[0] == juego.Buffer_MOV_INDEXCAPAS[1])
	{
		printf("No hay movimientos válidos\n");
		return;
	}

	for (i=juego.Buffer_MOV_INDEXCAPAS[0];i<juego.Buffer_MOV_INDEXCAPAS[1];i++)
	{

		hacerMovimiento(juego.Buffer_MOV[i]);

		if (juego.colorTurno) // Si le toca al negro, se comprueba que el rey blanco no haya quedado en jaque
		{
			esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
		} else {
			esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
		}

		desHacerMovimiento(juego.Buffer_MOV[i]);
		
		if (!esJaque)
		{
			verMov(juego.Buffer_MOV[i]);
		}
	}
}

void verMovimentosTodos()
{
	int i=0;

	if (juego.Buffer_MOV_INDEXCAPAS[0] == juego.Buffer_MOV_INDEXCAPAS[1])
	{
		printf("No hay movimientos válidos\n");
		return;
	}

	for (i=juego.Buffer_MOV_INDEXCAPAS[0];i<juego.Buffer_MOV_INDEXCAPAS[1];i++)
	{
		verMov(juego.Buffer_MOV[i]);
	}
}


void verMovimentosJuego()
{
	int i=0;

	for (;i< juego.indiceHJuego;i++)
	{
		printf("%c ", ((i % 2) ? 'N':'B'));
		printf("%s ", juego.historicoJuego[i].movAlg);
		printf("ori=%d des=%d pieza=%d cap=%d\n",OBT_MOV_ORIGEN(juego.historicoJuego[i].mov),
							OBT_MOV_DESTINO(juego.historicoJuego[i].mov),
							OBT_MOV_PIEZA(juego.historicoJuego[i].mov),
							OBT_MOV_CAPTURA(juego.historicoJuego[i].mov));
	}
}

void informacionVariables()
{
	printf("*** Información de las variables del juego ***\n\n");
	printf("Profundidad de búsqueda	(profundidadBusquedad)	: %d\n", juego.profundidadBusquedad);
	printf("Tiempo por movimiento		(juego.maxTiempo)	: %d ms\n", juego.maxTiempo);
	printf("Libro de Aperturas (SI/NO)	(usarLibroAperturas)	: %s\n", usarLibroAperturas?"Si":"No");
	printf("Nivel PC			(nivelPC)		: %s\n", NOMBRENIVELPC[nivelPC]);
	printf("Balance material		(material_total)	: %d\n", juego.material_total);
	printf("Material blanco			(material_lado_blanco)	: %d\n", juego.material_lado_blanco);
	printf("Material peones blanco		(material_peon_blanco)	: %d\n", juego.material_peon_blanco);
	printf("Material negro			(material_lado_negro)	: %d\n", juego.material_lado_negro);
	printf("Material peones negro		(material_peon_negro)	: %d\n", juego.material_peon_negro);
	printf("Bando que juega		(colorTurno)		: %s\n",(juego.colorTurno?"Negro":"Blanco"));
	printf("# Movimientos \"Regla 50 mov\"	(reglaCincuentaMov)	: %d\n",juego.reglaCincuentaMov);
	printf("# Total Movimientos 		(totalMov)		: %d\n",juego.totalMov);
	printf("Es posible OO  blanco		(OOB)			: %s\n",(juego.OOB?"Si":"No"));
	printf("Es posible OOO blanco		(OOOB)			: %s\n",(juego.OOOB?"Si":"No"));
	printf("Es posible OO  negro		(OON)			: %s\n",(juego.OON?"Si":"No"));
	printf("Es posible OOO negro		(OOON)			: %s\n",(juego.OOON?"Si":"No"));
	printf("Hubo Enroque blanco		(ENROQUEB)		: %s\n",(juego.ENROQUEB?"Si":"No"));
	printf("Hubo Enroque negro		(ENROQUEN)		: %s\n",(juego.ENROQUEN?"Si":"No"));
	printf("Índice de la partida		(juego.indiceHJuego)	: %d\n",juego.indiceHJuego);
	if (juego.posPeonPaso==SIN_POS_VALIDA)
	{
		printf("Escaque \"peón al paso\"		(posPeonPaso)		: Sin una posición válida\n");
	} else {
		printf("Escaque \"peón al paso\"		(posPeonPaso)		: %d\n",juego.posPeonPaso);
	}
	printf("Utilidad			(numEstatico)		: %d\n",numEstatico);
	printf("maxHistorial Blanco		(maxHistorial[BLANCO])	: %d\n",maxHistorial[BLANCO]);
	printf("maxHistorial Negro		(maxHistorial[NEGRO])	: %d\n",maxHistorial[NEGRO]);

}


void verMov(MOVIMIENTO mov)
{
	printf("Ori=%d, Des=%d, Pieza=%d, Captura=%d, Promocion=%d, cod=%d, EET=%d\n", 
		OBT_MOV_ORIGEN(mov),  OBT_MOV_DESTINO(mov),   OBT_MOV_PIEZA(mov),
		OBT_MOV_CAPTURA(mov), OBT_MOV_PROMOCION(mov), OBT_MOV_CODIGO(mov), EET(mov));
	 
}

void verMov2(MOVIMIENTO mov)
{
	printf("%d-%d\n",OBT_MOV_ORIGEN(mov),OBT_MOV_DESTINO(mov));
 
}

void nuevo_juego()
{
		inicializarTablero();
		inicializarVar();
		iniTablerosUtil();
		actualizarTablerosUtil(BLANCO);
		iniHashTablero();
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
}
//*

void xboard()
{
//	int quedan=0;
	int entrada = 0;                                
	char minutos[4];	                        
	int incremento;	                                
	int inc = 0;
	int mps = 0;
	BOOLEANO esPermitirPonder = FALSO;

	computadora = NEGRO;
	char linea[256], buffComandos[256];
	MOVIMIENTO m, ponder_mov=0;
	BOOLEANO esJaque;	
	esPost = VERDADERO;
	BOOLEANO esPonder=FALSO;

	nuevo_juego();
	computadora = NEGRO;

	for (;;) 
	{
		if (juego.colorTurno == computadora) {

			tipoDeBusqueda = TIPO_BUSQUEDA_NORMAL;
			m = pensarRapido();
			if (!m) continue; 
			hacerMovimiento(m);
			printf("move "); imprimirMovimiento(m);printf("\n");
			juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			statusFinJuego = obtEstadoJuego();
			mostrarMensajeFinalJuego();
			esPermitirPonder = VERDADERO;
			continue;

		} else if (computadora != SIN_COLOR) {
			if (esPonder && esPermitirPonder)
			{
				if (esMovimentoLegal(juego.triangularArray[0][1]))
				{
					tipoDeBusqueda = TIPO_BUSQUEDA_PONDER;
					mponder = juego.triangularArray[0][1];
					hacerMovimiento(mponder);
					ponder_mov = pensarRapido();
					desHacerMovimiento(mponder);
					juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
				}

				esPermitirPonder = FALSO;
				continue;
			}
		}

		if (!fgets(linea, 256, stdin))
			return;
		if (linea[0] == '\n')
			continue;
		sscanf(linea, "%s", buffComandos);
		if (!strcmp(buffComandos, "xboard"))
		{
			continue;
		}
		if (!strcmp(buffComandos, "new"))
		{
			nuevo_juego();
			computadora = NEGRO;
			continue;
		}
		if (!strcmp(buffComandos, "quit"))
		{
			cerrarLibro3();
			cerrarTablas();
			return;
		}
		if (!strcmp(buffComandos, "white"))
		{
			juego.colorTurno = BLANCO;
			computadora = NEGRO;
			continue;
		}
		if (!strcmp(buffComandos, "black"))
		{
			juego.colorTurno = NEGRO;
			computadora = BLANCO;
			continue;
		}
		if (!strcmp(buffComandos, "st"))
		{
			sscanf(linea, "st %d", &juego.maxTiempo);
			juego.maxTiempo *= 1000;
			juego.profundidadBusquedad = MAX_CAPAS;
			continue;
		}
		if (!strcmp(buffComandos, "sd"))
		{
			sscanf(linea, "sd %d", &juego.profundidadBusquedad);
			juego.maxTiempo = 1 << 25;
			continue;
		}

		if (!strcmp(buffComandos, "level"))
		{
			sscanf(linea, "level %d %s %d", &entrada, minutos, &incremento);
			mps = entrada;	    
			inc = incremento * 1000;
			continue;
		}

		if (!strcmp(buffComandos, "time"))
		{
			sscanf(linea, "time %d", &juego.maxTiempo);

			juego.maxTiempo *= 10;
			juego.maxTiempo /= 25;
			juego.maxTiempo -= 110;
/*
			if (mps == 0 && inc > 0)
			{
				if (juego.maxTiempo < 3 * inc)
				{
					juego.maxTiempo = 75*inc/100;

				} else {
					juego.maxTiempo = (juego.maxTiempo/30 + inc)*4;
				}
				continue;
			}

			if (computadora == BLANCO)
				quedan = (mps-juego.totalMov/2)-1;
			else
				quedan = (mps-juego.totalMov/2);
			if (mps == 0) {
				quedan = 30;
			}
			else {
				while (quedan <= 0)
					quedan += mps;
			}
			juego.maxTiempo = (juego.maxTiempo/quedan)*(5/4);
//*/
			continue;
		}

		if (!strcmp(buffComandos, "otim"))
		{
			continue;
		}
		if (!strcmp(buffComandos, "go"))
		{
			computadora = juego.colorTurno;
			continue;
		}
		if (!strcmp(buffComandos, "protover"))
		{
			printf("feature setboard=1 analyze=0 ping=1 colors=0 draw=0 sigint=0 sigterm=0 usermove=0"
				   " variants=\"normal\" myname=\"%s %s\"\n",NOMBRE_MANGO_AC, VERSION_MANGO_AC);
			printf("feature done=1\n");
			continue;
		}

		if (!strcmp(buffComandos, "accepted"))
		{
			continue;
		}
		if (!strcmp(buffComandos, "rejected"))
		{
			continue;
		}
		if (!strcmp(buffComandos, "setboard"))
		{
			char fen[100];
			char fencolor[2];     
			char fenEnroque[5];
			char fenPeonPaso[3];
			int fenReglaCincuentaMov=0;
			int fenTotalMov=0;

			sscanf(linea, "setboard %s %s %s %s %d %d",
						fen,fencolor,fenEnroque,fenPeonPaso,&fenReglaCincuentaMov,&fenTotalMov);
			setupFen(fen, fencolor, fenEnroque, fenPeonPaso, fenReglaCincuentaMov, fenTotalMov);
			continue;
		}


		if (!strcmp(buffComandos, "hint"))
		{
			m = pensarRapido();
			if (!m) continue; 
			printf("Hint: ");
			imprimirMovimiento(m);printf("\n");
			continue;
		}
		if (!strcmp(buffComandos, "undo"))
		{
			if (juego.indiceHJuego > 0)
			{
				desHacerMovimiento(juego.historicoJuego[juego.indiceHJuego-1].mov);
				juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			}
			continue;
		}
		if (!strcmp(buffComandos, "remove"))
		{
			if (juego.indiceHJuego > 1)
			{
				desHacerMovimiento(juego.historicoJuego[juego.indiceHJuego-1].mov);
				desHacerMovimiento(juego.historicoJuego[juego.indiceHJuego-1].mov);
				juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
			}
			continue;
		}
		if (!strcmp(buffComandos, "force"))
		{
			computadora = SIN_COLOR;
			continue;
		}
		if (!strcmp(buffComandos, "result"))
		{
			computadora = SIN_COLOR;
			continue;
		}
		if (!strcmp(buffComandos, "?"))
		{
			computadora = SIN_COLOR;
			continue;
		}
		if (!strcmp(buffComandos, "post"))
		{
			esPost = VERDADERO;
			continue;
		}
		if (!strcmp(buffComandos, "nopost"))
		{
			esPost = FALSO;
			continue;
		}
		if (!strcmp(buffComandos, "ping"))
		{
			int n;
			sscanf(linea, "ping %d", &n);
			printf("pong %d\n",n);
			continue;
		}

		if (!strcmp(buffComandos, "random"))
		{
			continue;
		}
		if (!strcmp(buffComandos, "computer"))
		{
			continue;
		}
		if (!strcmp(buffComandos, "easy")) 
		{
			esPonder=FALSO;
			continue;
		}
		if (!strcmp(buffComandos, "hard"))
		{
			esPonder=VERDADERO;
			continue;
		}
		if (!strcmp(buffComandos, "name"))
		{
			continue;
		}
		if (!strcmp(buffComandos, "bk"))
		{
			continue;
		}
		if (!strcmp(buffComandos, "edit"))
		{
			continue;
		}
		if (!strcmp(buffComandos, "rating"))
		{
			continue;
		}
		if (!strcmp(buffComandos, "ics"))
		{
			continue;
		}
		if (!strcmp(buffComandos, "."))
		{
			continue;
		}

		if (!strcmp(buffComandos, "usermove"))
		{
			if (sscanf(linea, "usermove %s", buffComandos) != 1)
				continue;
		}

		if (!esUnMovimiento(buffComandos))
		{
			printf("Error (unknown command): %s\n", buffComandos);
			continue;
		}

		m = parse_mov(buffComandos);
		if (m == -1)
			printf("Illegal move: %s\n", buffComandos);
		else {
			hacerMovimiento(m);

			if (juego.colorTurno) // Si le toca al negro, se comprueba que el rey blanco no haya quedado en jaque
			{
				esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
			} else {
				esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
			}
			
			if (!esJaque)
			{
				juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);

			} else {
				printf("Illegal move (in check): %s\n", buffComandos);
				desHacerMovimiento(m);
			}
		}
	}
//*/
}


BOOLEANO esUnMovimiento(const char *l)
{
	if (l[0] > 96 && l[0] < 105)
	{
		if (l[1] > 48 && l[1] < 57)
		{
			if (l[2] > 96 && l[2] < 105)
			{
				if (l[3] > 48 && l[3] < 57)
				{
					return VERDADERO;
				}			
			}
		}	
	}

	return FALSO;
}

MOVIMIENTO parse_mov(char *s)
{
	uint8 origen, destino;
	int i = 0;
	PIEZA pCoronada = VACIO;

	origen  = s[0]   - 97;
	origen += ((s[1] - 49) * 8);

	destino  = s[2]   - 97;
	destino += ((s[3] - 49) * 8);

	for (i=juego.Buffer_MOV_INDEXCAPAS[0];i<juego.Buffer_MOV_INDEXCAPAS[1];i++)
	{
		if ( ((OBT_MOV_ORIGEN(juego.Buffer_MOV[i])) == origen) &&  ((OBT_MOV_DESTINO(juego.Buffer_MOV[i]))==destino))
		{
			if (ES_MOV_PROMOCION(juego.Buffer_MOV[i]))
			{
				switch (s[4]) 
				{
					case 'n':
					case 'N':
						pCoronada = OBT_MOV_PROMOCION(juego.Buffer_MOV[i]);
						if (pCoronada == CABALLO_BLANCO || pCoronada == CABALLO_NEGRO)
							return juego.Buffer_MOV[i];
						break;
					case 'b':
					case 'B':
						pCoronada = OBT_MOV_PROMOCION(juego.Buffer_MOV[i]);
						if (pCoronada == ALFIL_BLANCO || pCoronada == ALFIL_NEGRO)
							return juego.Buffer_MOV[i];
						break;
					case 'r':
					case 'R':
						pCoronada = OBT_MOV_PROMOCION(juego.Buffer_MOV[i]);
						if (pCoronada == TORRE_BLANCO || pCoronada == TORRE_NEGRO)
							return juego.Buffer_MOV[i];
						break;
					case 'q':
					case 'Q':
						pCoronada = OBT_MOV_PROMOCION(juego.Buffer_MOV[i]);
						if (pCoronada == DAMA_BLANCO || pCoronada == DAMA_NEGRO)
							return juego.Buffer_MOV[i];
						break;
					default:  
						return juego.Buffer_MOV[i];
				}

			} else {

				return juego.Buffer_MOV[i];
			}
		}
	}

	return -1;
}

BOOLEANO esMovimentoLegal(MOVIMIENTO m)
{
	int i=0;
	BOOLEANO esJaque;	

	if (juego.Buffer_MOV_INDEXCAPAS[0] == juego.Buffer_MOV_INDEXCAPAS[1])
	{
		return FALSO;
	}

	for (i=juego.Buffer_MOV_INDEXCAPAS[0];i<juego.Buffer_MOV_INDEXCAPAS[1];i++)
	{

		hacerMovimiento(juego.Buffer_MOV[i]);

		if (juego.colorTurno) // Si le toca al negro, se comprueba que el rey blanco no haya quedado en jaque
		{
			esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
		} else {
			esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
		}

		desHacerMovimiento(juego.Buffer_MOV[i]);
		
		if (!esJaque)
		{
			if (juego.Buffer_MOV[i]==m) return VERDADERO;
		}
	}

	return FALSO;
}
//*/
#endif



