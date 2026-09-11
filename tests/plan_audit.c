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

static int full_uncached(void)
{
	HASH_EVAL *saved = hash_eval;
	int value;

	hash_eval = NULL;
	value = evaluacionTablero(-INFINITO, INFINITO);
	hash_eval = saved;
	return value;
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
	else if (strcmp(argv[1], "null_state") == 0)
	{
		ESTADO_MOV_NULL estado;
		uint64 llaveAntes;
		uint8 epAntes;
		uint16 relojAntes;
		COLOR colorAntes;

		load_fen("7k/8/8/8/3pP3/8/8/K7 b - e3 87 1");
		llaveAntes = juego.llaveHash;
		epAntes = juego.posPeonPaso;
		relojAntes = juego.reglaCincuentaMov;
		colorAntes = juego.colorTurno;
		hacerMovimientoNull(&estado);
		printf("during_ep_clear=%d during_clock=%u side_changed=%d ",
		       juego.posPeonPaso == SIN_POS_VALIDA,
		       (unsigned)juego.reglaCincuentaMov,
		       juego.colorTurno != colorAntes);
		desHacerMovimientoNull(&estado);
		printf("restored=%d\n",
		       juego.llaveHash == llaveAntes &&
		       juego.posPeonPaso == epAntes &&
		       juego.reglaCincuentaMov == relojAntes &&
		       juego.colorTurno == colorAntes);
	}
	else if (strcmp(argv[1], "material") == 0)
	{
		BOOLEANO insuficiente = FALSO;

		load_fen("7k/5K2/5NN1/8/8/8/8/8 b - - 0 1");
		ES_INSUFICIENTE_MATERIAL(insuficiente);
		computadora = NEGRO;
		printf("insufficient=%d status=%d expected=%d\n",
		       insuficiente, obtEstadoJuego(),
		       STATUS_FIN_JUEGO_MATE_GANAN_B);
	}
	else if (strcmp(argv[1], "bishop_defense") == 0)
	{
		load_fen("7k/7p/8/8/r2R4/8/P7/K5B1 w - - 0 1");
		full_uncached();
		printf("raw=%d map=%d threat_black=%d\n",
		       !!(genAlfilMOVAtaqueTablero(6, juego) & BITSET[27]),
		       !!(mapaPosAtacadas[BLANCO] & BITSET[27]),
		       puntajeAmenazas(NEGRO));
	}
	else if (strcmp(argv[1], "king_defense") == 0)
	{
		load_fen("7k/7p/8/8/r2R4/2K5/P7/8 w - - 0 1");
		full_uncached();
		printf("raw=%d map=%d threat_black=%d\n",
		       !!(mascaraRey[18] & BITSET[27]),
		       !!(mapaPosAtacadas[BLANCO] & BITSET[27]),
		       puntajeAmenazas(NEGRO));
	}
	else
	{
		cerrarTablas();
		return 2;
	}
	cerrarTablas();
	return 0;
}
