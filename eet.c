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

#ifndef C
#define C

typedef struct
{
	uint64 piezas[2][6];
	uint64 ocupados;
} POSICION_EET;

static const int ORDEN_PIEZAS_EET[6] = {PEON, CABALLO, ALFIL, TORRE, DAMA, REY};
static const int VALOR_TIPO_EET[6] = {
	VALOR_PEON, VALOR_ALFIL, VALOR_CABALLO, VALOR_TORRE, VALOR_DAMA, VALOR_REY
};

static int tipoPiezaEET(PIEZA pieza)
{
	switch (pieza)
	{
		case PEON_BLANCO:
		case PEON_NEGRO: return PEON;
		case CABALLO_BLANCO:
		case CABALLO_NEGRO: return CABALLO;
		case ALFIL_BLANCO:
		case ALFIL_NEGRO: return ALFIL;
		case TORRE_BLANCO:
		case TORRE_NEGRO: return TORRE;
		case DAMA_BLANCO:
		case DAMA_NEGRO: return DAMA;
		case REY_BLANCO:
		case REY_NEGRO: return REY;
		default: return -1;
	}
}

static uint64 ataquesTorreEET(int escaque, uint64 ocupados)
{
	uint64 fila = RANK_ATTACKS[escaque][
		(int)((ocupados & RANKMASK[escaque]) >> RANKSHIFT[escaque])];
	uint64 columna = FILE_ATTACKS[escaque][
		((ocupados & FILEMASK[escaque]) * FILEMAGIC[escaque]) >> 57];
	return fila | columna;
}

static uint64 ataquesAlfilEET(int escaque, uint64 ocupados)
{
	uint64 diagonalA8H1 = DIAGA8H1_ATTACKS[escaque][
		((ocupados & DIAGA8H1MASK[escaque]) * DIAGA8H1MAGIC[escaque]) >> 57];
	uint64 diagonalA1H8 = DIAGA1H8_ATTACKS[escaque][
		((ocupados & DIAGA1H8MASK[escaque]) * DIAGA1H8MAGIC[escaque]) >> 57];
	return diagonalA8H1 | diagonalA1H8;
}

static uint64 atacantesEET(const POSICION_EET *posicion, int destino, COLOR color)
{
	uint64 atacantes = 0;

	atacantes |= mascaraCaballo[destino] & posicion->piezas[color][CABALLO];
	atacantes |= mascaraRey[destino] & posicion->piezas[color][REY];
	atacantes |= mascaraCapturarPeon[destino][!color] & posicion->piezas[color][PEON];
	atacantes |= ataquesTorreEET(destino, posicion->ocupados) &
		(posicion->piezas[color][TORRE] | posicion->piezas[color][DAMA]);
	atacantes |= ataquesAlfilEET(destino, posicion->ocupados) &
		(posicion->piezas[color][ALFIL] | posicion->piezas[color][DAMA]);

	return atacantes;
}

static void quitarPiezaEET(POSICION_EET *posicion, COLOR color, int tipo, int escaque)
{
	posicion->piezas[color][tipo] &= ~BITSET[escaque];
	posicion->ocupados &= ~BITSET[escaque];
}

static void agregarPiezaEET(POSICION_EET *posicion, COLOR color, int tipo, int escaque)
{
	posicion->piezas[color][tipo] |= BITSET[escaque];
	posicion->ocupados |= BITSET[escaque];
}

static BOOLEANO capturaLegalEET(const POSICION_EET *posicion, COLOR color)
{
	uint64 rey = posicion->piezas[color][REY];
	int escaqueRey;

	if (!rey) return VERDADERO;
	escaqueRey = bitScanForwardBruijn(rey);
	return atacantesEET(posicion, escaqueRey, !color) == 0;
}

