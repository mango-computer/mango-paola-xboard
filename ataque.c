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

#ifndef ATAQUE_C
#define ATAQUE_C

BOOLEANO esAtacadoPor(uint64 bitmapDestino, const COLOR lado)
{
       uint64 tempDestino;
       uint64 ataqueDeslizante;
       int to;
 
       tempDestino = bitmapDestino;
       if (lado) // Evaluar posiciones atacadas por lado negro
       {
              while (tempDestino)
              {
                     to = bitScanForwardBruijn(tempDestino);
 
                     if (juego.tablero[NEGRO][PEON]    &  mascaraCapturarPeon[to][BLANCO]) 	return VERDADERO;
                     if (juego.tablero[NEGRO][CABALLO] &  mascaraCaballo[to]) 			return VERDADERO;
                     if (juego.tablero[NEGRO][REY]     &  mascaraRey[to]) 			return VERDADERO;
 
                     // Ataque en Cruz
                     ataqueDeslizante = juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][TORRE];
                     if (ataqueDeslizante)
                     {
                         if (RANK_ATTACKS[to][((juego.ocupados & RANKMASK[to]) >> RANKSHIFT[to])] & ataqueDeslizante) 
				return VERDADERO;
                         if (FILE_ATTACKS[to][((juego.ocupados & FILEMASK[to]) * FILEMAGIC[to]) >> 57] & ataqueDeslizante) 
				return VERDADERO;
                     }
 
                     // Ataque Diagonal
                     ataqueDeslizante = juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][ALFIL];
                     if (ataqueDeslizante)
                     {
                if (DIAGA8H1_ATTACKS[to][((juego.ocupados & DIAGA8H1MASK[to]) * DIAGA8H1MAGIC[to]) >> 57] & ataqueDeslizante)
			return VERDADERO;
                if (DIAGA1H8_ATTACKS[to][((juego.ocupados & DIAGA1H8MASK[to]) * DIAGA1H8MAGIC[to]) >> 57] & ataqueDeslizante)
			return VERDADERO;
                     }
 
                     tempDestino ^= BITSET[to];
              }
       }
       else // Evaluar posiciones atacadas por lado blanco
       {
              while (tempDestino)
              {
                     to = bitScanForwardBruijn(tempDestino);
 
                     if (juego.tablero[BLANCO][PEON]    &  mascaraCapturarPeon[to][NEGRO]) 	return VERDADERO;
                     if (juego.tablero[BLANCO][CABALLO] &  mascaraCaballo[to]) 			return VERDADERO;
                     if (juego.tablero[BLANCO][REY]     &  mascaraRey[to]) 			return VERDADERO;
 
                     // Ataque en Cruz
                     ataqueDeslizante = juego.tablero[BLANCO][DAMA] | juego.tablero[BLANCO][TORRE];
                     if (ataqueDeslizante)
                     {
                         if (RANK_ATTACKS[to][((juego.ocupados & RANKMASK[to]) >> RANKSHIFT[to])] & ataqueDeslizante) 
				return VERDADERO;
                         if (FILE_ATTACKS[to][((juego.ocupados & FILEMASK[to]) * FILEMAGIC[to]) >> 57] & ataqueDeslizante)
				return VERDADERO;
                     }
 
                     // Ataque Diagonal
                     ataqueDeslizante = juego.tablero[BLANCO][DAMA] | juego.tablero[BLANCO][ALFIL];
                     if (ataqueDeslizante)
                     {
                if (DIAGA8H1_ATTACKS[to][((juego.ocupados & DIAGA8H1MASK[to]) * DIAGA8H1MAGIC[to]) >> 57] & ataqueDeslizante)
			return VERDADERO;
                if (DIAGA1H8_ATTACKS[to][((juego.ocupados & DIAGA1H8MASK[to]) * DIAGA1H8MAGIC[to]) >> 57] & ataqueDeslizante)
			return VERDADERO;
                     }
 
                     tempDestino ^= BITSET[to];
              }
       }
       return FALSO;
}

