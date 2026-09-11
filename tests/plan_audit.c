/* Focused integration checks for Plan_mejora_evaluacion_Mango_AC.md. */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define main mango_engine_main
#include "bitmma3.c"
#undef main

static void init_audit(void)
{
	esUsoTablaHash = VERDADERO;
	usarLibroAperturas = FALSO;
	hashMBSolicitados = 1;
	iniMatrizMov();
	iniMatrizSeguridadRey();
	iniCarreraPeonesMask();
	iniPeonEscondido();
	iniPeonCC();
	ini_is_outside();
	inicializarTablero();
	inicializarVar();
	iniTablerosUtil();
	actualizarTablerosUtil(BLANCO);
	iniciarHash();
	iniHashTablero();
}

static void load_fen(const char *fen)
{
	char board[128], side[8], castle[16], ep[8];
	int halfmove, fullmove;

	if (sscanf(fen, "%127s %7s %15s %7s %d %d",
		   board, side, castle, ep, &halfmove, &fullmove) != 6)
		abort();
	setupFen(board, side, castle, ep, halfmove, fullmove);
	juego.Buffer_MOV_INDEXCAPAS[0] = 0;
}

static MOVIMIENTO make_uci(const char *uci)
{
	MOVIMIENTO mov = 0;
	char command[32];

	snprintf(command, sizeof(command), "move %s", uci);
	if (!esValidoMovUsuario(command, &mov))
		abort();
	hacerMovimiento(mov);
	return mov;
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return 2;

	init_audit();
	if (strcmp(argv[1], "repetition") == 0)
	{
		load_fen("7k/8/8/8/8/8/P7/K7 w - - 40 1");
		limpiarAntesDeBusqueda();
		tiempoVencido = FALSO;
		contadorDescendente = INT_MAX;
		printf("repetition_search=%d\n",
		       busquedadTranquilidad(0, -INFINITO, INFINITO));
	}
	else if (strcmp(argv[1], "rule50") == 0)
	{
		MOVIMIENTO mov;

		load_fen("7k/8/8/8/r2R4/8/8/K7 w - - 25 1");
		mov = make_uci("d4a4");
		printf("capture=%u ", (unsigned)juego.reglaCincuentaMov);
		desHacerMovimiento(mov);
		printf("capture_undo=%u\n", (unsigned)juego.reglaCincuentaMov);

		load_fen("7k/8/8/8/3R4/8/8/K7 w - - 300 1");
		mov = make_uci("d4d5");
		printf("quiet=%u ", (unsigned)juego.reglaCincuentaMov);
		desHacerMovimiento(mov);
		printf("quiet_undo=%u\n", (unsigned)juego.reglaCincuentaMov);
	}
	else if (strcmp(argv[1], "fen_reset") == 0)
	{
		juego.ENROQUEB = ENROQUE_OO;
		juego.ENROQUEN = ENROQUE_OOO;
		juego.indiceHJuego = 7;
		load_fen("7k/8/8/8/8/8/P7/K7 w - - 0 1");
		printf("castled_white=%d castled_black=%d history=%u\n",
		       juego.ENROQUEB, juego.ENROQUEN,
		       (unsigned)juego.indiceHJuego);
	}
	else
	{
		cerrarTablas();
		return 2;
	}
	cerrarTablas();
	return 0;
}