static int mejorGananciaEET(POSICION_EET *posicion, int destino, COLOR color,
			    int tipoPiezaEnDestino, int profundidad)
{
	int mejor = 0;
	int indiceTipo;
	uint64 atacantes;

#ifdef PRUEBAS_HCE
	if (profundidad > profundidadMaxEET) profundidadMaxEET = profundidad;
#endif
	if (profundidad >= 30 || tipoPiezaEnDestino == REY) return 0;
	atacantes = atacantesEET(posicion, destino, color);

	for (indiceTipo = 0; indiceTipo < 6; indiceTipo++)
	{
		int tipoAtacante = ORDEN_PIEZAS_EET[indiceTipo];
		uint64 candidatos = atacantes & posicion->piezas[color][tipoAtacante];

		while (candidatos)
		{
			int origen = bitScanForwardBruijn(candidatos);
			int tiposResultado[4] = {tipoAtacante, 0, 0, 0};
			int cantidadTipos = 1;
			int indiceResultado;

			candidatos &= ~BITSET[origen];
			if (tipoAtacante == PEON &&
			    ((color == BLANCO && RANKS[destino] == 8) ||
			     (color == NEGRO && RANKS[destino] == 1)))
			{
				tiposResultado[0] = DAMA;
				tiposResultado[1] = TORRE;
				tiposResultado[2] = ALFIL;
				tiposResultado[3] = CABALLO;
				cantidadTipos = 4;
			}

			for (indiceResultado = 0; indiceResultado < cantidadTipos; indiceResultado++)
			{
				int tipoResultado = tiposResultado[indiceResultado];
				int promocion = tipoResultado != tipoAtacante ?
					VALOR_TIPO_EET[tipoResultado] - VALOR_PEON : 0;
				int ganancia;
#ifndef EET_APLICAR_DESHACER
				POSICION_EET copia = *posicion;
				POSICION_EET *siguiente = &copia;
#else
				POSICION_EET *siguiente = posicion;
#endif

				quitarPiezaEET(siguiente, color, tipoAtacante, origen);
				quitarPiezaEET(siguiente, !color, tipoPiezaEnDestino, destino);
				agregarPiezaEET(siguiente, color, tipoResultado, destino);
				if (capturaLegalEET(siguiente, color))
				{
#ifdef PRUEBAS_HCE
					ramasEETCompleto++;
#endif
					ganancia = VALOR_TIPO_EET[tipoPiezaEnDestino] + promocion -
						mejorGananciaEET(siguiente, destino, !color,
								 tipoResultado, profundidad + 1);
				} else {
					ganancia = -INFINITO;
				}
#ifdef EET_APLICAR_DESHACER
				quitarPiezaEET(posicion, color, tipoResultado, destino);
				agregarPiezaEET(posicion, !color, tipoPiezaEnDestino, destino);
				agregarPiezaEET(posicion, color, tipoAtacante, origen);
#endif
				if (ganancia > mejor) mejor = ganancia;
			}
		}
	}

	return mejor;
}

