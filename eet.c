/* 
	Mango Paola Ajedrez 1.0
	Licencia: GPLv3   
    	Copyright (c) 2012-2013 - Mango Computer c.a 

    	El Programa Mango Paola Ajedrez ésta basado en la investigación de Jose Andres Morales Linares.

	Nadie debería estar restringido por el software que utilizan. Hay cuatro libertades que cada usuario debe tener:
	* La libertad de usar el programa para cualquier propósito,
	* La libertad de cambiar el software para satisfacer sus necesidades,
	* La libertad de compartir el software con sus amigos y vecinos.
	* La libertad de compartir los cambios que realice.

	Cuando un programa se ofrece a los usuarios todas estas libertades, lo llamamos software libre.

	Winglet es un bitboard de código abierto motor de ajedrez. El programa es software libre. Usted puede redistribuirlo y/o 		modificarlo bajo los términos de la Licencia Pública General de GNU según es publicada por la Free Software Foundation, 	bien de 	la versión 3 de la Licencia, o (a su elección) cualquier versión posterior. El programa se distribuye 		con la esperanza de que 		sea útil, pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de 		COMERCIALIZACIÓN o IDONEIDAD PARA UN PROPÓSITO 		PARTICULAR. Vea la Licencia Pública General de GNU para más 		detalles: http://www.gnu.org/licenses/

	Existen muchos avances en la forma de escribir motores de ajedrez, estos dos sitios web 
	fuerón de mucha ayuda durante el proceso de investigacion y desarrollo. 
	* http://chessprogramming.wikispaces.com
    	* http://www.sluijten.com/winglet/

    	Información de contacto:
	comprasmangocomputer@gmail.com

*/

#ifndef C
#define C
//*
int EET(MOVIMIENTO mov)
{

	int origen, destino, valorPiezaPivo, dirAtaque;
	int balanceMaterial=0;
	uint64 atacantes, atacantesNegado;
	COLOR colorLadotmp, colorEvaluar;
	BOOLEANO esFilaPromocion;
	PIEZA pieza, esMalaCaptura;

	pieza		= OBT_MOV_PIEZA(mov);
	colorEvaluar	= colorLadotmp = OBT_COLOR_PIEZA(pieza);
	destino      	= OBT_MOV_DESTINO(mov);
	atacantes 	= obtAtacantesDeEscaque(destino);
	origen 		= OBT_MOV_ORIGEN(mov);
	esMalaCaptura	= FALSO;
	esFilaPromocion = (((RANKS[destino] == 8) || (RANKS[destino] == 1)) && (pieza==PEON_BLANCO || pieza==PEON_NEGRO));
	atacantesNegado = ~0;	

	atacantes 	^= BITSET[origen];
	atacantesNegado ^= BITSET[origen];

	if (esFilaPromocion) // es una promocion 
	{
		balanceMaterial	 = (VALORPIEZA[OBT_MOV_PROMOCION(mov)] - VALOR_PEON);
		valorPiezaPivo	 = balanceMaterial;
	}  else {

		valorPiezaPivo   = VALORPIEZA[pieza];
	}

	balanceMaterial = (VALORPIEZA[ESCAQUES[destino]] + balanceMaterial); 
	
	if ((valorPiezaPivo > VALORPIEZA[ESCAQUES[destino]]) && !esFilaPromocion) esMalaCaptura = VERDADERO;

	if (!atacantes)
	{
		return balanceMaterial;
	}

	if (colorLadotmp)
	{
		if (!(atacantes & juego.blancos)) return balanceMaterial;

	} else {
		if (!(atacantes & juego.negros)) return balanceMaterial;
	}

	if (colorLadotmp)
	{
		balanceMaterial = -balanceMaterial;
	} 


	dirAtaque = DIRATAQUEVIENE[destino][origen];
	if (dirAtaque) atacantes = obtProximosAtacantes(atacantes, atacantesNegado, destino, dirAtaque);

	colorLadotmp = !colorLadotmp;

	while (atacantes)
	{
		if (colorLadotmp)
		{
			if  ((RANKS[destino] != 1) && (juego.tablero[NEGRO][PEON] & atacantes))
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

			} else if ((RANKS[destino] == 1) && (juego.tablero[NEGRO][PEON] & atacantes)) {

				origen = bitScanForwardBruijn(juego.tablero[NEGRO][PEON] & atacantes);
				balanceMaterial -= valorPiezaPivo;
				valorPiezaPivo   = VALOR_DAMA;

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
			if ((RANKS[destino] != 8) && (juego.tablero[BLANCO][PEON] & atacantes)) {

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

			} else if ((RANKS[destino] == 8) && (juego.tablero[BLANCO][PEON] & atacantes)) {

				origen = bitScanForwardBruijn(juego.tablero[BLANCO][PEON] & atacantes);
				balanceMaterial += valorPiezaPivo;
				valorPiezaPivo   = VALOR_DAMA;

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

		if (esMalaCaptura) break;

		atacantes 	^= BITSET[origen];
		atacantesNegado ^= BITSET[origen];
		dirAtaque 	 = DIRATAQUEVIENE[destino][origen];

		if (dirAtaque) atacantes = obtProximosAtacantes(atacantes, atacantesNegado, destino, dirAtaque);
		colorLadotmp = !colorLadotmp;
	}

	if (colorEvaluar)
	{
		balanceMaterial = -balanceMaterial;
	}

	return balanceMaterial;

}

int EETPOS(uint8 origen, COLOR colorPrimerTurno)
{

	int valorPiezaPivo, dirAtaque;
	uint8 destino; 	
	int balanceMaterial;
	uint64 atacantes, atacantesNegado;
	COLOR colorLadotmp, colorPieza;
	PIEZA pieza;

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

		atacantes 	^= BITSET[origen];
		atacantesNegado ^= BITSET[origen];
		dirAtaque 	 = DIRATAQUEVIENE[destino][origen];

		if (dirAtaque) atacantes = obtProximosAtacantes(atacantes, atacantesNegado, destino, dirAtaque);

		colorLadotmp = !colorLadotmp;
	}
	return balanceMaterial;

}

#endif
