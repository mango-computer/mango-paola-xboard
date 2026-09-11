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

/*
	Llave Hash de 26 bits = posiciones 67.108.863 * (24 Bytes del Registro Tabla Hash) = 1536 MB
	Llave Hash de 25 bits = posiciones 33.554.431 * (24 Bytes del Registro Tabla Hash) =  768 MB
	Llave Hash de 24 bits = posiciones 16.777.215 * (24 Bytes del Registro Tabla Hash) =  384 MB
	Llave Hash de 23 bits = posiciones  8.388.607 * (24 Bytes del Registro Tabla Hash) =  192 MB
	Llave Hash de 22 bits = posiciones  4.194.303 * (24 Bytes del Registro Tabla Hash) =   96 MB
	Llave Hash de 21 bits = posiciones  2.097.151 * (24 Bytes del Registro Tabla Hash) =   48 MB
*/


#ifndef HASH_C
#define HASH_C

#ifdef MANGO_SMP
static pthread_mutex_t mutexTablaHash = PTHREAD_MUTEX_INITIALIZER;
#define BLOQUEAR_TT() do { \
	if (esBusquedaParalela) \
		pthread_mutex_lock(&mutexTablaHash); \
} while (0)
#define DESBLOQUEAR_TT() do { \
	if (esBusquedaParalela) \
		pthread_mutex_unlock(&mutexTablaHash); \
} while (0)
#else
#define BLOQUEAR_TT() ((void)0)
#define DESBLOQUEAR_TT() ((void)0)
#endif

_Static_assert(sizeof(REGISTRO_TABLA_HASH) == 32,
	       "La entrada TT debe conservar el layout de 32 bytes");

void iniciarHash()
{
	// Inicializar todos los números de 64 bits al azar

	int i;
//	time_t now;

//	srand((unsigned int)time(&now));

	for (i = 0; i < 64; i++)
	{
		arrayHash.ep[i] = enpassant_random[i];//rand64();
		arrayHash.llaves[i][PEON_BLANCO] = randoms[BLANCO][1][i];
		arrayHash.llaves[i][CABALLO_BLANCO] = randoms[BLANCO][2][i];
		arrayHash.llaves[i][ALFIL_BLANCO] = randoms[BLANCO][3][i];
		arrayHash.llaves[i][TORRE_BLANCO] = randoms[BLANCO][4][i];
		arrayHash.llaves[i][DAMA_BLANCO] = randoms[BLANCO][5][i];
		arrayHash.llaves[i][REY_BLANCO] = randoms[BLANCO][6][i];
		arrayHash.llaves[i][PEON_NEGRO] = randoms[NEGRO][1][i];
		arrayHash.llaves[i][CABALLO_NEGRO] = randoms[NEGRO][2][i];
		arrayHash.llaves[i][ALFIL_NEGRO] = randoms[NEGRO][3][i];
		arrayHash.llaves[i][TORRE_NEGRO] = randoms[NEGRO][4][i];
		arrayHash.llaves[i][DAMA_NEGRO] = randoms[NEGRO][5][i];
		arrayHash.llaves[i][REY_NEGRO] = randoms[NEGRO][6][i];
	}

	arrayHash.lado 	= 0x0002000204081abcull;//rand64();
	arrayHash.OOB 	= castle_random[BLANCO][0]; //rand64();
	arrayHash.OOOB 	= castle_random[BLANCO][1]; //rand64();
	arrayHash.OON 	= castle_random[NEGRO][0];  //rand64();
	arrayHash.OOON 	= castle_random[NEGRO][1];  //rand64();

}

uint64 rand64()
{
	return rand()^((uint64)rand()<<15)^((uint64)rand()<<30)^((uint64)rand()<<45)^((uint64)rand()<<60);
}

void recalcularLlavePeones(void)
{
	uint64 peones;
	int escaque;

	juego.llavePeones = 0;
	peones = juego.tablero[BLANCO][PEON];
	while (peones)
	{
		escaque = bitScanForwardBruijn(peones);
		juego.llavePeones ^= arrayHash.llaves[escaque][PEON_BLANCO];
		peones ^= BITSET[escaque];
	}
	peones = juego.tablero[NEGRO][PEON];
	while (peones)
	{
		escaque = bitScanForwardBruijn(peones);
		juego.llavePeones ^= arrayHash.llaves[escaque][PEON_NEGRO];
		peones ^= BITSET[escaque];
	}
}

