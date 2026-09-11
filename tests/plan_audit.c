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

int main(int argc, char **argv)
{
	if (argc != 2 || strcmp(argv[1], "repetition") != 0)
		return 2;

	init_audit();
	load_fen("7k/8/8/8/8/8/P7/K7 w - - 40 1");
	limpiarAntesDeBusqueda();
	tiempoVencido = FALSO;
	contadorDescendente = INT_MAX;
	printf("repetition_search=%d\n",
	       busquedadTranquilidad(0, -INFINITO, INFINITO));
	cerrarTablas();
	return 0;
}