//atacadas sin tomar en cuenta al rey
BOOLEANO esAtacadoPorSR(uint64 bitmapDestino, const COLOR lado)
{
       uint64 tempDestino;
       uint64 ataqueDeslizante;
       int to;
 
       tempDestino = bitmapDestino;
       if (lado) // Evaluar posiciones atacadas por lado negro
       {
              while (tempDestino)
              {
                     to = bitScanForwardBruijn(tempDestino);
 
                     if (juego.tablero[NEGRO][PEON]    &  mascaraCapturarPeon[to][BLANCO]) 	return VERDADERO;
                     if (juego.tablero[NEGRO][CABALLO] &  mascaraCaballo[to]) 			return VERDADERO;
//                     if (juego.tablero[NEGRO][REY]     &  mascaraRey[to]) 			return VERDADERO;
 
                     // Ataque en Cruz
                     ataqueDeslizante = juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][TORRE];
                     if (ataqueDeslizante)
                     {
                         if (RANK_ATTACKS[to][((juego.ocupados & RANKMASK[to]) >> RANKSHIFT[to])] & ataqueDeslizante) 
				return VERDADERO;
                         if (FILE_ATTACKS[to][((juego.ocupados & FILEMASK[to]) * FILEMAGIC[to]) >> 57] & ataqueDeslizante) 
				return VERDADERO;
                     }
 
                     // Ataque Diagonal
                     ataqueDeslizante = juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][ALFIL];
                     if (ataqueDeslizante)
                     {
                if (DIAGA8H1_ATTACKS[to][((juego.ocupados & DIAGA8H1MASK[to]) * DIAGA8H1MAGIC[to]) >> 57] & ataqueDeslizante)
			return VERDADERO;
                if (DIAGA1H8_ATTACKS[to][((juego.ocupados & DIAGA1H8MASK[to]) * DIAGA1H8MAGIC[to]) >> 57] & ataqueDeslizante)
			return VERDADERO;
                     }
 
                     tempDestino ^= BITSET[to];
              }
       }
       else // Evaluar posiciones atacadas por lado blanco
       {
              while (tempDestino)
              {
                     to = bitScanForwardBruijn(tempDestino);
 
                     if (juego.tablero[BLANCO][PEON]    &  mascaraCapturarPeon[to][NEGRO]) 	return VERDADERO;
                     if (juego.tablero[BLANCO][CABALLO] &  mascaraCaballo[to]) 			return VERDADERO;
//                     if (juego.tablero[BLANCO][REY]     &  mascaraRey[to]) 			return VERDADERO;
 
                     // Ataque en Cruz
                     ataqueDeslizante = juego.tablero[BLANCO][DAMA] | juego.tablero[BLANCO][TORRE];
                     if (ataqueDeslizante)
                     {
                         if (RANK_ATTACKS[to][((juego.ocupados & RANKMASK[to]) >> RANKSHIFT[to])] & ataqueDeslizante) 
				return VERDADERO;
                         if (FILE_ATTACKS[to][((juego.ocupados & FILEMASK[to]) * FILEMAGIC[to]) >> 57] & ataqueDeslizante)
				return VERDADERO;
                     }
 
                     // Ataque Diagonal
                     ataqueDeslizante = juego.tablero[BLANCO][DAMA] | juego.tablero[BLANCO][ALFIL];
                     if (ataqueDeslizante)
                     {
                if (DIAGA8H1_ATTACKS[to][((juego.ocupados & DIAGA8H1MASK[to]) * DIAGA8H1MAGIC[to]) >> 57] & ataqueDeslizante)
			return VERDADERO;
                if (DIAGA1H8_ATTACKS[to][((juego.ocupados & DIAGA1H8MASK[to]) * DIAGA1H8MAGIC[to]) >> 57] & ataqueDeslizante)
			return VERDADERO;
                     }
 
                     tempDestino ^= BITSET[to];
              }
       }
       return FALSO;
}