static uint64 llaveEvaluacion(void)
{
	uint64 llave = juego.llaveHash;

	/* El hecho histórico de haber enrocado afecta evalRey pero no la llave UCI. */
	if (juego.ENROQUEB == ENROQUE_OO)
		llave ^= 0x9e3779b97f4a7c15ull;
	else if (juego.ENROQUEB == ENROQUE_OOO)
		llave ^= 0xbf58476d1ce4e5b9ull;
	if (juego.ENROQUEN == ENROQUE_OO)
		llave ^= 0x94d049bb133111ebull;
	else if (juego.ENROQUEN == ENROQUE_OOO)
		llave ^= 0xd6e8feb86659fd93ull;
	return llave;
}

void agregarEvalTablaHash(int valor)
{
	uint64 llave;
	if (esBusquedaParalela || !hash_eval)
		return;
	llave = llaveEvaluacion();
	HASH_EVAL *ptabla 	= hash_eval + (llave & LARGO_HASH_EVAL);

	ptabla->id 		= llave;
	ptabla->valor		= valor;
	ptabla->generacion	= generacionHash;
}

BOOLEANO verificarEvalTablaHash(int *valor)
{
	uint64 llave;
	if (esBusquedaParalela || !hash_eval)
		return FALSO;
	llave = llaveEvaluacion();
	HASH_EVAL *ptabla 	= hash_eval + (llave & LARGO_HASH_EVAL);
	BOOLEANO esEncontrado 	= FALSO;

	if (ptabla->generacion == generacionHash && ptabla->id == llave)
	{
		*valor 		= ptabla->valor;
		esEncontrado 	= VERDADERO;
	}

	return esEncontrado;	
}

static HASH_PEONES *entradaHashPeones(void)
{
	return hash_peones + (juego.llavePeones & LARGO_HASH_PEONES);
}

BOOLEANO cargarHashPeones(void)
{
	HASH_PEONES *entrada;
	int color;

	consultasHashPeones++;
	if (esBusquedaParalela || !hash_peones)
		return FALSO;
	entrada = entradaHashPeones();
	if (entrada->generacion != generacionHash ||
	    entrada->peonesBlancos != juego.tablero[BLANCO][PEON] ||
	    entrada->peonesNegros != juego.tablero[NEGRO][PEON])
		return FALSO;

	aciertosHashPeones++;
	for (color = BLANCO; color <= NEGRO; color++)
	{
		puntajeEval_m[color] += entrada->puntaje_m[color];
		puntajeEval_f[color] += entrada->puntaje_f[color];
		peonesDebiles[color] = entrada->debiles[color];
		peonesPasados[color] = entrada->pasados[color];
		peonesCandidatos[color] = entrada->candidatos[color];
		peonesMapaFila[color] = entrada->filas[color];
		peonesPasadosMapaFila[color] = entrada->filasPasados[color];
		memcpy(peonDefectos[color], entrada->defectosRey[color],
		       sizeof(entrada->defectosRey[color]));
	}
	return VERDADERO;
}

void guardarHashPeones(const int *puntajeMAnterior, const int *puntajeFAnterior)
{
	HASH_PEONES *entrada;
	int color;

	if (esBusquedaParalela || !hash_peones)
		return;
	entrada = entradaHashPeones();
	entrada->peonesBlancos = juego.tablero[BLANCO][PEON];
	entrada->peonesNegros = juego.tablero[NEGRO][PEON];
	for (color = BLANCO; color <= NEGRO; color++)
	{
		entrada->puntaje_m[color] = puntajeEval_m[color] - puntajeMAnterior[color];
		entrada->puntaje_f[color] = puntajeEval_f[color] - puntajeFAnterior[color];
		entrada->debiles[color] = peonesDebiles[color];
		entrada->pasados[color] = peonesPasados[color];
		entrada->candidatos[color] = peonesCandidatos[color];
		entrada->filas[color] = peonesMapaFila[color];
		entrada->filasPasados[color] = peonesPasadosMapaFila[color];
		memcpy(entrada->defectosRey[color], peonDefectos[color],
		       sizeof(entrada->defectosRey[color]));
	}
	entrada->generacion = generacionHash;
}


