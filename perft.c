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

#ifndef PERFT_C
#define PERFT_C

uint64 perft(int capa, int profundidad)
{
 
	uint64 numNodos = 0;     
	int i, contMov=0;
	BOOLEANO esJaque;	
	 	
	if (profundidad == 0)
	{
		return 1;
	}
	 
	juego.Buffer_MOV_INDEXCAPAS[capa+1] = generarTodosMov(juego.Buffer_MOV_INDEXCAPAS[capa]);
	 
	for (i = juego.Buffer_MOV_INDEXCAPAS[capa]; i < juego.Buffer_MOV_INDEXCAPAS[capa+1]; i++)
	{
		hacerMovimiento(juego.Buffer_MOV[i]);

		if (juego.colorTurno) // Le toca al negro, se chequea que el rey blanco no haya quedado en jaque
		{
			esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
		} else {
			esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
		}

		if (!esJaque)
		{
			contMov++;
			numNodos += perft(capa + 1, profundidad-1);

			if (profundidad == 1)
			{
//*
				if (ES_MOV_CAPTURA(juego.Buffer_MOV[i])) 	DataPerft.InvCaptura++;
				if (ES_MOV_CAPT_PEON_PASO(juego.Buffer_MOV[i]))	DataPerft.InvPeonPaso++;
				if (ES_MOV_PROMOCION(juego.Buffer_MOV[i])) 	DataPerft.InvPromocion++;	
				if (ES_MOV_OO(juego.Buffer_MOV[i])) 		DataPerft.InvEnroqueOO++;
				if (ES_MOV_OOO(juego.Buffer_MOV[i])) 		DataPerft.InvEnroqueOOO++;

				if (!juego.colorTurno) 
				{
					esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
				} else {
					esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
				}
				if (esJaque) 					DataPerft.InvJaqueContrario++;
//*/
			}
		}

		desHacerMovimiento(juego.Buffer_MOV[i]);
	}


	return numNodos;
}


#endif
