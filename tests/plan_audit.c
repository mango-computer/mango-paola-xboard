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
	else if (strcmp(argv[1], "mobility") == 0)
	{
		uint64 raw, useful;

		load_fen("7k/8/8/3P4/3R4/8/8/K7 w - - 0 1");
		full_uncached();
		raw = genTorreMOVAtaqueTablero(27, juego);
		useful = areaMovilidadUtil(BLANCO, 27, raw);
		printf("own_blocker_raw=%d own_blocker_useful=%d\n",
		       !!(raw & BITSET[35]), !!(useful & BITSET[35]));
	}
	else if (strcmp(argv[1], "promotion_path") == 0)
	{
		int safeMg, safeEg;

		load_fen("7k/P7/8/8/8/8/8/K7 w - - 0 1");
		full_uncached();
		mapaPosAtacadas[NEGRO] = 0;
		puntajeEval_m[BLANCO] = puntajeEval_f[BLANCO] = 0;
		evalPeonesPasados(BLANCO);
		safeMg = puntajeEval_m[BLANCO];
		safeEg = puntajeEval_f[BLANCO];
		mapaPosAtacadas[NEGRO] = BITSET[56];
		puntajeEval_m[BLANCO] = puntajeEval_f[BLANCO] = 0;
		evalPeonesPasados(BLANCO);
		printf("promotion_delta_mg=%d promotion_delta_eg=%d\n",
		       safeMg - puntajeEval_m[BLANCO],
		       safeEg - puntajeEval_f[BLANCO]);
	}
	else if (strcmp(argv[1], "pawnless") == 0)
	{
		int first, second;

		load_fen("6k1/1q6/8/8/8/8/8/K6Q w - - 0 1");
		first = full_uncached();
		load_fen("6k1/1q6/8/8/7Q/8/8/K7 w - - 0 1");
		second = full_uncached();
		printf("queen_h1=%d queen_h4=%d attacks=%llu\n",
		       first, second,
		       (unsigned long long)
		       (mapaPosAtacadas[BLANCO] | mapaPosAtacadas[NEGRO]));
	}
	else if (strcmp(argv[1], "lazy_rule50") == 0)
	{
		HASH_EVAL *saved = hash_eval;
		int full, narrow;

		load_fen("7k/8/8/8/8/8/8/KQ6 w - - 99 1");
		hash_eval = NULL;
		full = evaluacionTablero(-INFINITO, INFINITO);
		narrow = evaluacionTablero(55, 56);
		hash_eval = saved;
		printf("full=%d narrow=%d equal=%d\n",
		       full, narrow, full == narrow);
	}
	else if (strcmp(argv[1], "eval_cache") == 0)
	{
		int cold, warm;

		load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		limpiarTablasHash();
		cold = evaluacionTablero(-INFINITO, INFINITO);
		mapaPosAtacadas[BLANCO] = 0xdeadbeefULL;
		warm = evaluacionTablero(-INFINITO, INFINITO);
		printf("same_score=%d cache_is_score_only=%d\n",
		       cold == warm,
		       mapaPosAtacadas[BLANCO] == 0xdeadbeefULL);
	}
	else if (strcmp(argv[1], "eval_result_class") == 0)
	{
		int fullClass, cacheClass, lazyClass;

		load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		limpiarTablasHash();
		evaluacionTablero(-INFINITO, INFINITO);
		fullClass = ultimoResultadoEval;
		evaluacionTablero(-INFINITO, INFINITO);
		cacheClass = ultimoResultadoEval;
		limpiarTablasHash();
		evaluacionTablero(10000, 10001);
		lazyClass = ultimoResultadoEval;
		printf("full=%d cache=%d lazy=%d\n",
		       fullClass, cacheClass, lazyClass);
	}
	else if (strcmp(argv[1], "bitops") == 0)
	{
		uint64 value = 0x9e3779b97f4a7c15ULL;
		unsigned sample;

		for (sample = 0; sample < 100000; sample++)
		{
			uint64 probe;
			uint32 count = 0;
			int first = 0, last = 63;

			value = value * 6364136223846793005ULL + 1;
			probe = value | 1ULL;
			for (uint64 copy = probe; copy; copy &= copy - 1)
				count++;
			while (!(probe & (1ULL << first)))
				first++;
			while (!(probe & (1ULL << last)))
				last--;
			if (cuentaBit(probe) != count ||
			    bitScanForwardBruijn(probe) != first ||
			    bitScanLast(probe) != (uint32)last)
			{
				printf("bitops_mismatch=%u\n", sample);
				cerrarTablas();
				return 1;
			}
		}
		printf("bitops_samples=100000\n");
	}
	else if (strcmp(argv[1], "pin_clear") == 0)
	{
		unsigned index, dirty = 0;

		for (index = 0; index < 64; index++)
			mapaRayosClavada[index] = ~(uint64)0;
		load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		full_uncached();
		for (index = 0; index < 64; index++)
			dirty += mapaRayosClavada[index] != 0;
		printf("dirty_pin_rays=%u\n", dirty);
	}
	else if (strcmp(argv[1], "print_bits") == 0)
	{
		imprimirBitTablero(0x8000000000000001ULL);
		printf("printed_bitboard=1\n");
	}
	else if (strcmp(argv[1], "tt_equal_depth") == 0)
	{
		int flag = 0;
		MOVIMIENTO mov = 0;
		int staticEval = INT_MAX;
		int score;

		load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		limpiarTablasHash();
		agregarMovTablaHash(4, 2, 123, BANDERA_HASH_EXACTO, 0, 17);
		score = verificarTablaHash(-1000, 1000, 2, 4, &flag, &mov,
					  &staticEval);
		printf("score=%d flag=%d expected_flag=%d static_eval=%d\n",
		       score, flag, BANDERA_HASH_EXACTO, staticEval);
	}
	else if (strcmp(argv[1], "tt_context") == 0)
	{
		int flag = 0;
		MOVIMIENTO mov = 0;
		int staticEval = INT_MAX;

		load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		limpiarTablasHash();
		agregarMovTablaHash(4, 2, 123, BANDERA_HASH_EXACTO, 0, 17);
		juego.reglaCincuentaMov = 99;
		verificarTablaHash(-1000, 1000, 2, 4, &flag, &mov, &staticEval);
		printf("clock_miss=%d ", flag == BANDERA_HASH_VACIO);
		juego.reglaCincuentaMov = 0;
		juego.ENROQUEB = ENROQUE_OO;
		verificarTablaHash(-1000, 1000, 2, 4, &flag, &mov, &staticEval);
		printf("history_miss=%d size=%zu\n",
		       flag == BANDERA_HASH_VACIO,
		       sizeof(REGISTRO_TABLA_HASH));
	}
	else if (strcmp(argv[1], "tt_mate") == 0)
	{
		int positive = VALOR_JAQUE_MATE - 10;
		int negative = -VALOR_JAQUE_MATE + 10;
		int positiveRead =
			puntajeHashAlLeer(puntajeHashAlGuardar(positive, 7), 3);
		int negativeRead =
			puntajeHashAlLeer(puntajeHashAlGuardar(negative, 7), 3);
		int ordinary =
			puntajeHashAlLeer(puntajeHashAlGuardar(123, 7), 3);

		printf("positive_delta=%d negative_delta=%d ordinary=%d\n",
		       positiveRead - positive, negativeRead - negative, ordinary);
	}
	else if (strcmp(argv[1], "eval_state") == 0)
	{
		static const char *positions[] = {
			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
			"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
			"4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1",
			"4k3/P7/8/8/8/8/7p/4K3 w - - 0 1"
		};
		unsigned position, checked = 0;

		for (position = 0;
		     position < sizeof(positions) / sizeof(positions[0]);
		     position++)
		{
			ESTADO_EVALUACION initial;
			unsigned index, end;

			load_fen(positions[position]);
			initial = juego.estadoEvaluacion;
			end = generarTodosMov(0);
			for (index = 0; index < end; index++)
			{
				MOVIMIENTO mov = juego.Buffer_MOV[index];
				ESTADO_EVALUACION incremental;

				hacerMovimiento(mov);
				incremental = juego.estadoEvaluacion;
				reconstruirEstadoEvaluacion();
				if (memcmp(&incremental, &juego.estadoEvaluacion,
					   sizeof(incremental)) != 0)
				{
					printf("eval_state_mismatch=%u:%u\n",
					       position, index);
					cerrarTablas();
					return 1;
				}
				desHacerMovimiento(mov);
				if (memcmp(&initial, &juego.estadoEvaluacion,
					   sizeof(initial)) != 0)
				{
					printf("eval_state_undo_mismatch=%u:%u\n",
					       position, index);
					cerrarTablas();
					return 1;
				}
				checked++;
			}
		}
		printf("eval_state_transitions=%u\n", checked);
	}
	else if (strcmp(argv[1], "pawn_shelter") == 0)
	{
		int expected[2][8];

		load_fen("7k/5pp1/8/3P4/2P5/8/7P/K7 w - - 0 1");
		limpiarTablasHash();
		full_uncached();
		memcpy(expected, peonDefectos, sizeof(expected));
		memset(peonDefectos, 0x55, sizeof(peonDefectos));
		full_uncached();
		printf("shelter_restored=%d pawn_hits=%llu\n",
		       memcmp(expected, peonDefectos, sizeof(expected)) == 0,
		       (unsigned long long)aciertosHashPeones);
	}
	else if (strcmp(argv[1], "material_cache") == 0)
	{
		uint64 hitsBefore;
		int first, second;

		load_fen("r2q1rk1/pp2bppp/2n1pn2/2pp4/3P4/2PBPN2/PPQ2PPP/R1B1K2R w KQ - 4 10");
		limpiarTablasHash();
		hitsBefore = aciertosHashMaterial;
		first = full_uncached();
		second = full_uncached();
		printf("same_score=%d material_hits=%llu\n",
		       first == second,
		       (unsigned long long)(aciertosHashMaterial - hitsBefore));
	}
	else if (strcmp(argv[1], "thread_eval") == 0)
	{
		static const char *positions[] = {
			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
			"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
			"7k/8/8/8/8/8/8/KQ6 w - - 0 1"
		};
		int first[3];
		int second[3];
		unsigned index;
		int same = 1;

		for (index = 0; index < 3; index++)
		{
			hiloActual = &hilosBusqueda[0];
			load_fen(positions[index]);
			first[index] = full_uncached();
			hilosBusqueda[1] = hilosBusqueda[0];
			hiloActual = &hilosBusqueda[1];
			second[index] = full_uncached();
			same = same && first[index] == second[index];
		}
		hiloActual = &hilosBusqueda[0];
		printf("same=%d workers=2 scores=%d,%d,%d\n",
		       same, first[0], first[1], first[2]);
	}
	else if (strcmp(argv[1], "candidate_local") == 0)
	{
		load_fen("4k3/8/8/2P5/2P5/8/3P4/4K3 w - - 0 1");
		full_uncached();
		printf("blocked=%d ", !!(peonesCandidatos[BLANCO] & BITSET[26]));
		load_fen("4k3/8/3p4/8/2P5/8/3P4/4K3 w - - 0 1");
		full_uncached();
		printf("candidate=%d\n", !!(peonesCandidatos[BLANCO] & BITSET[26]));
	}
	else if (strcmp(argv[1], "qsearch_static") == 0)
	{
		HASH_EVAL *saved = hash_eval;
		int full;
		int score;
		uint64 evalsCold;
		uint64 evalsWarm;

		load_fen("7k/8/8/8/8/8/8/KQ6 w - - 0 1");
		hash_eval = NULL;
		permitirLazyEval = FALSO;
		full = evaluacionTablero(-INFINITO, INFINITO);
		permitirLazyEval = VERDADERO;
		limpiarAntesDeBusqueda();
		limpiarTablasHash();
		tiempoVencido = FALSO;
		contadorDescendente = INT_MAX;
		esUsoTablaHash = FALSO;
		score = busquedadTranquilidad(0, 55, 56);
		evalsCold = llamadasEval;
		esUsoTablaHash = VERDADERO;
		limpiarAntesDeBusqueda();
		limpiarTablasHash();
		agregarMovTablaHash(4, 0, 123, BANDERA_HASH_EXACTO, 0, full);
		score = busquedadTranquilidad(0, 55, 56);
		evalsWarm = llamadasEval;
		hash_eval = saved;
		printf("standpat=%d full=%d equal=%d reused=%d\n",
		       score, full, score == full, evalsWarm < evalsCold);
	}
	else if (strcmp(argv[1], "qsearch_check") == 0)
	{
		int score;

		load_fen("8/8/8/8/8/k7/r7/K7 w - - 0 1");
		limpiarAntesDeBusqueda();
		tiempoVencido = FALSO;
		contadorDescendente = INT_MAX;
		score = busquedadTranquilidad(0, -INFINITO, INFINITO);
		printf("evals_in_check=%llu evals=%llu score=%d\n",
		       (unsigned long long)llamadasEvalEnJaque,
		       (unsigned long long)llamadasEval,
		       score);
	}
	else if (strcmp(argv[1], "tt_epoch") == 0)
	{
		int flag = 0;
		int staticEval = 0;
		MOVIMIENTO mov = 0;

		load_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		limpiarTablasHash();
		agregarMovTablaHash(4, 2, 123, BANDERA_HASH_EXACTO, 0, 17);
		limpiarTablasHash();
		verificarTablaHash(-1000, 1000, 2, 4, &flag, &mov, &staticEval);
		printf("epoch_miss=%d static_unknown=%d size=%zu\n",
		       flag == BANDERA_HASH_VACIO, staticEval == INT_MAX,
		       sizeof(REGISTRO_TABLA_HASH));
	}
	else
	{
		cerrarTablas();
		return 2;
	}
	cerrarTablas();
	return 0;
}
