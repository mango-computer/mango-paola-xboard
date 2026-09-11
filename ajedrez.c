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

#ifndef AJEDREZ_C
#define AJEDREZ_C

uint32 generarTodosMov(uint32 indice)
{
	int escaqueOrigen=0, escaqueDestino=0;
	uint64 tempOrigenes=0, tempDestinos=0;
	MOVIMIENTO mov;
	mov ^= mov;

	juego.desOcupados	= ~juego.ocupados;

	if (juego.colorTurno) //Negro
	{
		juego.destinos	= juego.desOcupados | juego.blancos;
	
	} else {
		juego.destinos	= juego.desOcupados | juego.negros;
	}


	if (juego.colorTurno) //Lado Negro
	{
		//	Generación de los movimientos del peón negro
		tempOrigenes = juego.tablero[NEGRO][PEON];
		EST_MOV_PIEZA(mov, PEON_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos = mascaraPeon[escaqueOrigen][NEGRO] & juego.desOcupados;		
			if (RANKS[escaqueOrigen] == 7) 
			{		
				tempDestinos |= (tempDestinos>>8) & juego.desOcupados;			
			}								
			tempDestinos|=mascaraCapturarPeon[escaqueOrigen][NEGRO] & juego.blancos;	

			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);

				if (RANKS[escaqueDestino]==1) // Promoción
				{
					EST_MOV_PROMOCION(mov,DAMA_NEGRO);
					juego.Buffer_MOV[indice++] = mov;
					EST_MOV_PROMOCION(mov,TORRE_NEGRO);
					juego.Buffer_MOV[indice++] = mov;
					EST_MOV_PROMOCION(mov,CABALLO_NEGRO);
					juego.Buffer_MOV[indice++] = mov;
					EST_MOV_PROMOCION(mov,ALFIL_NEGRO);
					juego.Buffer_MOV[indice++] = mov;
					EST_MOV_PROMOCION(mov,VACIO);
				} else {
					juego.Buffer_MOV[indice++] = mov; 
				}
				tempDestinos ^= BITSET[escaqueDestino];
			}

			if (juego.posPeonPaso != SIN_POS_VALIDA)  // Captura de peón al paso
			{
				if (mascaraCapturarPeon[escaqueOrigen][NEGRO] & BITSET[juego.posPeonPaso])
				{
					 // no estamos en un movimiento nulo
					if (RANKBIT[3] & BITSET[juego.posPeonPaso])
					{
						EST_MOV_CAPTURA(mov, PEON_BLANCO);
						EST_MOV_DESTINO(mov, juego.posPeonPaso);
						EST_MOV_CODIGO(mov, COD_MOV_CAPTURA_PEON_PASO);
						juego.Buffer_MOV[indice++] = mov;
					}
				}
			}
			EST_MOV_PROMOCION(mov, VACIO);
			EST_MOV_CODIGO(mov, VACIO);
			tempOrigenes ^= BITSET[escaqueOrigen];

		}
		
		//	Generación de los movimientos de la torre negra
		tempOrigenes = juego.tablero[NEGRO][TORRE];
		EST_MOV_PIEZA(mov, TORRE_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genTorreAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice++] = mov; 
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos del caballo negro
		tempOrigenes = juego.tablero[NEGRO][CABALLO];
		EST_MOV_PIEZA(mov, CABALLO_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genCaballoAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice++] = mov; 
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos del alfil negro
		tempOrigenes = juego.tablero[NEGRO][ALFIL];
		EST_MOV_PIEZA(mov, ALFIL_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genAlfilAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice++] = mov; 
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos de la dama negra
		tempOrigenes = juego.tablero[NEGRO][DAMA];
		EST_MOV_PIEZA(mov, DAMA_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genDamaAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice++] = mov; 
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos del rey negro
		tempOrigenes = juego.tablero[NEGRO][REY];
		EST_MOV_PIEZA(mov, REY_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genReyAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice++] = mov; 
				tempDestinos ^= BITSET[escaqueDestino];
			}

			if (juego.OON)
			{
				if (!(mascaraEnroque[NEGRO][OO] & juego.ocupados))
				{
					if (!esAtacadoPor(mascaraEnroqueMovRey[NEGRO][OO],BLANCO))
					{
						juego.Buffer_MOV[indice++] = ENROQUE_NEGRO_OO;
					}
				}	
			}

			if (juego.OOON)
			{
				if (!(mascaraEnroque[NEGRO][OOO] & juego.ocupados))
				{
					if (!esAtacadoPor(mascaraEnroqueMovRey[NEGRO][OOO],BLANCO))
					{
						juego.Buffer_MOV[indice++] = ENROQUE_NEGRO_OOO;
					}
				}	
			}
			EST_MOV_PROMOCION(mov, VACIO);
			tempOrigenes ^= BITSET[escaqueOrigen];
		}
	
	} else { // ******* Lado Blanco *********

		//	Generación de los movimientos del peón blanco
		tempOrigenes = juego.tablero[BLANCO][PEON];
		EST_MOV_PIEZA(mov, PEON_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos = mascaraPeon[escaqueOrigen][BLANCO] & juego.desOcupados;		
			if (RANKS[escaqueOrigen] == 2)
			{			
				tempDestinos |= (tempDestinos<<8) & juego.desOcupados;			
			} 
			tempDestinos|=mascaraCapturarPeon[escaqueOrigen][BLANCO] & juego.negros;	

			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);

				if (RANKS[escaqueDestino]==8) // Promoción
				{
					EST_MOV_PROMOCION(mov,DAMA_BLANCO);
					juego.Buffer_MOV[indice++] = mov;
					EST_MOV_PROMOCION(mov,TORRE_BLANCO);
					juego.Buffer_MOV[indice++] = mov;
					EST_MOV_PROMOCION(mov,CABALLO_BLANCO);
					juego.Buffer_MOV[indice++] = mov;
					EST_MOV_PROMOCION(mov,ALFIL_BLANCO);
					juego.Buffer_MOV[indice++] = mov;
					EST_MOV_PROMOCION(mov,VACIO);
				} else {
					juego.Buffer_MOV[indice++] = mov; 
				}

				tempDestinos ^= BITSET[escaqueDestino];
			}

			if (juego.posPeonPaso != SIN_POS_VALIDA) // Captura de peón al paso
			{
				if (mascaraCapturarPeon[escaqueOrigen][BLANCO] & BITSET[juego.posPeonPaso])
				{
					 // no estamos en un movimiento nulo
					if (RANKBIT[6] & BITSET[juego.posPeonPaso])
					{

						EST_MOV_CAPTURA(mov, PEON_NEGRO);
						EST_MOV_DESTINO(mov, juego.posPeonPaso);
						EST_MOV_CODIGO(mov, COD_MOV_CAPTURA_PEON_PASO);
						juego.Buffer_MOV[indice++] = mov;
					}
				}
			}
			EST_MOV_PROMOCION(mov, VACIO);
			EST_MOV_CODIGO(mov, VACIO);
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos de la torre blanca
		tempOrigenes = juego.tablero[BLANCO][TORRE];
		EST_MOV_PIEZA(mov, TORRE_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genTorreAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice++] = mov; 
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos del caballo blanco
		tempOrigenes = juego.tablero[BLANCO][CABALLO];
		EST_MOV_PIEZA(mov, CABALLO_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genCaballoAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice++] = mov; 
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos del alfil blanco
		tempOrigenes = juego.tablero[BLANCO][ALFIL];
		EST_MOV_PIEZA(mov, ALFIL_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genAlfilAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice++] = mov; 
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos de la dama blanca
		tempOrigenes = juego.tablero[BLANCO][DAMA];
		EST_MOV_PIEZA(mov, DAMA_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genDamaAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice++] = mov; 
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos del rey blanco
		tempOrigenes = juego.tablero[BLANCO][REY];
		EST_MOV_PIEZA(mov, REY_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genReyAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice++] = mov; 
				tempDestinos ^= BITSET[escaqueDestino];
			}

			if (juego.OOB)
			{
				if (!(mascaraEnroque[BLANCO][OO] & juego.ocupados))
				{
					if (!esAtacadoPor(mascaraEnroqueMovRey[BLANCO][OO],NEGRO))
					{
						juego.Buffer_MOV[indice++] = ENROQUE_BLANCO_OO;
					}
				}	
			}

			if (juego.OOOB)
			{
				if (!(mascaraEnroque[BLANCO][OOO] & juego.ocupados))
				{
					if (!esAtacadoPor(mascaraEnroqueMovRey[BLANCO][OOO],NEGRO))
					{
						juego.Buffer_MOV[indice++] = ENROQUE_BLANCO_OOO;
					}
				}	
			}
			EST_MOV_PROMOCION(mov, VACIO);
			tempOrigenes ^= BITSET[escaqueOrigen];
		}
	}


	return indice;
}