#ifdef EET_BASELINE_HEURISTICA
static int EETHeuristicaAnterior(MOVIMIENTO mov)
{
	int origen, destino, valorPiezaPivo, dirAtaque;
	int balanceMaterial = 0;
	uint64 atacantes, atacantesNegado;
	COLOR colorLadotmp, colorEvaluar;
	BOOLEANO esFilaPromocion;
	PIEZA pieza, victima;
	BOOLEANO esMalaCaptura;

	pieza = OBT_MOV_PIEZA(mov);
	colorEvaluar = colorLadotmp = OBT_COLOR_PIEZA(pieza);
	destino = OBT_MOV_DESTINO(mov);
	atacantes = obtAtacantesDeEscaque(destino);
	origen = OBT_MOV_ORIGEN(mov);
	esMalaCaptura = FALSO;
	esFilaPromocion = (((RANKS[destino] == 8) || (RANKS[destino] == 1)) &&
				  (pieza == PEON_BLANCO || pieza == PEON_NEGRO));
	atacantesNegado = ~0;
	victima = ES_MOV_CAPTURA(mov) ? OBT_MOV_CAPTURA(mov) : ESCAQUES[destino];

	atacantes &= ~BITSET[origen];
	atacantesNegado &= ~BITSET[origen];

	if (esFilaPromocion)
	{
		balanceMaterial = VALORPIEZA[OBT_MOV_PROMOCION(mov)] - VALOR_PEON;
		valorPiezaPivo = balanceMaterial;
	} else {
		valorPiezaPivo = VALORPIEZA[pieza];
	}
	balanceMaterial = VALORPIEZA[victima] + balanceMaterial;
	if ((valorPiezaPivo > VALORPIEZA[victima]) && !esFilaPromocion)
		esMalaCaptura = VERDADERO;

	if (!atacantes) return balanceMaterial;
	if (colorLadotmp)
	{
		if (!(atacantes & juego.blancos)) return balanceMaterial;
	} else if (!(atacantes & juego.negros)) {
		return balanceMaterial;
	}
	if (colorLadotmp) balanceMaterial = -balanceMaterial;

	dirAtaque = DIRATAQUEVIENE[destino][origen];
	if (dirAtaque)
		atacantes = obtProximosAtacantes(atacantes, atacantesNegado, destino, dirAtaque);
	colorLadotmp = !colorLadotmp;

	while (atacantes)
	{
		if (colorLadotmp)
		{
			if ((RANKS[destino] != 1) && (juego.tablero[NEGRO][PEON] & atacantes))
			{
				origen = bitScanForwardBruijn(juego.tablero[NEGRO][PEON] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = VALOR_PEON;
			} else if (juego.tablero[NEGRO][CABALLO] & atacantes) {
				origen = bitScanForwardBruijn(juego.tablero[NEGRO][CABALLO] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = VALOR_CABALLO;
			} else if (juego.tablero[NEGRO][ALFIL] & atacantes) {
				origen = bitScanForwardBruijn(juego.tablero[NEGRO][ALFIL] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = VALOR_ALFIL;
			} else if (juego.tablero[NEGRO][TORRE] & atacantes) {
				origen = bitScanForwardBruijn(juego.tablero[NEGRO][TORRE] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = VALOR_TORRE;
			} else if ((RANKS[destino] == 1) &&
				   (juego.tablero[NEGRO][PEON] & atacantes)) {
				origen = bitScanForwardBruijn(juego.tablero[NEGRO][PEON] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = VALOR_DAMA;
			} else if (juego.tablero[NEGRO][DAMA] & atacantes) {
				origen = bitScanForwardBruijn(juego.tablero[NEGRO][DAMA] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = VALOR_DAMA;
			} else if ((juego.tablero[NEGRO][REY] & atacantes) &&
				   !(atacantes & juego.blancos)) {
				origen = bitScanForwardBruijn(juego.tablero[NEGRO][REY]);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = 0;
			} else {
				break;
			}
		} else {
			if ((RANKS[destino] != 8) && (juego.tablero[BLANCO][PEON] & atacantes))
			{
				origen = bitScanForwardBruijn(juego.tablero[BLANCO][PEON] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo = VALOR_PEON;
			} else if (juego.tablero[BLANCO][CABALLO] & atacantes) {
				origen = bitScanForwardBruijn(juego.tablero[BLANCO][CABALLO] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo = VALOR_CABALLO;
			} else if (juego.tablero[BLANCO][ALFIL] & atacantes) {
				origen = bitScanForwardBruijn(juego.tablero[BLANCO][ALFIL] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo = VALOR_ALFIL;
			} else if (juego.tablero[BLANCO][TORRE] & atacantes) {
				origen = bitScanForwardBruijn(juego.tablero[BLANCO][TORRE] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo = VALOR_TORRE;
			} else if ((RANKS[destino] == 8) &&
				   (juego.tablero[BLANCO][PEON] & atacantes)) {
				origen = bitScanForwardBruijn(juego.tablero[BLANCO][PEON] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo = VALOR_DAMA;
			} else if (juego.tablero[BLANCO][DAMA] & atacantes) {
				origen = bitScanForwardBruijn(juego.tablero[BLANCO][DAMA] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo = VALOR_DAMA;
			} else if ((juego.tablero[BLANCO][REY] & atacantes) &&
				   !(atacantes & juego.negros)) {
				origen = bitScanForwardBruijn(juego.tablero[BLANCO][REY]);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo = 0;
			} else {
				break;
			}
		}

		if (esMalaCaptura) break;
		atacantes &= ~BITSET[origen];
		atacantesNegado &= ~BITSET[origen];
		dirAtaque = DIRATAQUEVIENE[destino][origen];
		if (dirAtaque)
			atacantes = obtProximosAtacantes(
				atacantes, atacantesNegado, destino, dirAtaque);
		colorLadotmp = !colorLadotmp;
	}

	if (colorEvaluar) balanceMaterial = -balanceMaterial;
	return balanceMaterial;
}
#endif

/*
 * Evalua el intercambio con el tablero en el estado ANTERIOR al movimiento.
 * El resultado esta expresado desde la perspectiva del bando que captura.
 */
int EET(MOVIMIENTO mov)
{
#ifdef EET_BASELINE_HEURISTICA
	return EETHeuristicaAnterior(mov);
#else
	POSICION_EET posicion;
	PIEZA pieza = OBT_MOV_PIEZA(mov);
	PIEZA victima = ES_MOV_CAPTURA(mov) ? OBT_MOV_CAPTURA(mov) : VACIO;
	PIEZA promocion = OBT_MOV_PROMOCION(mov);
	COLOR color = OBT_COLOR_PIEZA(pieza);
	int tipoAtacante = tipoPiezaEET(pieza);
	int tipoVictima = tipoPiezaEET(victima);
	int tipoResultado = promocion ? tipoPiezaEET(promocion) : tipoAtacante;
	int origen = OBT_MOV_ORIGEN(mov);
	int destino = OBT_MOV_DESTINO(mov);
	int escaqueVictima = destino;
	int gananciaInicial = 0;
	int c, t;

#ifdef PRUEBAS_HCE
	llamadasEETCompleto++;
#endif
	for (c = BLANCO; c <= NEGRO; c++)
		for (t = PEON; t <= REY; t++)
			posicion.piezas[c][t] = juego.tablero[c][t];
	posicion.ocupados = juego.ocupados;

	if (tipoAtacante < 0 || tipoResultado < 0) return 0;

	if (tipoVictima >= 0)
	{
		gananciaInicial += VALOR_TIPO_EET[tipoVictima];
		if (ES_MOV_CAPT_PEON_PASO(mov))
			escaqueVictima = destino + (color == BLANCO ? -8 : 8);
		quitarPiezaEET(&posicion, !color, tipoVictima, escaqueVictima);
	}

	quitarPiezaEET(&posicion, color, tipoAtacante, origen);
	if (tipoResultado != tipoAtacante)
		gananciaInicial += VALOR_TIPO_EET[tipoResultado] - VALOR_PEON;
	agregarPiezaEET(&posicion, color, tipoResultado, destino);

	return gananciaInicial -
		mejorGananciaEET(&posicion, destino, !color, tipoResultado, 0);
#endif
}

static BOOLEANO gananciaAlMenosEET(POSICION_EET *posicion, int destino, COLOR color,
				    int tipoPiezaEnDestino, int profundidad, int umbral)
{
	int indiceTipo;
	uint64 atacantes;

#ifdef PRUEBAS_HCE
	if (profundidad > profundidadMaxEET) profundidadMaxEET = profundidad;
#endif
	if (umbral <= 0) return VERDADERO;
	if (profundidad >= 30 || tipoPiezaEnDestino == REY) return FALSO;
	atacantes = atacantesEET(posicion, destino, color);

	for (indiceTipo = 0; indiceTipo < 6; indiceTipo++)
	{
		int tipoAtacante = ORDEN_PIEZAS_EET[indiceTipo];
		uint64 candidatos = atacantes & posicion->piezas[color][tipoAtacante];

		while (candidatos)
		{
			int origen = bitScanForwardBruijn(candidatos);
			int tiposResultado[4] = {tipoAtacante, 0, 0, 0};
			int cantidadTipos = 1;
			int indiceResultado;

			candidatos &= ~BITSET[origen];
			if (tipoAtacante == PEON &&
			    ((color == BLANCO && RANKS[destino] == 8) ||
			     (color == NEGRO && RANKS[destino] == 1)))
			{
				tiposResultado[0] = DAMA;
				tiposResultado[1] = TORRE;
				tiposResultado[2] = ALFIL;
				tiposResultado[3] = CABALLO;
				cantidadTipos = 4;
			}

			for (indiceResultado = 0; indiceResultado < cantidadTipos; indiceResultado++)
			{
				int tipoResultado = tiposResultado[indiceResultado];
				int promocion = tipoResultado != tipoAtacante ?
					VALOR_TIPO_EET[tipoResultado] - VALOR_PEON : 0;
				int ganancia = VALOR_TIPO_EET[tipoPiezaEnDestino] + promocion;
				BOOLEANO alcanza = FALSO;
				if (ganancia < umbral) continue;
#ifndef EET_APLICAR_DESHACER
				POSICION_EET copia = *posicion;
				POSICION_EET *siguiente = &copia;
#else
				POSICION_EET *siguiente = posicion;
#endif

				quitarPiezaEET(siguiente, color, tipoAtacante, origen);
				quitarPiezaEET(siguiente, !color, tipoPiezaEnDestino, destino);
				agregarPiezaEET(siguiente, color, tipoResultado, destino);
				if (capturaLegalEET(siguiente, color))
				{
#ifdef PRUEBAS_HCE
					ramasEETUmbral++;
#endif
					alcanza = !gananciaAlMenosEET(
						siguiente, destino, !color, tipoResultado,
						profundidad + 1, ganancia - umbral + 1);
				}
#ifdef EET_APLICAR_DESHACER
				quitarPiezaEET(posicion, color, tipoResultado, destino);
				agregarPiezaEET(posicion, !color, tipoPiezaEnDestino, destino);
				agregarPiezaEET(posicion, color, tipoAtacante, origen);
#endif

				if (alcanza)
				{
#ifdef PRUEBAS_HCE
					cortesEETUmbral++;
#endif
					return VERDADERO;
				}
			}
		}
	}

	return FALSO;
}

BOOLEANO EETSuperaUmbral(MOVIMIENTO mov, int umbral)
{
#if defined(EET_UMBRAL_COMPLETO) || defined(EET_BASELINE_HEURISTICA)
	return EET(mov) >= umbral;
#else
	POSICION_EET posicion;
	PIEZA pieza = OBT_MOV_PIEZA(mov);
	PIEZA victima = ES_MOV_CAPTURA(mov) ? OBT_MOV_CAPTURA(mov) : VACIO;
	PIEZA promocion = OBT_MOV_PROMOCION(mov);
	COLOR color = OBT_COLOR_PIEZA(pieza);
	int tipoAtacante = tipoPiezaEET(pieza);
	int tipoVictima = tipoPiezaEET(victima);
	int tipoResultado = promocion ? tipoPiezaEET(promocion) : tipoAtacante;
	int origen = OBT_MOV_ORIGEN(mov);
	int destino = OBT_MOV_DESTINO(mov);
	int escaqueVictima = destino;
	int gananciaInicial = 0;
	int c, t;

#ifdef PRUEBAS_HCE
	llamadasEETUmbral++;
#endif
	for (c = BLANCO; c <= NEGRO; c++)
		for (t = PEON; t <= REY; t++)
			posicion.piezas[c][t] = juego.tablero[c][t];
	posicion.ocupados = juego.ocupados;

	if (tipoAtacante < 0 || tipoResultado < 0) return 0 >= umbral;
	if (tipoVictima >= 0)
	{
		gananciaInicial += VALOR_TIPO_EET[tipoVictima];
		if (ES_MOV_CAPT_PEON_PASO(mov))
			escaqueVictima = destino + (color == BLANCO ? -8 : 8);
		quitarPiezaEET(&posicion, !color, tipoVictima, escaqueVictima);
	}

	quitarPiezaEET(&posicion, color, tipoAtacante, origen);
	if (tipoResultado != tipoAtacante)
		gananciaInicial += VALOR_TIPO_EET[tipoResultado] - VALOR_PEON;
	agregarPiezaEET(&posicion, color, tipoResultado, destino);

	return !gananciaAlMenosEET(&posicion, destino, !color, tipoResultado, 0,
				   gananciaInicial - umbral + 1);
#endif
}

#ifdef PRUEBAS_HCE
void reiniciarEstadisticasEET(void)
{
	llamadasEETCompleto = 0;
	llamadasEETUmbral = 0;
	ramasEETCompleto = 0;
	ramasEETUmbral = 0;
	cortesEETUmbral = 0;
	llamadasEETPOS = 0;
	profundidadMaxEET = 0;
}

static int mejorGananciaEETReferencia(int destino, int profundidad)
{
	MOVIMIENTO movimientos[256];
	uint32 fin;
	int cantidad;
	int i;
	int mejor = 0;

	if (profundidad >= 30) return 0;
	fin = generarMovCapPro(0);
	cantidad = (int)MINIMO(fin, 256);
	for (i = 0; i < cantidad; i++) movimientos[i] = juego.Buffer_MOV[i];

	for (i = 0; i < cantidad; i++)
	{
		MOVIMIENTO mov = movimientos[i];
		int ganancia;
		int respuesta;
		BOOLEANO movimientoIlegal;

		if (OBT_MOV_DESTINO(mov) != destino || !ES_MOV_CAPTURA(mov)) continue;
		ganancia = VALORPIEZA[OBT_MOV_CAPTURA(mov)];
		if (ES_MOV_PROMOCION(mov))
			ganancia += VALORPIEZA[OBT_MOV_PROMOCION(mov)] - VALOR_PEON;

		hacerMovimiento(mov);
		if (juego.colorTurno)
			movimientoIlegal = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
		else
			movimientoIlegal = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);

		if (!movimientoIlegal)
		{
			respuesta = mejorGananciaEETReferencia(destino, profundidad + 1);
			if (ganancia - respuesta > mejor) mejor = ganancia - respuesta;
		}
		desHacerMovimiento(mov);
	}

	return mejor;
}

int EETReferencia(MOVIMIENTO mov)
{
	int destino = OBT_MOV_DESTINO(mov);
	int ganancia = ES_MOV_CAPTURA(mov) ? VALORPIEZA[OBT_MOV_CAPTURA(mov)] : 0;
	int respuesta;
	BOOLEANO movimientoIlegal;

	if (ES_MOV_PROMOCION(mov))
		ganancia += VALORPIEZA[OBT_MOV_PROMOCION(mov)] - VALOR_PEON;

	hacerMovimiento(mov);
	if (juego.colorTurno)
		movimientoIlegal = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
	else
		movimientoIlegal = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);

	if (movimientoIlegal)
	{
		desHacerMovimiento(mov);
		return -VALOR_REY;
	}

	respuesta = mejorGananciaEETReferencia(destino, 0);
	desHacerMovimiento(mov);
	return ganancia - respuesta;
}
#endif

int EETPOS(uint8 origen, COLOR colorPrimerTurno)
{

	int valorPiezaPivo, dirAtaque;
	uint8 destino; 	
	int balanceMaterial;
	uint64 atacantes, atacantesNegado;
	COLOR colorLadotmp, colorPieza;
	PIEZA pieza;

#ifdef PRUEBAS_HCE
	llamadasEETPOS++;
#endif
	if ((ESCAQUES[origen]==VACIO) || (ESCAQUES[origen]==REY_BLANCO) || (ESCAQUES[origen]==REY_NEGRO)) return 0;

	atacantes 	= obtAtacantesDeEscaque(origen);
	if (!atacantes) return 0;

	pieza		= ESCAQUES[origen];
	colorPieza	= OBT_COLOR_PIEZA(pieza);

	if (colorPieza)
	{
		if (!(atacantes & juego.blancos)) return 0;

	} else {
		if (!(atacantes & juego.negros)) return 0;
	}

	if (colorPrimerTurno == colorPieza) colorPrimerTurno = !colorPrimerTurno;
	
	destino		= origen;
	colorLadotmp 	= colorPrimerTurno;
	atacantesNegado = ~0;	
	valorPiezaPivo  = VALORPIEZA[pieza];
	balanceMaterial = 0;

	while (atacantes)
	{
		if (colorLadotmp)
		{
			if  (juego.tablero[NEGRO][PEON] & atacantes)
			{
				origen = bitScanForwardBruijn(juego.tablero[NEGRO][PEON] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = VALOR_PEON;

			} else if (juego.tablero[NEGRO][CABALLO] & atacantes) {

				origen = bitScanForwardBruijn(juego.tablero[NEGRO][CABALLO] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = VALOR_CABALLO;

			} else if (juego.tablero[NEGRO][ALFIL] & atacantes) {

				origen = bitScanForwardBruijn(juego.tablero[NEGRO][ALFIL] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = VALOR_ALFIL;

			} else if (juego.tablero[NEGRO][TORRE] & atacantes) {

				origen = bitScanForwardBruijn(juego.tablero[NEGRO][TORRE] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = VALOR_TORRE;

			} else if (juego.tablero[NEGRO][DAMA] & atacantes) {

				origen = bitScanForwardBruijn(juego.tablero[NEGRO][DAMA] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = VALOR_DAMA;
	

			} else if ((juego.tablero[NEGRO][REY] & atacantes) && !(atacantes & juego.blancos)) {

				origen = bitScanForwardBruijn(juego.tablero[NEGRO][REY]);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo = 0;

			} else {
				break;
			}
	    	} else	{
			if (juego.tablero[BLANCO][PEON] & atacantes) {

				origen = bitScanForwardBruijn(juego.tablero[BLANCO][PEON] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo   = VALOR_PEON;

			} else if (juego.tablero[BLANCO][CABALLO] & atacantes) {

				origen = bitScanForwardBruijn(juego.tablero[BLANCO][CABALLO] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo   = VALOR_CABALLO;

			} else if (juego.tablero[BLANCO][ALFIL] & atacantes) {

				origen = bitScanForwardBruijn(juego.tablero[BLANCO][ALFIL] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo   = VALOR_ALFIL;

			} else if (juego.tablero[BLANCO][TORRE] & atacantes) {

				origen = bitScanForwardBruijn(juego.tablero[BLANCO][TORRE] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo   = VALOR_TORRE;

			} else if (juego.tablero[BLANCO][DAMA] & atacantes) {

				origen = bitScanForwardBruijn(juego.tablero[BLANCO][DAMA] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo   = VALOR_DAMA;

			} else if ((juego.tablero[BLANCO][REY] & atacantes) && !(atacantes & juego.negros)) {

				origen = bitScanForwardBruijn(juego.tablero[BLANCO][REY]);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo   = 0;

			} else {
				break;
			}
	    	}

		atacantes 	&= ~BITSET[origen];
		atacantesNegado &= ~BITSET[origen];
		dirAtaque 	 = DIRATAQUEVIENE[destino][origen];

		if (dirAtaque) atacantes = obtProximosAtacantes(atacantes, atacantesNegado, destino, dirAtaque);

		colorLadotmp = !colorLadotmp;
	}
	return balanceMaterial;

}

#endif