//**************************************************************************

static int puntajeHashAlGuardar(int valor, int capa)
{
	if (valor > VALOR_ALTO)
		return valor + capa;
	if (valor < VALOR_BAJO)
		return valor - capa;
	return valor;
}

static int puntajeHashAlLeer(int valor, int capa)
{
	if (valor > VALOR_ALTO)
		return valor - capa;
	if (valor < VALOR_BAJO)
		return valor + capa;
	return valor;
}

void agregarMovTablaHash(int profundidad, int capa, int valor, int banderas,
			 MOVIMIENTO mov, int evalEstatico)
{
	if (!tabla_hash || !entradasTablaHash)
		return;
	BLOQUEAR_TT();
	REGISTRO_TABLA_HASH *ptabla 	= tabla_hash + (juego.llaveHash & LARGO_TABLA_HASH);

	if ((ptabla->generacion == generacionHash) &&
	    (ptabla->profundidad > profundidad) &&
	    (ptabla->id == juego.llaveHash) &&
	    (ptabla->reglaCincuentaMov == juego.reglaCincuentaMov) &&
	    (ptabla->enroqueBlanco == juego.ENROQUEB) &&
	    (ptabla->enroqueNegro == juego.ENROQUEN))
	{
		DESBLOQUEAR_TT();
		return;
	}

	valor = puntajeHashAlGuardar(valor, capa);

	ptabla->id 		= (uint64)juego.llaveHash;
	ptabla->profundidad	= profundidad;
	ptabla->generacion	= generacionHash;
	ptabla->reglaCincuentaMov = juego.reglaCincuentaMov;
	ptabla->evalEstatico	= evalEstatico;
	ptabla->puntaje		= valor;
	ptabla->mov		= mov;
	ptabla->banderas	= banderas;
	ptabla->enroqueBlanco	= juego.ENROQUEB;
	ptabla->enroqueNegro	= juego.ENROQUEN;
	DESBLOQUEAR_TT();
}

int verificarTablaHash(int alfa, int beta, int capa, int profundidad,
		       int *banderas, MOVIMIENTO *mov, int *evalEstatico)
{
	if (!tabla_hash || !entradasTablaHash) {
		*banderas = BANDERA_HASH_VACIO;
		*evalEstatico = INT_MAX;
		return 0;
	}
	BLOQUEAR_TT();
	REGISTRO_TABLA_HASH *ptabla = tabla_hash + (juego.llaveHash & LARGO_TABLA_HASH);

	int valor = 0;
	int flag  = 0; 

	*banderas = BANDERA_HASH_VACIO;
	*evalEstatico = INT_MAX;

	if (ptabla->generacion == generacionHash &&
	    ptabla->id == juego.llaveHash &&
	    ptabla->reglaCincuentaMov == juego.reglaCincuentaMov &&
	    ptabla->enroqueBlanco == juego.ENROQUEB &&
	    ptabla->enroqueNegro == juego.ENROQUEN)
	{
		valor 		= ptabla->puntaje;
		flag	 	= ptabla->banderas;
		*mov		= ptabla->mov;
		*evalEstatico	= ptabla->evalEstatico;

		valor = puntajeHashAlLeer(valor, capa);

		if ((ptabla->profundidad >= profundidad-REDUCCION_MOV_NULL) && (valor < beta) && (flag == BANDERA_HASH_ARRIBA))
		{
			*banderas = BANDERA_HASH_EVITAR_NULL;
		}

		if (ptabla->profundidad >= profundidad)
		{
			if ((flag == BANDERA_HASH_ARRIBA) && (valor <=alfa))
			{
				*banderas = BANDERA_HASH_ARRIBA;
				DESBLOQUEAR_TT();
				return alfa;
			}

			if ((flag == BANDERA_HASH_ABAJO) && (valor >= beta))
			{
				*banderas = BANDERA_HASH_ABAJO;
				DESBLOQUEAR_TT();
				return beta;
			}

			if (flag == BANDERA_HASH_EXACTO)
			{
				*banderas = BANDERA_HASH_EXACTO;
				DESBLOQUEAR_TT();
				return valor;
			}
		}
	}

	DESBLOQUEAR_TT();
	return 0;	
}


#endif