uint64 obtPiezasClavadasPor(uint8 escaque, const COLOR lado, uint64 mapaPosiblesPiezasClavadas, uint64 mapaPosiblesPiezasAtaX,uint64 mapaPosiblesPiezasAtaNoX)
{
	uint64 tempOcupados 	= juego.ocupados;
	uint64 tempDesOcupados 	= juego.desOcupados;
	uint64 tempDestinos 	= juego.destinos;
	uint64 atacantes 	= 0;
	uint64 mapaAtacantes	= 0;
	uint64 clavadas		= 0;
	uint64 piezasEntre	= 0;
	int escaqueAtacantes	= 0;


	if (lado)
	{
		juego.ocupados 		= (juego.tablero[BLANCO][REY] | juego.negros); 	
		juego.desOcupados	= ~juego.ocupados;
		juego.destinos		= (juego.desOcupados | juego.negros);

		// Atacantes y piezas amigas que mueven en Cruz NoX
		mapaAtacantes 	= genTorreAtaqueTablero(escaque, juego); 
		atacantes	= (mapaAtacantes & mapaPosiblesPiezasAtaNoX);

		// Atacantes y piezas amigas que mueven en Equis X
		mapaAtacantes 	= genAlfilAtaqueTablero(escaque, juego); 
		atacantes      |= (mapaAtacantes & mapaPosiblesPiezasAtaX);

		while (atacantes)
		{		
			escaqueAtacantes = bitScanForwardBruijn(atacantes);
			piezasEntre  = (ESCAQUES_INTERMEDIOS[escaque][escaqueAtacantes] & juego.blancos);
			if (cuentaBit(piezasEntre)==1)
			{
				if (piezasEntre & mapaPosiblesPiezasClavadas)
				{
					clavadas |= piezasEntre;
				}		
			}  
			atacantes ^= BITSET[escaqueAtacantes];
		}

	} else {

		juego.ocupados 		= juego.tablero[NEGRO][REY] | juego.blancos; 	
		juego.desOcupados	= ~juego.ocupados;
		juego.destinos		= juego.desOcupados | juego.blancos;

		// Atacantes y piezas amigas que mueven en Cruz NoX
		mapaAtacantes 	= genTorreAtaqueTablero(escaque, juego); 
		atacantes	= (mapaAtacantes & mapaPosiblesPiezasAtaNoX);

		// Atacantes y piezas amigas que mueven en Equis X
		mapaAtacantes 	= genAlfilAtaqueTablero(escaque, juego); 
		atacantes      |= (mapaAtacantes & mapaPosiblesPiezasAtaX);

		while (atacantes)
		{		
			escaqueAtacantes = bitScanForwardBruijn(atacantes);
			piezasEntre = (ESCAQUES_INTERMEDIOS[escaque][escaqueAtacantes] & juego.negros);
			if (cuentaBit(piezasEntre)==1)
			{
				if (piezasEntre & mapaPosiblesPiezasClavadas)
				{
					clavadas |= piezasEntre;
				}		
			}  
			atacantes ^= BITSET[escaqueAtacantes];
		}
	}

	juego.ocupados		= tempOcupados;
	juego.desOcupados	= tempDesOcupados;
	juego.destinos		= tempDestinos;

	return clavadas;
}