//******************************************************************************************************************************
//	Genera movimientos seudolegales que solo sean capturas o promociones 
//******************************************************************************************************************************

uint32 generarMovCapPro(uint32 indice)
{
	int escaqueOrigen=0, escaqueDestino=0;
	uint32  primero=indice;
	uint64 tempOrigenes=0, tempDestinos=0;
	MOVIMIENTO mov;
	mov ^= mov;

	juego.desOcupados	= ~juego.ocupados;

	if (juego.colorTurno) //Negro
	{
		juego.destinos	= juego.blancos;
	
	} else {
		juego.destinos	= juego.negros;
	}


	if (juego.colorTurno) //Lado Negro
	{
		//	Generación de los movimientos del peón negro
		tempOrigenes = juego.tablero[NEGRO][PEON];
		EST_MOV_PIEZA(mov, PEON_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos = mascaraCapturarPeon[escaqueOrigen][NEGRO] & juego.blancos;	
			if (RANKS[escaqueOrigen]==2)
			{
				tempDestinos |= mascaraPeon[escaqueOrigen][NEGRO] & juego.desOcupados;
			}

			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);

				if (RANKS[escaqueDestino]==1) // Promoción
				{
					EST_MOV_PROMOCION(mov,DAMA_NEGRO);
					juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
					EST_MOV_PROMOCION(mov,TORRE_NEGRO); 
					juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
					EST_MOV_PROMOCION(mov,CABALLO_NEGRO);
					juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
					EST_MOV_PROMOCION(mov,ALFIL_NEGRO);
					juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
					EST_MOV_PROMOCION(mov,VACIO);
				} else {
					juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
				}
				tempDestinos ^= BITSET[escaqueDestino];
			}

			if (juego.posPeonPaso != SIN_POS_VALIDA) // Captura de peón al paso
			{
				if (mascaraCapturarPeon[escaqueOrigen][NEGRO] & BITSET[juego.posPeonPaso])
				{
					if (RANKBIT[3] & BITSET[juego.posPeonPaso])
					{
						EST_MOV_CAPTURA(mov, PEON_BLANCO);
						EST_MOV_DESTINO(mov, juego.posPeonPaso);
						EST_MOV_CODIGO(mov, COD_MOV_CAPTURA_PEON_PASO);
						juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
					}
				}
			}

			EST_MOV_PROMOCION(mov, VACIO);
			EST_MOV_CODIGO(mov, VACIO);
			tempOrigenes ^= BITSET[escaqueOrigen];

		}

		//	Generación de los movimientos del caballo negro
		tempOrigenes = juego.tablero[NEGRO][CABALLO];
		EST_MOV_PIEZA(mov, CABALLO_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genCaballoAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++; 
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos del alfil negro
		tempOrigenes = juego.tablero[NEGRO][ALFIL];
		EST_MOV_PIEZA(mov, ALFIL_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genAlfilAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos de la torre negra
		tempOrigenes = juego.tablero[NEGRO][TORRE];
		EST_MOV_PIEZA(mov, TORRE_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genTorreAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos de la dama negra
		tempOrigenes = juego.tablero[NEGRO][DAMA];
		EST_MOV_PIEZA(mov, DAMA_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genDamaAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos del rey negro
		tempOrigenes = juego.tablero[NEGRO][REY];
		EST_MOV_PIEZA(mov, REY_NEGRO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genReyAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
				tempDestinos ^= BITSET[escaqueDestino];
			}

			EST_MOV_PROMOCION(mov, VACIO);
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

	} else { // ******* Lado Blanco *********

		//	Generación de los movimientos del peón blanco
		tempOrigenes = juego.tablero[BLANCO][PEON];
		EST_MOV_PIEZA(mov, PEON_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos =mascaraCapturarPeon[escaqueOrigen][BLANCO] & juego.negros;	
			if (RANKS[escaqueOrigen]==7)
			{
				tempDestinos |= mascaraPeon[escaqueOrigen][BLANCO] & juego.desOcupados;
			}

			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);

				if (RANKS[escaqueDestino]==8) // Promoción
				{
					EST_MOV_PROMOCION(mov,DAMA_BLANCO);
					juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
					EST_MOV_PROMOCION(mov,TORRE_BLANCO);
					juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
					EST_MOV_PROMOCION(mov,CABALLO_BLANCO);
					juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
					EST_MOV_PROMOCION(mov,ALFIL_BLANCO);
					juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
					EST_MOV_PROMOCION(mov,VACIO);
				} else {
					juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++; 
				}

				tempDestinos ^= BITSET[escaqueDestino];
			}

			if (juego.posPeonPaso != SIN_POS_VALIDA) // Captura de peón al paso
			{
				if (mascaraCapturarPeon[escaqueOrigen][BLANCO] & BITSET[juego.posPeonPaso])
				{
					if (RANKBIT[6] & BITSET[juego.posPeonPaso])
					{
						EST_MOV_CAPTURA(mov, PEON_NEGRO);
						EST_MOV_DESTINO(mov, juego.posPeonPaso);
						EST_MOV_CODIGO(mov, COD_MOV_CAPTURA_PEON_PASO);
						juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
					}
				}
			}

			EST_MOV_PROMOCION(mov, VACIO);
			EST_MOV_CODIGO(mov, VACIO);
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos del caballo blanco
		tempOrigenes = juego.tablero[BLANCO][CABALLO];
		EST_MOV_PIEZA(mov, CABALLO_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genCaballoAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos del alfil blanco
		tempOrigenes = juego.tablero[BLANCO][ALFIL];
		EST_MOV_PIEZA(mov, ALFIL_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genAlfilAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos de la torre blanca
		tempOrigenes = juego.tablero[BLANCO][TORRE];
		EST_MOV_PIEZA(mov, TORRE_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genTorreAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos de la dama blanca
		tempOrigenes = juego.tablero[BLANCO][DAMA];
		EST_MOV_PIEZA(mov, DAMA_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genDamaAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
				tempDestinos ^= BITSET[escaqueDestino];
			}
			tempOrigenes ^= BITSET[escaqueOrigen];
		}

		//	Generación de los movimientos del rey blanco
		tempOrigenes = juego.tablero[BLANCO][REY];
		EST_MOV_PIEZA(mov, REY_BLANCO);
		while (tempOrigenes)
		{
			escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
			tempDestinos  = genReyAtaqueTablero(escaqueOrigen, juego);
			EST_MOV_ORIGEN(mov, escaqueOrigen);
			while (tempDestinos)
			{
				escaqueDestino = bitScanForwardBruijn(tempDestinos);
				EST_MOV_DESTINO(mov, escaqueDestino);
				EST_MOV_CAPTURA(mov, ESCAQUES[escaqueDestino]);
				juego.Buffer_MOV[indice] = mov; agregarMovCapPro(primero, &indice);indice++;
				tempDestinos ^= BITSET[escaqueDestino];
			}
			EST_MOV_PROMOCION(mov, VACIO);
			tempOrigenes ^= BITSET[escaqueOrigen];
		}
	}


	return indice;
}

//******************************************************************************************************************************
//	Agregar los movimientos seudolegales de captura o promoción   

void agregarMovCapPro(uint32 indexPrimero, uint32 *index)
{
//*
	juego.Buffer_MOV_PONDERACION[*index] = 0;

	if (ES_MOV_CAPTURA(juego.Buffer_MOV[*index]))
	{
		juego.Buffer_MOV_PONDERACION[*index] = 128*VALORPIEZA[OBT_MOV_CAPTURA(juego.Buffer_MOV[*index])] - 
							   VALORPIEZA[OBT_MOV_PIEZA(juego.Buffer_MOV[*index])];	
	} 

	if (ES_MOV_PROMOCION(juego.Buffer_MOV[*index])) 
	{
		juego.Buffer_MOV_PONDERACION[*index] += (VALORPIEZA[OBT_MOV_PROMOCION(juego.Buffer_MOV[*index])]*128-VALOR_PEON);
 	}
//*/

/*
	int V = EET(juego.Buffer_MOV[*index]);

	if (V < 0)
	{
		(*index)--;
		return;
	}

	juego.Buffer_MOV_PONDERACION[*index] = V;
//*/
}


//******************************************************************************************************************************

BOOLEANO esValidoMovUsuario(const char *mov, MOVIMIENTO *m)
{
	uint8 origen, destino;
	int i=0;

	origen  = mov[5]   - 97;
	origen += ((mov[6] - 49) * 8);

	destino  = mov[7]   - 97;
	destino += ((mov[8] - 49) * 8);

	for (i=juego.Buffer_MOV_INDEXCAPAS[0];i<juego.Buffer_MOV_INDEXCAPAS[1];i++)
	{
		if ( ((OBT_MOV_ORIGEN(juego.Buffer_MOV[i])) == origen) &&  ((OBT_MOV_DESTINO(juego.Buffer_MOV[i]))==destino))
		{
			*m = juego.Buffer_MOV[i];
			return VERDADERO;
		}
	}
	return FALSO;
}

static int indicePiezaEvaluacion(PIEZA pieza)
{
	switch (pieza)
	{
		case PEON_BLANCO: case PEON_NEGRO: return PEON;
		case ALFIL_BLANCO: case ALFIL_NEGRO: return ALFIL;
		case CABALLO_BLANCO: case CABALLO_NEGRO: return CABALLO;
		case TORRE_BLANCO: case TORRE_NEGRO: return TORRE;
		case DAMA_BLANCO: case DAMA_NEGRO: return DAMA;
		case REY_BLANCO: case REY_NEGRO: return REY;
		default: return -1;
	}
}

static int fasePiezaEvaluacion(int tipo)
{
	switch (tipo)
	{
		case CABALLO: case ALFIL: return 3;
		case TORRE: return 5;
		case DAMA: return 9;
		default: return 0;
	}
}

static int pstMedioPieza(PIEZA pieza, COLOR color, int tipo, int sq)
{
	switch (tipo)
	{
		case PEON: return PEON_PUNTAJE_POS[color][sq];
		case CABALLO: return CABALLO_PUNTAJE_POS[color][sq];
		case ALFIL: return ALFIL_PUNTAJE_POS[color][sq];
		case TORRE: return TORRE_PUNTAJE_POS[color ? ESPEJO[sq] : sq];
		case DAMA: return DAMA_PUNTAJE_POS[color][sq];
		default: (void)pieza; return 0;
	}
}

static int pstFinalPieza(PIEZA pieza, COLOR color, int tipo, int sq)
{
	switch (tipo)
	{
		case PEON: return PEON_PUNTAJE_POS[color][sq];
		case CABALLO: return CABALLO_PUNTAJE_POS_FINAL[color ? ESPEJO[sq] : sq];
		case ALFIL: return ALFIL_PUNTAJE_POS_FINAL[color ? ESPEJO[sq] : sq];
		case TORRE: return TORRE_PUNTAJE_POS[color ? ESPEJO[sq] : sq];
		case DAMA: return DAMA_PUNTAJE_POS_FINAL[color ? ESPEJO[sq] : sq];
		default: (void)pieza; return 0;
	}
}

static void sumarPiezaEstadoEvaluacion(PIEZA pieza, int sq, int delta)
{
	int tipo = indicePiezaEvaluacion(pieza);
	COLOR color;

	if (tipo < 0)
		return;
	color = OBT_COLOR_PIEZA(pieza);
	juego.estadoEvaluacion.conteo[color][tipo] =
		(uint8)(juego.estadoEvaluacion.conteo[color][tipo] + delta);
	juego.estadoEvaluacion.fase[color] =
		(uint8)(juego.estadoEvaluacion.fase[color] +
			delta * fasePiezaEvaluacion(tipo));
	juego.estadoEvaluacion.pstMedio[color] +=
		delta * pstMedioPieza(pieza, color, tipo, sq);
	juego.estadoEvaluacion.pstFinal[color] +=
		delta * pstFinalPieza(pieza, color, tipo, sq);
	if (tipo == REY && delta > 0)
		juego.estadoEvaluacion.escaqueRey[color] = (uint8)sq;
}

void reconstruirEstadoEvaluacion(void)
{
	int sq;

	memset(&juego.estadoEvaluacion, 0, sizeof(juego.estadoEvaluacion));
	for (sq = 0; sq < 64; sq++)
		sumarPiezaEstadoEvaluacion(ESCAQUES[sq], sq, 1);
}

static void actualizarEstadoEvaluacionMovimiento(MOVIMIENTO mov)
{
	int origen = OBT_MOV_ORIGEN(mov);
	int destino = OBT_MOV_DESTINO(mov);
	PIEZA pieza = OBT_MOV_PIEZA(mov);
	PIEZA captura = OBT_MOV_CAPTURA(mov);
	int escaqueCaptura = destino;

	sumarPiezaEstadoEvaluacion(pieza, origen, -1);
	sumarPiezaEstadoEvaluacion(pieza, destino, 1);
	if (captura)
	{
		if (ES_MOV_CAPT_PEON_PASO(mov))
			escaqueCaptura = pieza == PEON_BLANCO ? destino - 8 : destino + 8;
		sumarPiezaEstadoEvaluacion(captura, escaqueCaptura, -1);
	}
	if (ES_MOV_PROMOCION(mov))
	{
		sumarPiezaEstadoEvaluacion(pieza, destino, -1);
		sumarPiezaEstadoEvaluacion(OBT_MOV_PROMOCION(mov), destino, 1);
	}
	if (mov == ENROQUE_BLANCO_OO)
	{
		sumarPiezaEstadoEvaluacion(TORRE_BLANCO, 7, -1);
		sumarPiezaEstadoEvaluacion(TORRE_BLANCO, 5, 1);
	}
	else if (mov == ENROQUE_BLANCO_OOO)
	{
		sumarPiezaEstadoEvaluacion(TORRE_BLANCO, 0, -1);
		sumarPiezaEstadoEvaluacion(TORRE_BLANCO, 3, 1);
	}
	else if (mov == ENROQUE_NEGRO_OO)
	{
		sumarPiezaEstadoEvaluacion(TORRE_NEGRO, 63, -1);
		sumarPiezaEstadoEvaluacion(TORRE_NEGRO, 61, 1);
	}
	else if (mov == ENROQUE_NEGRO_OOO)
	{
		sumarPiezaEstadoEvaluacion(TORRE_NEGRO, 56, -1);
		sumarPiezaEstadoEvaluacion(TORRE_NEGRO, 59, 1);
	}
}

void hacerMovimiento(MOVIMIENTO mov)
{
	uint8 		origen;
	uint8 		destino;
	uint8 		pieza;
	uint8 		captura;
	uint8		postmp;
	PIEZA		piezaPromocion;
	uint16		relojPrevio;
	BOOLEANO	eraPeon;

	uint64 bitmapOri, bitmapOriDes, bitmapTorreEnroque;

	origen 	= OBT_MOV_ORIGEN(mov);
	destino = OBT_MOV_DESTINO(mov);
	pieza 	= OBT_MOV_PIEZA(mov);
	captura	= OBT_MOV_CAPTURA(mov);
	relojPrevio = juego.reglaCincuentaMov;
	eraPeon = (pieza == PEON_BLANCO || pieza == PEON_NEGRO);

	bitmapOri    = BITSET[origen];
	bitmapOriDes = BITSET[origen] | BITSET[destino];

	juego.historicoJuego[juego.indiceHJuego].OOB	 		= juego.OOB;
	juego.historicoJuego[juego.indiceHJuego].OOOB			= juego.OOOB;
	juego.historicoJuego[juego.indiceHJuego].OON			= juego.OON;
	juego.historicoJuego[juego.indiceHJuego].OOON			= juego.OOON;
	juego.historicoJuego[juego.indiceHJuego].ENROQUEB		= juego.ENROQUEB;
	juego.historicoJuego[juego.indiceHJuego].ENROQUEN		= juego.ENROQUEN;
	juego.historicoJuego[juego.indiceHJuego].posPeonPaso		= juego.posPeonPaso;
	juego.historicoJuego[juego.indiceHJuego].reglaCincuentaMov	= juego.reglaCincuentaMov;
	juego.historicoJuego[juego.indiceHJuego].totalMov		= juego.totalMov;
	juego.historicoJuego[juego.indiceHJuego].mov			= mov;
	juego.historicoJuego[juego.indiceHJuego].llaveHash		= juego.llaveHash;
	juego.historicoJuego[juego.indiceHJuego].llavePeones		= juego.llavePeones;
	juego.historicoJuego[juego.indiceHJuego].estadoEvaluacion	= juego.estadoEvaluacion;
	actualizarEstadoEvaluacionMovimiento(mov);

	juego.llaveHash ^= (arrayHash.llaves[origen][pieza] ^ arrayHash.llaves[destino][pieza]);
	if (pieza == PEON_BLANCO || pieza == PEON_NEGRO)
	{
		juego.llavePeones ^= arrayHash.llaves[origen][pieza];
		if (!ES_MOV_PROMOCION(mov))
			juego.llavePeones ^= arrayHash.llaves[destino][pieza];
	}
	if (captura == PEON_BLANCO || captura == PEON_NEGRO)
	{
		postmp = destino;
		if (ES_MOV_CAPT_PEON_PASO(mov))
			postmp = (pieza == PEON_BLANCO) ? destino - 8 : destino + 8;
		juego.llavePeones ^= arrayHash.llaves[postmp][captura];
	}

	if (juego.posPeonPaso != SIN_POS_VALIDA)
	{
		juego.llaveHash	^= arrayHash.ep[juego.posPeonPaso];
	}

	switch (pieza)
	{
		/*PIEZAS BLANCAS*/
		case PEON_BLANCO:
			juego.tablero[BLANCO][PEON] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = PEON_BLANCO;
			ESCAQUES[origen]  		 = VACIO;

			if (captura)
			{
				if (ES_MOV_CAPT_PEON_PASO(mov))
				{
					postmp = destino-8;
					juego.tablero[NEGRO][PEON] 	^= BITSET[postmp];
					juego.negros			^= BITSET[postmp];
					juego.ocupados			^= (bitmapOriDes | BITSET[postmp]); 	
					ESCAQUES[postmp] = VACIO;
					juego.material_peon_negro   	-= VALOR_PEON;
					juego.material_total        	+= VALOR_PEON;
					juego.llaveHash 		^= arrayHash.llaves[postmp][PEON_NEGRO];

				} else {
					hacerCaptura(captura, destino);
					juego.ocupados ^= bitmapOri; 	
				}
			} else {
				juego.ocupados ^= bitmapOriDes;
			}

			if ((RANKS[origen] == 2) && (RANKS[destino] == 4)) 
			{
				juego.posPeonPaso = origen+8;
				juego.llaveHash  ^= arrayHash.ep[juego.posPeonPaso]; 
			} else {
				juego.posPeonPaso = SIN_POS_VALIDA;
			}

			if (ES_MOV_PROMOCION(mov))
			{
				piezaPromocion = OBT_MOV_PROMOCION(mov);
				hacerPromocion(piezaPromocion,destino);
				ESCAQUES[destino] = piezaPromocion;
			}

			juego.reglaCincuentaMov = 0;
			juego.colorTurno = NEGRO;
			break;


		case TORRE_BLANCO:
			juego.tablero[BLANCO][TORRE] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = TORRE_BLANCO;
			ESCAQUES[origen]  		 = VACIO;

			if (captura)
			{
				hacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			if (juego.OOB)
			{
				if (origen == 7)
				{
					juego.llaveHash	^= arrayHash.OOB;
					juego.OOB 	 = FALSO;
//					juego.ENROQUEB 	 = VERDADERO;
				}
			} 

			if (juego.OOOB)
			{
				if (origen == 0)
				{
					juego.llaveHash	^= arrayHash.OOOB;
					juego.OOOB 	 = FALSO;
//					juego.ENROQUEB 	 = VERDADERO;
				}
			} 

			juego.posPeonPaso 	= SIN_POS_VALIDA;
			juego.reglaCincuentaMov++;
			juego.colorTurno 	= NEGRO;
			break;

		case CABALLO_BLANCO:
			juego.tablero[BLANCO][CABALLO] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = CABALLO_BLANCO;
			ESCAQUES[origen]  		 = VACIO;

			if (captura)
			{
				hacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.posPeonPaso 	= SIN_POS_VALIDA;
			juego.reglaCincuentaMov++;
			juego.colorTurno 	= NEGRO;
			break;

		case ALFIL_BLANCO:
			juego.tablero[BLANCO][ALFIL] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = ALFIL_BLANCO;
			ESCAQUES[origen]  		 = VACIO;

			if (captura)
			{
				hacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.posPeonPaso 	= SIN_POS_VALIDA;
			juego.reglaCincuentaMov++;
			juego.colorTurno 	= NEGRO;
			break;

		case DAMA_BLANCO:
			juego.tablero[BLANCO][DAMA] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = DAMA_BLANCO;
			ESCAQUES[origen]  		 = VACIO;

			if (captura)
			{
				hacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.posPeonPaso 	= SIN_POS_VALIDA;
			juego.reglaCincuentaMov++;
			juego.colorTurno 	= NEGRO;
			break;

		case REY_BLANCO:
			juego.tablero[BLANCO][REY] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = REY_BLANCO;
			ESCAQUES[origen]  		 = VACIO;
			if (juego.OOB)
			{
				juego.llaveHash	^= arrayHash.OOB;
			}

			if (juego.OOOB)
			{
				juego.llaveHash	^= arrayHash.OOOB;
			}

			if (captura)
			{
				hacerCaptura(captura, destino);
			}

			if (mov == ENROQUE_BLANCO_OO)
			{
				bitmapTorreEnroque 		 = BITSET[5] | BITSET[7]; 
				juego.tablero[BLANCO][TORRE] 	^= bitmapTorreEnroque;
				juego.blancos			^= bitmapTorreEnroque;
				ESCAQUES[7] 			 = VACIO;
				ESCAQUES[5] 			 = TORRE_BLANCO;
				juego.llaveHash ^= (arrayHash.llaves[7][TORRE_BLANCO] ^ arrayHash.llaves[5][TORRE_BLANCO]);
				juego.ENROQUEB 	 		 = ENROQUE_OO;

			} else if (mov == ENROQUE_BLANCO_OOO) {
				bitmapTorreEnroque = BITSET[0] | BITSET[3]; 
				juego.tablero[BLANCO][TORRE] 	^= bitmapTorreEnroque;
				juego.blancos			^= bitmapTorreEnroque;
				ESCAQUES[0] 			 = VACIO;
				ESCAQUES[3] 			 = TORRE_BLANCO;
				juego.llaveHash ^= (arrayHash.llaves[0][TORRE_BLANCO] ^ arrayHash.llaves[3][TORRE_BLANCO]);
				juego.ENROQUEB 	 		 = ENROQUE_OOO;
			}
			juego.ocupados   = juego.blancos | juego.negros;
	
			if (juego.OOB || juego.OOOB)
			{
				juego.OOB  = FALSO;
				juego.OOOB = FALSO;
			} 
			juego.posPeonPaso 	= SIN_POS_VALIDA;
			juego.reglaCincuentaMov++;
			juego.colorTurno 	= NEGRO;
			break;


		/*PIEZAS NEGRAS*/
		case PEON_NEGRO:
			juego.tablero[NEGRO][PEON] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = PEON_NEGRO;
			ESCAQUES[origen]  		 = VACIO;

			if (captura)
			{
				if (ES_MOV_CAPT_PEON_PASO(mov))
				{
					postmp = destino+8;
					juego.tablero[BLANCO][PEON] 	^= BITSET[postmp];
					juego.blancos			^= BITSET[postmp];
					juego.ocupados			^= (bitmapOriDes | BITSET[postmp]);
					ESCAQUES[postmp]		 = VACIO;
 					juego.material_peon_blanco   	-= VALOR_PEON;
					juego.material_total        	-= VALOR_PEON;
					juego.llaveHash 		^= arrayHash.llaves[postmp][PEON_BLANCO];

				} else {
					hacerCaptura(captura, destino);
					juego.ocupados			^= bitmapOri; 	
				}
			} else {
				juego.ocupados ^= bitmapOriDes;
			}

			if ((RANKS[origen] == 7) && (RANKS[destino] == 5)) 
			{
				juego.posPeonPaso = origen-8;
				juego.llaveHash  ^= arrayHash.ep[juego.posPeonPaso]; 

			} else {
				juego.posPeonPaso = SIN_POS_VALIDA;
			}

			if (ES_MOV_PROMOCION(mov))
			{
				piezaPromocion = OBT_MOV_PROMOCION(mov);
				hacerPromocion(piezaPromocion,destino);
				ESCAQUES[destino] = piezaPromocion;
			}

			juego.reglaCincuentaMov = 0;
			juego.colorTurno = BLANCO;
			break;
		case TORRE_NEGRO:
			juego.tablero[NEGRO][TORRE] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = TORRE_NEGRO;
			ESCAQUES[origen]  		 = VACIO;

			if (captura)
			{
				hacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			if (juego.OON)
			{
				if (origen == 63)
				{
					juego.llaveHash	^= arrayHash.OON;
					juego.OON 	 = FALSO;
//					juego.ENROQUEN 	 = VERDADERO;
				}
			} 

			if (juego.OOON)
			{
				if (origen == 56)
				{
					juego.llaveHash	^= arrayHash.OOON;
					juego.OOON 	 = FALSO;
//					juego.ENROQUEN 	 = VERDADERO;
				}
			} 

			juego.posPeonPaso 	= SIN_POS_VALIDA;
			juego.reglaCincuentaMov++;
			juego.colorTurno 	= BLANCO;
			break;

		case CABALLO_NEGRO:
			juego.tablero[NEGRO][CABALLO] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = CABALLO_NEGRO;
			ESCAQUES[origen]  		 = VACIO;

			if (captura)
			{
				hacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.posPeonPaso 	= SIN_POS_VALIDA;
			juego.reglaCincuentaMov++;
			juego.colorTurno 	= BLANCO;
			break;

		case ALFIL_NEGRO:
			juego.tablero[NEGRO][ALFIL] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = ALFIL_NEGRO;
			ESCAQUES[origen]  		 = VACIO;

			if (captura)
			{
				hacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.posPeonPaso 	= SIN_POS_VALIDA;
			juego.reglaCincuentaMov++;
			juego.colorTurno 	= BLANCO;
			break;

		case DAMA_NEGRO:
			juego.tablero[NEGRO][DAMA] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = DAMA_NEGRO;
			ESCAQUES[origen]  		 = VACIO;

			if (captura)
			{
				hacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.posPeonPaso 	= SIN_POS_VALIDA;
			juego.reglaCincuentaMov++;
			juego.colorTurno 	= BLANCO;
			break;

		case REY_NEGRO:
			juego.tablero[NEGRO][REY] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = REY_NEGRO;
			ESCAQUES[origen]  		 = VACIO;
			if (juego.OON)
			{
				juego.llaveHash	^= arrayHash.OON;
			}

			if (juego.OOON)
			{
				juego.llaveHash	^= arrayHash.OOON;
			}


			if (captura)
			{
				hacerCaptura(captura, destino);
			}

			if (mov == ENROQUE_NEGRO_OO)
			{
				bitmapTorreEnroque 		 = BITSET[61] | BITSET[63]; 
				juego.tablero[NEGRO][TORRE] 	^= bitmapTorreEnroque;
				juego.negros			^= bitmapTorreEnroque;
				ESCAQUES[63] 			 = VACIO;
				ESCAQUES[61] 			 = TORRE_NEGRO;
				juego.llaveHash ^= (arrayHash.llaves[63][TORRE_NEGRO] ^ arrayHash.llaves[61][TORRE_NEGRO]);
				juego.ENROQUEN 	 		 = ENROQUE_OO;


			} else if (mov == ENROQUE_NEGRO_OOO) {
				bitmapTorreEnroque 		 = BITSET[56] | BITSET[59]; 
				juego.tablero[NEGRO][TORRE] 	^= bitmapTorreEnroque;
				juego.negros			^= bitmapTorreEnroque;
				ESCAQUES[56] 			 = VACIO;
				ESCAQUES[59] 			 = TORRE_NEGRO;
				juego.llaveHash ^= (arrayHash.llaves[56][TORRE_NEGRO] ^ arrayHash.llaves[59][TORRE_NEGRO]);
				juego.ENROQUEN 	 		 = ENROQUE_OOO;
			}
			juego.ocupados   = juego.blancos | juego.negros;

			if (juego.OON || juego.OOON)
			{
				juego.OON  = FALSO;
				juego.OOON = FALSO;
			} 

			juego.posPeonPaso 	= SIN_POS_VALIDA;
			juego.reglaCincuentaMov++;
			juego.colorTurno 	= BLANCO;
			break;
	}

	if (eraPeon || captura)
		juego.reglaCincuentaMov = 0;
	else
		juego.reglaCincuentaMov =
			(relojPrevio < 65535u) ? (uint16)(relojPrevio + 1u) : 65535u;

	juego.totalMov++;
	juego.indiceHJuego++;
	juego.llaveHash ^= arrayHash.lado;

#ifdef DEBUG_MANGO_AJEDREZ 
	debugMov("Hacer mov", mov);
#endif

}

void desHacerMovimiento(MOVIMIENTO mov)
{
	uint8 		origen;
	uint8 		destino;
	uint8 		pieza;
	uint8 		captura;
	uint8		postmp;

	uint64 bitmapOri, bitmapOriDes, bitmapTorreEnroque;

	origen 	= OBT_MOV_ORIGEN(mov);
	destino = OBT_MOV_DESTINO(mov);
	pieza 	= OBT_MOV_PIEZA(mov);
	captura	= OBT_MOV_CAPTURA(mov);


	bitmapOri    = BITSET[origen];
	bitmapOriDes = BITSET[origen] | BITSET[destino];

	switch (pieza)
	{
		/*PIEZAS BLANCAS*/
		case PEON_BLANCO:
			juego.tablero[BLANCO][PEON] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = PEON_BLANCO;

			if (captura)
			{
				if (ES_MOV_CAPT_PEON_PASO(mov))
				{
					postmp = destino-8;
					juego.tablero[NEGRO][PEON] 	^= BITSET[postmp];
					juego.negros			^= BITSET[postmp];
					juego.ocupados			^= (bitmapOriDes | BITSET[postmp]); 	
					ESCAQUES[postmp]		 = PEON_NEGRO;
					juego.material_peon_negro   	+= VALOR_PEON;
					juego.material_total        	-= VALOR_PEON;

				} else {
					desHacerCaptura(captura, destino);
					juego.ocupados ^= bitmapOri; 	
				}
			} else {
				juego.ocupados ^= bitmapOriDes;
			}

			if (ES_MOV_PROMOCION(mov))
			{
				desHacerPromocion(OBT_MOV_PROMOCION(mov),destino);
			}

			juego.colorTurno = BLANCO;
			break;


		case TORRE_BLANCO:
			juego.tablero[BLANCO][TORRE] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = TORRE_BLANCO;

			if (captura)
			{
				desHacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.colorTurno 	= BLANCO;
			break;

		case CABALLO_BLANCO:
			juego.tablero[BLANCO][CABALLO] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = CABALLO_BLANCO;

			if (captura)
			{
				desHacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.colorTurno 	= BLANCO;
			break;

		case ALFIL_BLANCO:
			juego.tablero[BLANCO][ALFIL] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = ALFIL_BLANCO;

			if (captura)
			{
				desHacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.colorTurno 	= BLANCO;
			break;

		case DAMA_BLANCO:
			juego.tablero[BLANCO][DAMA] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = DAMA_BLANCO;

			if (captura)
			{
				desHacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.colorTurno 	= BLANCO;
			break;

		case REY_BLANCO:
			juego.tablero[BLANCO][REY] 	^= bitmapOriDes;
			juego.blancos			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = REY_BLANCO;

			if (captura)
			{
				desHacerCaptura(captura, destino);
			}

			if (mov == ENROQUE_BLANCO_OO)
			{
				bitmapTorreEnroque = BITSET[5] | BITSET[7]; 
				juego.tablero[BLANCO][TORRE] 	^= bitmapTorreEnroque;
				juego.blancos			^= bitmapTorreEnroque;
				ESCAQUES[7] 			 = TORRE_BLANCO;
				ESCAQUES[5] 			 = VACIO;
				juego.ENROQUEB 			 = SIN_ENROQUE;

			} else if (mov == ENROQUE_BLANCO_OOO) {
				bitmapTorreEnroque = BITSET[0] | BITSET[3]; 
				juego.tablero[BLANCO][TORRE] 	^= bitmapTorreEnroque;
				juego.blancos			^= bitmapTorreEnroque;
				ESCAQUES[0] 			 = TORRE_BLANCO;
				ESCAQUES[3] 			 = VACIO;
				juego.ENROQUEB 			 = SIN_ENROQUE;
			}
			juego.ocupados   = juego.blancos | juego.negros;
			juego.colorTurno = BLANCO;
			break;

		/*PIEZAS NEGRAS*/
		case PEON_NEGRO:
			juego.tablero[NEGRO][PEON] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = PEON_NEGRO;

			if (captura)
			{
				if (ES_MOV_CAPT_PEON_PASO(mov))
				{
					postmp = destino+8;
					juego.tablero[BLANCO][PEON] 	^= BITSET[postmp];
					juego.blancos			^= BITSET[postmp];
					juego.ocupados			^= (bitmapOriDes | BITSET[postmp]);
					ESCAQUES[postmp]		 = PEON_BLANCO;
 					juego.material_peon_blanco   	+= VALOR_PEON;
					juego.material_total        	+= VALOR_PEON;
	

				} else {
					desHacerCaptura(captura, destino);
					juego.ocupados			^= bitmapOri; 	
				}
			} else {
				juego.ocupados ^= bitmapOriDes;
			}

			if (ES_MOV_PROMOCION(mov))
			{
				desHacerPromocion(OBT_MOV_PROMOCION(mov),destino);
			}

			juego.colorTurno = NEGRO;
			break;

		case TORRE_NEGRO:
			juego.tablero[NEGRO][TORRE] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = TORRE_NEGRO;

			if (captura)
			{
				desHacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.colorTurno 	= NEGRO;
			break;

		case CABALLO_NEGRO:
			juego.tablero[NEGRO][CABALLO] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = CABALLO_NEGRO;

			if (captura)
			{
				desHacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.colorTurno 	= NEGRO;
			break;

		case ALFIL_NEGRO:
			juego.tablero[NEGRO][ALFIL] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = ALFIL_NEGRO;

			if (captura)
			{
				desHacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.colorTurno 	= NEGRO;
			break;

		case DAMA_NEGRO:
			juego.tablero[NEGRO][DAMA] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = DAMA_NEGRO;

			if (captura)
			{
				desHacerCaptura(captura, destino);
				juego.ocupados ^= bitmapOri;
			} else {

				juego.ocupados ^= bitmapOriDes;
			}

			juego.colorTurno 	= NEGRO;
			break;

		case REY_NEGRO:
			juego.tablero[NEGRO][REY] 	^= bitmapOriDes;
			juego.negros			^= bitmapOriDes;
			ESCAQUES[destino] 		 = VACIO;
			ESCAQUES[origen]  		 = REY_NEGRO;

			if (captura)
			{
				desHacerCaptura(captura, destino);
			}

			if (mov == ENROQUE_NEGRO_OO)
			{
				bitmapTorreEnroque = BITSET[61] | BITSET[63]; 
				juego.tablero[NEGRO][TORRE] 	^= bitmapTorreEnroque;
				juego.negros			^= bitmapTorreEnroque;
				ESCAQUES[63] 			 = TORRE_NEGRO;
				ESCAQUES[61] 			 = VACIO;
				juego.ENROQUEN 			 = SIN_ENROQUE;


			} else if (mov == ENROQUE_NEGRO_OOO) {
				bitmapTorreEnroque = BITSET[56] | BITSET[59]; 
				juego.tablero[NEGRO][TORRE] 	^= bitmapTorreEnroque;
				juego.negros			^= bitmapTorreEnroque;
				ESCAQUES[56] 			 = TORRE_NEGRO;
				ESCAQUES[59] 			 = VACIO;
				juego.ENROQUEN 			 = SIN_ENROQUE;

			}
			juego.ocupados   = juego.blancos | juego.negros;
			juego.colorTurno = NEGRO;
			break;
	}

	juego.indiceHJuego--;

	juego.OOB 		= juego.historicoJuego[juego.indiceHJuego].OOB;
	juego.OOOB 		= juego.historicoJuego[juego.indiceHJuego].OOOB;
	juego.OON		= juego.historicoJuego[juego.indiceHJuego].OON;
	juego.OOON		= juego.historicoJuego[juego.indiceHJuego].OOON;
	juego.ENROQUEB		= juego.historicoJuego[juego.indiceHJuego].ENROQUEB;		
	juego.ENROQUEN		= juego.historicoJuego[juego.indiceHJuego].ENROQUEN;		
	juego.posPeonPaso	= juego.historicoJuego[juego.indiceHJuego].posPeonPaso;
	juego.reglaCincuentaMov	= juego.historicoJuego[juego.indiceHJuego].reglaCincuentaMov;
	juego.totalMov		= juego.historicoJuego[juego.indiceHJuego].totalMov;
	mov			= juego.historicoJuego[juego.indiceHJuego].mov;
	juego.llaveHash		= juego.historicoJuego[juego.indiceHJuego].llaveHash;
	juego.llavePeones	= juego.historicoJuego[juego.indiceHJuego].llavePeones;
	juego.estadoEvaluacion	= juego.historicoJuego[juego.indiceHJuego].estadoEvaluacion;


#ifdef DEBUG_MANGO_AJEDREZ 
	debugMov("Des hacer mov", mov);
#endif


}


void hacerCaptura(PIEZA PiezaCaptura, uint8 destino)
{
	uint64 mapaBit = BITSET[destino];
	juego.llaveHash ^= arrayHash.llaves[destino][PiezaCaptura];

	switch (PiezaCaptura)
	{
		/*PIEZAS BLANCAS*/
		case PEON_BLANCO:
			juego.tablero[BLANCO][PEON] ^= mapaBit;
			juego.blancos		    ^= mapaBit;	
			juego.material_peon_blanco  -= VALOR_PEON;
			juego.material_total        -= VALOR_PEON;
			break;

		case TORRE_BLANCO:
			juego.tablero[BLANCO][TORRE] ^= mapaBit;
			juego.blancos		     ^= mapaBit;	
			juego.material_lado_blanco   -= VALOR_TORRE;
			juego.material_total         -= VALOR_TORRE;
			if ((destino == 7) && juego.OOB)
			{  
				juego.llaveHash ^= arrayHash.OOB;
				juego.OOB   = FALSO;
			} 
			if ((destino == 0) && juego.OOOB)
			{
				juego.llaveHash ^= arrayHash.OOOB;
				juego.OOOB  = FALSO; 
			}
			break;

		case CABALLO_BLANCO:
			juego.tablero[BLANCO][CABALLO] ^= mapaBit;
			juego.blancos		       ^= mapaBit;	
			juego.material_lado_blanco     -= VALOR_CABALLO;
			juego.material_total           -= VALOR_CABALLO;
			break;

		case ALFIL_BLANCO:
			juego.tablero[BLANCO][ALFIL] ^= mapaBit;
			juego.blancos		     ^= mapaBit;	
			juego.material_lado_blanco  -= VALOR_ALFIL;
			juego.material_total        -= VALOR_ALFIL;
			break;

		case DAMA_BLANCO:
			juego.tablero[BLANCO][DAMA] ^= mapaBit;
			juego.blancos		    ^= mapaBit;
			juego.material_lado_blanco  -= VALOR_DAMA;
			juego.material_total        -= VALOR_DAMA;
			break;

		case REY_BLANCO:
			juego.tablero[BLANCO][REY] ^= mapaBit;
			juego.blancos		   ^= mapaBit;
			break;


		/*PIEZAS NEGRAS*/
		case PEON_NEGRO:
			juego.tablero[NEGRO][PEON]  ^= mapaBit;
			juego.negros		    ^= mapaBit;	
			juego.material_peon_negro   -= VALOR_PEON;
			juego.material_total        += VALOR_PEON;
			break;

		case TORRE_NEGRO:
			juego.tablero[NEGRO][TORRE]  ^= mapaBit;
			juego.negros		     ^= mapaBit;
			juego.material_lado_negro    -= VALOR_TORRE;
			juego.material_total         += VALOR_TORRE;
			if ((destino == 63) && juego.OON)
			{  
				juego.llaveHash ^= arrayHash.OON;
				juego.OON   = FALSO;
			} 
			if ((destino == 56) && juego.OOON)
			{
				juego.llaveHash ^= arrayHash.OOON;
				juego.OOON  = FALSO; 
			}
			break;

		case CABALLO_NEGRO:
			juego.tablero[NEGRO][CABALLO]  ^= mapaBit;
			juego.negros		       ^= mapaBit;	
			juego.material_lado_negro      -= VALOR_CABALLO;
			juego.material_total           += VALOR_CABALLO;
			break;

		case ALFIL_NEGRO:
			juego.tablero[NEGRO][ALFIL]  ^= mapaBit;
			juego.negros		     ^= mapaBit;
			juego.material_lado_negro    -= VALOR_ALFIL;
			juego.material_total         += VALOR_ALFIL;
			break;

		case DAMA_NEGRO:
			juego.tablero[NEGRO][DAMA]  ^= mapaBit;
			juego.negros		    ^= mapaBit;	
			juego.material_lado_negro   -= VALOR_DAMA;
			juego.material_total        += VALOR_DAMA;
			break;

		case REY_NEGRO:
			juego.tablero[NEGRO][REY]   ^= mapaBit;
			juego.negros		    ^= mapaBit;
			break;
	}

	juego.reglaCincuentaMov = 0;

}

void desHacerCaptura(PIEZA PiezaCaptura, uint8 destino)
{
	uint64 mapaBit = BITSET[destino];

	switch (PiezaCaptura)
	{
		/*PIEZAS BLANCAS*/
		case PEON_BLANCO:
			juego.tablero[BLANCO][PEON] ^= mapaBit;
			juego.blancos		    ^= mapaBit;	
			ESCAQUES[destino]	     = PEON_BLANCO;
			juego.material_peon_blanco  += VALOR_PEON;
			juego.material_total        += VALOR_PEON;
			break;

		case TORRE_BLANCO:
			juego.tablero[BLANCO][TORRE] ^= mapaBit;
			juego.blancos		     ^= mapaBit;	
			ESCAQUES[destino]	      = TORRE_BLANCO;
			juego.material_lado_blanco   += VALOR_TORRE;
			juego.material_total         += VALOR_TORRE;
			break;

		case CABALLO_BLANCO:
			juego.tablero[BLANCO][CABALLO] ^= mapaBit;
			juego.blancos		       ^= mapaBit;	
			ESCAQUES[destino]	        = CABALLO_BLANCO;
			juego.material_lado_blanco     += VALOR_CABALLO;
			juego.material_total           += VALOR_CABALLO;
			break;

		case ALFIL_BLANCO:
			juego.tablero[BLANCO][ALFIL] ^= mapaBit;
			juego.blancos		     ^= mapaBit;	
			ESCAQUES[destino]	      = ALFIL_BLANCO;
			juego.material_lado_blanco   += VALOR_ALFIL;
			juego.material_total         += VALOR_ALFIL;
			break;

		case DAMA_BLANCO:
			juego.tablero[BLANCO][DAMA] ^= mapaBit;
			juego.blancos		    ^= mapaBit;	
			ESCAQUES[destino]	     = DAMA_BLANCO;
			juego.material_lado_blanco  += VALOR_DAMA;
			juego.material_total        += VALOR_DAMA;
			break;

		case REY_BLANCO:
			juego.tablero[BLANCO][REY]  ^= mapaBit;
			juego.blancos		    ^= mapaBit;	
			ESCAQUES[destino]	     = REY_BLANCO;	
			break;


		/*PIEZAS NEGRAS*/
		case PEON_NEGRO:
			juego.tablero[NEGRO][PEON]  ^= mapaBit;
			juego.negros		    ^= mapaBit;	
			ESCAQUES[destino]	     = PEON_NEGRO;
			juego.material_peon_negro   += VALOR_PEON;
			juego.material_total        -= VALOR_PEON;
			break;

		case TORRE_NEGRO:
			juego.tablero[NEGRO][TORRE]  ^= mapaBit;
			juego.negros		     ^= mapaBit;	
			ESCAQUES[destino]	      = TORRE_NEGRO;
			juego.material_lado_negro    += VALOR_TORRE;
			juego.material_total         -= VALOR_TORRE;
			break;

		case CABALLO_NEGRO:
			juego.tablero[NEGRO][CABALLO]  ^= mapaBit;
			juego.negros		       ^= mapaBit;	
			ESCAQUES[destino]	        = CABALLO_NEGRO;
			juego.material_lado_negro      += VALOR_CABALLO;
			juego.material_total           -= VALOR_CABALLO;
			break;

		case ALFIL_NEGRO:
			juego.tablero[NEGRO][ALFIL]  ^= mapaBit;
			juego.negros		     ^= mapaBit;	
			ESCAQUES[destino]	      = ALFIL_NEGRO;
			juego.material_lado_negro    += VALOR_ALFIL;
			juego.material_total         -= VALOR_ALFIL;
			break;

		case DAMA_NEGRO:
			juego.tablero[NEGRO][DAMA]  ^= mapaBit;
			juego.negros		    ^= mapaBit;	
			ESCAQUES[destino]	     = DAMA_NEGRO;
			juego.material_lado_negro   += VALOR_DAMA;
			juego.material_total        -= VALOR_DAMA;
			break;

		case REY_NEGRO:
			juego.tablero[NEGRO][REY]   ^= mapaBit;
			juego.negros		    ^= mapaBit;	
			ESCAQUES[destino]	     = REY_NEGRO;	
			break;
	}

}

void hacerPromocion(PIEZA piezaPromocion, uint8 posPromocion)
{
	uint64 mapaBit = BITSET[posPromocion];

	switch (piezaPromocion)
	{
		/*PIEZAS BLANCAS*/
		case TORRE_BLANCO:
			juego.tablero[BLANCO][TORRE] ^= mapaBit;
			juego.tablero[BLANCO][PEON]  ^= mapaBit;
			juego.material_peon_blanco   -= VALOR_PEON;
			juego.material_total         -= VALOR_PEON;
			juego.material_lado_blanco   += VALOR_TORRE;
			juego.material_total         += VALOR_TORRE;
			juego.llaveHash ^= (arrayHash.llaves[posPromocion][PEON_BLANCO]^arrayHash.llaves[posPromocion][piezaPromocion]);
			break;

		case CABALLO_BLANCO:
			juego.tablero[BLANCO][CABALLO] ^= mapaBit;
			juego.tablero[BLANCO][PEON]    ^= mapaBit;
			juego.material_peon_blanco     -= VALOR_PEON;
			juego.material_total           -= VALOR_PEON;
			juego.material_lado_blanco     += VALOR_CABALLO;
			juego.material_total           += VALOR_CABALLO;
			juego.llaveHash ^= (arrayHash.llaves[posPromocion][PEON_BLANCO]^arrayHash.llaves[posPromocion][piezaPromocion]);
			break;

		case ALFIL_BLANCO:
			juego.tablero[BLANCO][ALFIL] ^= mapaBit;
			juego.tablero[BLANCO][PEON]  ^= mapaBit;
			juego.material_peon_blanco   -= VALOR_PEON;
			juego.material_total         -= VALOR_PEON;
			juego.material_lado_blanco   += VALOR_ALFIL;
			juego.material_total         += VALOR_ALFIL;
			juego.llaveHash ^= (arrayHash.llaves[posPromocion][PEON_BLANCO]^arrayHash.llaves[posPromocion][piezaPromocion]);
			break;

		case DAMA_BLANCO:
			juego.tablero[BLANCO][DAMA] ^= mapaBit;
			juego.tablero[BLANCO][PEON] ^= mapaBit;
			juego.material_peon_blanco  -= VALOR_PEON;
			juego.material_total        -= VALOR_PEON;
			juego.material_lado_blanco  += VALOR_DAMA;
			juego.material_total        += VALOR_DAMA;
			juego.llaveHash ^= (arrayHash.llaves[posPromocion][PEON_BLANCO]^arrayHash.llaves[posPromocion][piezaPromocion]);
			break;


		/*PIEZAS NEGRAS*/

		case TORRE_NEGRO:
			juego.tablero[NEGRO][TORRE]  ^= mapaBit;
			juego.tablero[NEGRO][PEON]   ^= mapaBit;
			juego.material_peon_negro    -= VALOR_PEON;
			juego.material_total         += VALOR_PEON;
			juego.material_lado_negro    += VALOR_TORRE;
			juego.material_total         -= VALOR_TORRE;
			juego.llaveHash ^= (arrayHash.llaves[posPromocion][PEON_NEGRO]^arrayHash.llaves[posPromocion][piezaPromocion]);
			break;

		case CABALLO_NEGRO:
			juego.tablero[NEGRO][CABALLO]  ^= mapaBit;
			juego.tablero[NEGRO][PEON]     ^= mapaBit;
			juego.material_peon_negro      -= VALOR_PEON;
			juego.material_total           += VALOR_PEON;
			juego.material_lado_negro      += VALOR_CABALLO;
			juego.material_total           -= VALOR_CABALLO;
			juego.llaveHash ^= (arrayHash.llaves[posPromocion][PEON_NEGRO]^arrayHash.llaves[posPromocion][piezaPromocion]);
			break;

		case ALFIL_NEGRO:
			juego.tablero[NEGRO][ALFIL]  ^= mapaBit;
			juego.tablero[NEGRO][PEON]   ^= mapaBit;
			juego.material_peon_negro    -= VALOR_PEON;
			juego.material_total         += VALOR_PEON;
			juego.material_lado_negro    += VALOR_ALFIL;
			juego.material_total         -= VALOR_ALFIL;
			juego.llaveHash ^= (arrayHash.llaves[posPromocion][PEON_NEGRO]^arrayHash.llaves[posPromocion][piezaPromocion]);
			break;

		case DAMA_NEGRO:
			juego.tablero[NEGRO][DAMA]  ^= mapaBit;
			juego.tablero[NEGRO][PEON]  ^= mapaBit;
			juego.material_peon_negro   -= VALOR_PEON;
			juego.material_total        += VALOR_PEON;
			juego.material_lado_negro   += VALOR_DAMA;
			juego.material_total        -= VALOR_DAMA;
			juego.llaveHash ^= (arrayHash.llaves[posPromocion][PEON_NEGRO]^arrayHash.llaves[posPromocion][piezaPromocion]);
			break;
	}
}

void desHacerPromocion(PIEZA piezaPromocion, uint8 posPromocion)
{
	uint64 mapaBit = BITSET[posPromocion];

	switch (piezaPromocion)
	{
		/*PIEZAS BLANCAS*/
		case TORRE_BLANCO:
			juego.tablero[BLANCO][TORRE] ^= mapaBit;
			juego.tablero[BLANCO][PEON]  ^= mapaBit;
			juego.material_peon_blanco   += VALOR_PEON;
			juego.material_total         += VALOR_PEON;
			juego.material_lado_blanco   -= VALOR_TORRE;
			juego.material_total         -= VALOR_TORRE;
			break;

		case CABALLO_BLANCO:
			juego.tablero[BLANCO][CABALLO] ^= mapaBit;
			juego.tablero[BLANCO][PEON]    ^= mapaBit;
			juego.material_peon_blanco     += VALOR_PEON;
			juego.material_total           += VALOR_PEON;
			juego.material_lado_blanco     -= VALOR_CABALLO;
			juego.material_total           -= VALOR_CABALLO;
			break;

		case ALFIL_BLANCO:
			juego.tablero[BLANCO][ALFIL] ^= mapaBit;
			juego.tablero[BLANCO][PEON]  ^= mapaBit;
			juego.material_peon_blanco   += VALOR_PEON;
			juego.material_total         += VALOR_PEON;
			juego.material_lado_blanco   -= VALOR_ALFIL;
			juego.material_total         -= VALOR_ALFIL;
			break;

		case DAMA_BLANCO:
			juego.tablero[BLANCO][DAMA]  ^= mapaBit;
			juego.tablero[BLANCO][PEON]  ^= mapaBit;
			juego.material_peon_blanco   += VALOR_PEON;
			juego.material_total         += VALOR_PEON;
			juego.material_lado_blanco   -= VALOR_DAMA;
			juego.material_total         -= VALOR_DAMA;
			break;


		/*PIEZAS NEGRAS*/

		case TORRE_NEGRO:
			juego.tablero[NEGRO][TORRE]  ^= mapaBit;
			juego.tablero[NEGRO][PEON]   ^= mapaBit;
			juego.material_peon_negro    += VALOR_PEON;
			juego.material_total         -= VALOR_PEON;
			juego.material_lado_negro    -= VALOR_TORRE;
			juego.material_total         += VALOR_TORRE;
			break;

		case CABALLO_NEGRO:
			juego.tablero[NEGRO][CABALLO]  ^= mapaBit;
			juego.tablero[NEGRO][PEON]     ^= mapaBit;
			juego.material_peon_negro    += VALOR_PEON;
			juego.material_total         -= VALOR_PEON;
			juego.material_lado_negro    -= VALOR_CABALLO;
			juego.material_total         += VALOR_CABALLO;
			break;

		case ALFIL_NEGRO:
			juego.tablero[NEGRO][ALFIL]  ^= mapaBit;
			juego.tablero[NEGRO][PEON]   ^= mapaBit;
			juego.material_peon_negro    += VALOR_PEON;
			juego.material_total         -= VALOR_PEON;
			juego.material_lado_negro    -= VALOR_ALFIL;
			juego.material_total         += VALOR_ALFIL;
			break;

		case DAMA_NEGRO:
			juego.tablero[NEGRO][DAMA]  ^= mapaBit;
			juego.tablero[NEGRO][PEON]  ^= mapaBit;
			juego.material_peon_negro    += VALOR_PEON;
			juego.material_total         -= VALOR_PEON;
			juego.material_lado_negro    -= VALOR_DAMA;
			juego.material_total         += VALOR_DAMA;
			break;
	}
}

void iniciarTableroFEN()
{

	juego.tablero[BLANCO][PEON] 	^= 	juego.tablero[BLANCO][PEON];
	juego.tablero[BLANCO][TORRE] 	^= 	juego.tablero[BLANCO][TORRE];
	juego.tablero[BLANCO][CABALLO] 	^= 	juego.tablero[BLANCO][CABALLO];
	juego.tablero[BLANCO][ALFIL] 	^= 	juego.tablero[BLANCO][ALFIL];
	juego.tablero[BLANCO][DAMA]  	^= 	juego.tablero[BLANCO][DAMA];
	juego.tablero[BLANCO][REY]  	^= 	juego.tablero[BLANCO][REY];

	juego.tablero[NEGRO][PEON] 	^= 	juego.tablero[NEGRO][PEON];
	juego.tablero[NEGRO][TORRE] 	^= 	juego.tablero[NEGRO][TORRE];
	juego.tablero[NEGRO][CABALLO] 	^= 	juego.tablero[NEGRO][CABALLO];
	juego.tablero[NEGRO][ALFIL] 	^= 	juego.tablero[NEGRO][ALFIL];
	juego.tablero[NEGRO][DAMA]  	^= 	juego.tablero[NEGRO][DAMA];
	juego.tablero[NEGRO][REY]  	^= 	juego.tablero[NEGRO][REY];

	int i=0;

	for (i = 0; i < 64; i++)
	{
		if (ESCAQUES[i] == REY_BLANCO) 
		{  
			juego.tablero[BLANCO][REY]    	|= BITSET[i]; 
		}
		if (ESCAQUES[i] == DAMA_BLANCO)
		{
			juego.tablero[BLANCO][DAMA] 	|= BITSET[i];
		}
		if (ESCAQUES[i] == TORRE_BLANCO)
		{
			juego.tablero[BLANCO][TORRE]	|= BITSET[i];
		}
		if (ESCAQUES[i] == ALFIL_BLANCO)
		{
			juego.tablero[BLANCO][ALFIL] 	|= BITSET[i];
		}
		if (ESCAQUES[i] == CABALLO_BLANCO) 
		{
			juego.tablero[BLANCO][CABALLO]  |= BITSET[i];
		}
		if (ESCAQUES[i] == PEON_BLANCO)   
		{
			juego.tablero[BLANCO][PEON] 	|= BITSET[i];
		}
		if (ESCAQUES[i] == REY_NEGRO)   
		{
			juego.tablero[NEGRO][REY] 	|= BITSET[i];
		}
		if (ESCAQUES[i] == DAMA_NEGRO)  
		{
			juego.tablero[NEGRO][DAMA] 	|= BITSET[i];
		}
		if (ESCAQUES[i] == TORRE_NEGRO)   
		{
			juego.tablero[NEGRO][TORRE] 	|= BITSET[i];
		}
		if (ESCAQUES[i] == ALFIL_NEGRO) 
		{
			juego.tablero[NEGRO][ALFIL] 	|= BITSET[i];
		}
		if (ESCAQUES[i] == CABALLO_NEGRO) 
		{
			juego.tablero[NEGRO][CABALLO] 	|= BITSET[i];
		}
		if (ESCAQUES[i] == PEON_NEGRO)   
		{
			juego.tablero[NEGRO][PEON] 	|= BITSET[i];
		}
	}

	iniTablerosUtil();
	
	juego.material_peon_blanco 	= cuentaBit(juego.tablero[BLANCO][PEON]) 	* VALOR_PEON;
	juego.material_peon_negro 	= cuentaBit(juego.tablero[NEGRO][PEON])  	* VALOR_PEON;

	juego.material_lado_blanco 	= cuentaBit(juego.tablero[BLANCO][TORRE])  	* VALOR_TORRE 	+
					  cuentaBit(juego.tablero[BLANCO][CABALLO])  	* VALOR_CABALLO +
					  cuentaBit(juego.tablero[BLANCO][ALFIL])  	* VALOR_ALFIL   +
					  cuentaBit(juego.tablero[BLANCO][DAMA])  	* VALOR_DAMA;

	juego.material_lado_negro 	= cuentaBit(juego.tablero[NEGRO][TORRE])  	* VALOR_TORRE 	+
					  cuentaBit(juego.tablero[NEGRO][CABALLO])  	* VALOR_CABALLO +
					  cuentaBit(juego.tablero[NEGRO][ALFIL])  	* VALOR_ALFIL   +
					  cuentaBit(juego.tablero[NEGRO][DAMA])  	* VALOR_DAMA;

	juego.material_total		=  (juego.material_lado_blanco + juego.material_peon_blanco) -
  					   (juego.material_lado_negro  + juego.material_peon_negro);

	juego.indiceHJuego = 0;

}

int obtMayorValorPiezaDeBando(COLOR color)
{

	if (juego.tablero[color][DAMA]) 	return VALOR_DAMA;
	if (juego.tablero[color][TORRE]) 	return VALOR_TORRE;
	if (juego.tablero[color][ALFIL]) 	return VALOR_ALFIL;
	if (juego.tablero[color][CABALLO]) 	return VALOR_CABALLO;
	if (juego.tablero[color][PEON]) 	return VALOR_PEON;

	return 0;

}

int obtSegundoValorPiezaDeBando(COLOR color)
{

	if (juego.tablero[color][DAMA]) 	
	{
		if (juego.tablero[color][TORRE]) 	return VALOR_TORRE;
		if (juego.tablero[color][ALFIL]) 	return VALOR_ALFIL;
		if (juego.tablero[color][CABALLO]) 	return VALOR_CABALLO;
		if (juego.tablero[color][PEON]) 	return VALOR_PEON;

		return VALOR_DAMA;
	}


	if (juego.tablero[color][TORRE]) 	
	{
		if (juego.tablero[color][ALFIL]) 	return VALOR_ALFIL;
		if (juego.tablero[color][CABALLO]) 	return VALOR_CABALLO;
		if (juego.tablero[color][PEON]) 	return VALOR_PEON;

		return VALOR_TORRE;
	}

	if (juego.tablero[color][ALFIL]) 	
	{
		if (juego.tablero[color][CABALLO]) 	return VALOR_CABALLO;
		if (juego.tablero[color][PEON]) 	return VALOR_PEON;

		return VALOR_ALFIL;
	}

	if (juego.tablero[color][CABALLO]) 	
	{
		if (juego.tablero[color][PEON]) 	return VALOR_PEON;

		return VALOR_CABALLO;
	}

	if (juego.tablero[color][PEON]) 	
	{
		return VALOR_MEDIO_PEON;
	}

	return 0;

}
#endif
