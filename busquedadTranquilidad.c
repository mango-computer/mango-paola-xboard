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

#ifndef BUSQUEDADTRANQULIDAD_C
#define BUSQUEDADTRANQULIDAD_C


int busquedadTranquilidad(int capa, int alfa, int beta)
{
	int i				= 0;
	int V				= 0;
	int Vparcial			= 0;
	int j				= 0;
	int valorFC			= 0;
	BOOLEANO esJaque 		= FALSO;
	BOOLEANO estoyJaque 		= FALSO;
	BOOLEANO existeUnMovValido 	= FALSO;
	MOVIMIENTO h_mov		= 0;
	BOOLEANO es			= FALSO;
	BOOLEANO esFinal		= ES_ETAPA_FINAL;
	COLOR colorMueve;

	juego.triangularLargo[capa] = capa;

	//Chequear Repeticion del tablero
	ES_REPETICION_TABLERO(es);
	if (es)
	{
		return PUNTAJE_EMPATE;
	}

//*
	// Poda Distancia de Mate
	if (capa) 
	{
		register int mv = VALOR_JAQUE_MATE - capa + 2;

		if (mv < beta)
		{
			beta = mv;
			if (alfa >= mv) return mv;
		}

		mv = -VALOR_JAQUE_MATE + capa - 2;
		if (mv > alfa)
		{
			alfa = mv;
			if (beta <= mv) return mv;
		}
	}
//*/

	if (tiempoVencido) 	  		return 0;	
	if (capa >= MAX_CAPAS_QBUSQUEDAD) 	return evaluacionTablero(alfa, beta);	

	estoyJaque = ES_ESTADO_JUEGO_JAQUE;
	Vparcial   = evaluacionTablero(alfa, beta);

	// Si le toca al negro, se chequea que el rey negro este en jaque o viceversa
	if (estoyJaque)
	{
		juego.Buffer_MOV_INDEXCAPAS[capa+1] = generarTodosMov(juego.Buffer_MOV_INDEXCAPAS[capa]);
		ponderarMovimientos(capa, &h_mov, 2);


	} else {

		if (Vparcial >= beta) return Vparcial;//beta;

//****************************************************************************************************************
//****************************************************************************************************************
//			DELTA PODA 
//****************************************************************************************************************
//****************************************************************************************************************
//*
		if (!ES_ETAPA_FINAL)
		{
			int DELTA = VALOR_DAMA; 
			if ((juego.tablero[BLANCO][PEON] & RANKBIT[7]) || (juego.tablero[NEGRO][PEON] & RANKBIT[2]))
			{	
				DELTA += INCREMENTO_PROMO_DELTA_CORTE;
			}
			if ( Vparcial + DELTA < alfa  ) 
			{
				return alfa;
			}
		}
//*/
//****************************************************************************************************************
//****************************************************************************************************************
//			FIN DELTA PODA 
//****************************************************************************************************************
//****************************************************************************************************************

		if (Vparcial >  alfa) alfa = Vparcial;

		juego.Buffer_MOV_INDEXCAPAS[capa+1] = generarMovCapPro(juego.Buffer_MOV_INDEXCAPAS[capa]);
	}

	colorMueve = juego.colorTurno; 

	for (i = juego.Buffer_MOV_INDEXCAPAS[capa]; i < juego.Buffer_MOV_INDEXCAPAS[capa+1]; i++)
	{
		seleccionarMovimiento(capa, i);
		hacerMovimiento(juego.Buffer_MOV[i]);
		
		if (juego.colorTurno) // Le toca al negro, se chequea que el rey blanco no haya quedado en jaque
		{
			esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
		} else {
			esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
		}

		if (!esJaque)
		{
//*
			if (!OBT_MOV_PROMOCION(juego.Buffer_MOV[i]) && !ALFA_BETA_PROXIMO_MATE && !esFinal &&
			    !estoyJaque && ES_MOV_CAPTURA(juego.Buffer_MOV[i]))
			{
				nDama[colorMueve]  		= cuentaBit(juego.tablero[colorMueve][DAMA]);
				nCaballos[colorMueve]		= cuentaBit(juego.tablero[colorMueve][CABALLO]);
				nTorres[colorMueve]		= cuentaBit(juego.tablero[colorMueve][TORRE]);
				nAlfil[colorMueve]		= cuentaBit(juego.tablero[colorMueve][ALFIL]);
				fase[colorMueve] 		= MINIMO(31,((nDama[colorMueve]	   * 9)+
									     (nTorres[colorMueve]  * 5)+
									     (nCaballos[colorMueve]* 3)+
									     (nAlfil[colorMueve]   * 3)));

				valorFC = VALORPIEZA_FASE[OBT_MOV_CAPTURA(juego.Buffer_MOV[i])];
		
				if ((VALORPIEZA_FASE[OBT_MOV_PIEZA(juego.Buffer_MOV[i])] > valorFC) &&
				    ((fase[colorMueve] - valorFC) > 0) && (EET(juego.Buffer_MOV[i]) < -15))
				{
					desHacerMovimiento(juego.Buffer_MOV[i]);
					continue;
				}
			}
//*/
			existeUnMovValido = VERDADERO;
			contadorNodos++;
			QcontadorNodos++;
			if (--contadorDescendente <=0) verReloj_y_Teclado();

			V = -busquedadTranquilidad(capa+1,-beta, -alfa);
			desHacerMovimiento(juego.Buffer_MOV[i]);

			if (V > alfa)
			{
				if (V >= beta) 
				{ 
					actualizarMovMatadores(juego.Buffer_MOV[i], V, capa);
					//Actualizar Tabla Hash de Movimientos
					if (esUsoTablaHash)
					{
						agregarMovTablaHash(0, capa, V, BANDERA_HASH_ABAJO, juego.Buffer_MOV[i]);
					}
					return V;//beta;
				}

				alfa = V;
				juego.triangularArray[capa][capa] = juego.Buffer_MOV[i];		
				for (j = capa + 1; j < juego.triangularLargo[capa+1]; ++j) 
				{
					juego.triangularArray[capa][j] = juego.triangularArray[capa+1][j];
				}
				juego.triangularLargo[capa] = juego.triangularLargo[capa+1];
			}

		} else {
			desHacerMovimiento(juego.Buffer_MOV[i]);
		}
	}

	if (!existeUnMovValido)
	{
		if (estoyJaque) // Esta jaque mate
		{
			return -VALOR_JAQUE_MATE+capa-2;
		}
	}

	if (juego.reglaCincuentaMov >= 100)
	{ 
		return PUNTAJE_EMPATE;
	}
	return alfa;

}

#endif