uint64 obtProximosAtacantes(uint64 atacantes, uint64 noRemovidos, int destino, int direccion)
{

	uint64 mapaAtacantes;
	int estado;

	switch(direccion)
	{
		case 	ESTE:
			mapaAtacantes = RAYO_E[destino] & ((juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][TORRE] |
							    juego.tablero[BLANCO][DAMA]| juego.tablero[BLANCO][TORRE]) & noRemovidos);

			if (mapaAtacantes)
			{
				estado = (int)((juego.ocupados & noRemovidos & RANKMASK[destino]) >> RANKSHIFT[destino]);
				mapaAtacantes = RANK_ATTACKS[destino][estado] & mapaAtacantes;
				return (atacantes | mapaAtacantes);
			} else {
				return atacantes;
			}
			break;
	
		case	NOR_OESTE:
			mapaAtacantes = RAYO_NO[destino] & ((juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][ALFIL] |
							    juego.tablero[BLANCO][DAMA]| juego.tablero[BLANCO][ALFIL]) & noRemovidos);

			if (mapaAtacantes)
			{
				estado = ((juego.ocupados & noRemovidos & DIAGA8H1MASK[destino]) * DIAGA8H1MAGIC[destino]) >> 57;
				mapaAtacantes = DIAGA8H1_ATTACKS[destino][estado] & mapaAtacantes;
				return (atacantes | mapaAtacantes);
			} else {
				return atacantes;
			}
			break;

		case	NORTE:
			mapaAtacantes = RAYO_N[destino] & ((juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][TORRE] |
							    juego.tablero[BLANCO][DAMA]| juego.tablero[BLANCO][TORRE]) & noRemovidos);

			if (mapaAtacantes)
			{
				estado = ((juego.ocupados & noRemovidos & FILEMASK[destino]) * FILEMAGIC[destino]) >> 57;
				mapaAtacantes = FILE_ATTACKS[destino][estado] & mapaAtacantes;
				return (atacantes | mapaAtacantes);
			} else {
				return atacantes;
			}
			break;

		case	NOR_ESTE:
			mapaAtacantes = RAYO_NE[destino] & ((juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][ALFIL] |
							    juego.tablero[BLANCO][DAMA]| juego.tablero[BLANCO][ALFIL]) & noRemovidos);

			if (mapaAtacantes)
			{
				estado = ((juego.ocupados & noRemovidos & DIAGA1H8MASK[destino]) * DIAGA1H8MAGIC[destino]) >> 57;
				mapaAtacantes = DIAGA1H8_ATTACKS[destino][estado] & mapaAtacantes;
				return (atacantes | mapaAtacantes);
			} else {
				return atacantes;
			}
			break;

		case	OESTE:
			mapaAtacantes = RAYO_O[destino] & ((juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][TORRE] |
							    juego.tablero[BLANCO][DAMA]| juego.tablero[BLANCO][TORRE]) & noRemovidos);

			if (mapaAtacantes)
			{
				estado = (int)((juego.ocupados & noRemovidos & RANKMASK[destino]) >> RANKSHIFT[destino]);
				mapaAtacantes = RANK_ATTACKS[destino][estado] & mapaAtacantes;
				return (atacantes | mapaAtacantes);
			} else {
				return atacantes;
			}
			break;

		case	SUR_ESTE:
			mapaAtacantes = RAYO_SE[destino] & ((juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][ALFIL] |
							    juego.tablero[BLANCO][DAMA]| juego.tablero[BLANCO][ALFIL]) & noRemovidos);

			if (mapaAtacantes)
			{
				estado = ((juego.ocupados & noRemovidos & DIAGA8H1MASK[destino]) * DIAGA8H1MAGIC[destino]) >> 57;
				mapaAtacantes = DIAGA8H1_ATTACKS[destino][estado] & mapaAtacantes;
				return (atacantes | mapaAtacantes);
			} else {
				return atacantes;
			}
			break;

		case	SUR:
			mapaAtacantes = RAYO_S[destino] & ((juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][TORRE] |
							    juego.tablero[BLANCO][DAMA]| juego.tablero[BLANCO][TORRE]) & noRemovidos);

			if (mapaAtacantes)
			{
				estado = ((juego.ocupados & noRemovidos & FILEMASK[destino]) * FILEMAGIC[destino]) >> 57;
				mapaAtacantes = FILE_ATTACKS[destino][estado] & mapaAtacantes;
				return (atacantes | mapaAtacantes);
			} else {
				return atacantes;
			}
			break;

		case	SUR_OESTE:
			mapaAtacantes = RAYO_SO[destino] & ((juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][ALFIL] |
							    juego.tablero[BLANCO][DAMA]| juego.tablero[BLANCO][ALFIL]) & noRemovidos);

			if (mapaAtacantes)
			{
				estado = ((juego.ocupados & noRemovidos & DIAGA1H8MASK[destino]) * DIAGA1H8MAGIC[destino]) >> 57;
				mapaAtacantes = DIAGA1H8_ATTACKS[destino][estado] & mapaAtacantes;
				return (atacantes | mapaAtacantes);
			} else {
				return atacantes;
			}
			break;
	}

	return atacantes;

}

uint64 obtAtacantesDeEscaque(uint8 escaque)
{

	uint64 atacantes, mapaAtacantes;

	// Atacantes y piezas amigas que mueven en Cruz
	mapaAtacantes 	= genTorreMOVAtaqueTablero(escaque, juego); 
	atacantes	= (mapaAtacantes & (juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][TORRE] | 
					    juego.tablero[BLANCO][DAMA] | juego.tablero[BLANCO][TORRE]));

	// Atacantes y piezas amigas que mueven en Equis
	mapaAtacantes 	= genAlfilMOVAtaqueTablero(escaque, juego); 
	atacantes      |= (mapaAtacantes & (juego.tablero[NEGRO][DAMA] | juego.tablero[NEGRO][ALFIL] | 
					    juego.tablero[BLANCO][DAMA] | juego.tablero[BLANCO][ALFIL]));
	
	// Atacantes caballos
	mapaAtacantes 	= mascaraCaballo[escaque]; 
	atacantes      |= (mapaAtacantes & (juego.tablero[NEGRO][CABALLO] | juego.tablero[BLANCO][CABALLO]));

	// Atacantes peon negro
	mapaAtacantes 	= mascaraCapturarPeon[escaque][BLANCO]; 
	atacantes      |= (mapaAtacantes & juego.tablero[NEGRO][PEON]);

	// Atacantes peon blanco
	mapaAtacantes 	= mascaraCapturarPeon[escaque][NEGRO]; 
	atacantes      |= (mapaAtacantes & juego.tablero[BLANCO][PEON]);

	// Atacantes Rey
	mapaAtacantes 	= mascaraRey[escaque]; 
	atacantes      |= (mapaAtacantes & (juego.tablero[BLANCO][REY] | juego.tablero[NEGRO][REY]));


	return  atacantes;
}


#endif
