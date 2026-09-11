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

#ifndef BUSQUEDADTRANQULIDAD_C
#define BUSQUEDADTRANQULIDAD_C


int busquedadTranquilidad(int capa, int alfa, int beta)
{
	int i				= 0;
	int V				= 0;
	int Vparcial			= 0;
	int j				= 0;
	int valorFC			= 0;
	BOOLEANO eetSuperaPoda		= VERDADERO;
	BOOLEANO esJaque 		= FALSO;
	BOOLEANO estoyJaque 		= FALSO;
	BOOLEANO existeUnMovValido 	= FALSO;
	MOVIMIENTO h_mov		= 0;
	BOOLEANO es			= FALSO;
	BOOLEANO esFinal		= ES_ETAPA_FINAL;
	COLOR colorMueve;

	juego.triangularLargo[capa] = capa;

	// Comprobar repetición del tablero
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
	Vparcial = INT_MAX;

	if (estoyJaque)
	{
		juego.Buffer_MOV_INDEXCAPAS[capa+1] = generarTodosMov(juego.Buffer_MOV_INDEXCAPAS[capa]);
		ponderarMovimientos(capa, &h_mov, 2);
	} else {
		if (esUsoTablaHash)
		{
			int qflag = BANDERA_HASH_VACIO;
			MOVIMIENTO qmov = 0;

			verificarTablaHash(alfa, beta, capa, 0, &qflag, &qmov,
					  &Vparcial);
		}
		if (Vparcial == INT_MAX)
		{
			BOOLEANO lazyPrev = permitirLazyEval;

			permitirLazyEval = FALSO;
			Vparcial = evaluacionTablero(alfa, beta);
			permitirLazyEval = lazyPrev;
		}

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
		eetSuperaPoda = VERDADERO;
		if (!OBT_MOV_PROMOCION(juego.Buffer_MOV[i]) && !ALFA_BETA_PROXIMO_MATE && !esFinal &&
		    !estoyJaque && ES_MOV_CAPTURA(juego.Buffer_MOV[i]))
		{
			eetSuperaPoda = EETSuperaUmbral(juego.Buffer_MOV[i], -15);
		}
		hacerMovimiento(juego.Buffer_MOV[i]);
		
		if (juego.colorTurno) // Si le toca al negro, se comprueba que el rey blanco no haya quedado en jaque
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
				faseEval[colorMueve] = MINIMO(31,
					juego.estadoEvaluacion.fase[colorMueve]);

				valorFC = VALORPIEZA_FASE[OBT_MOV_CAPTURA(juego.Buffer_MOV[i])];
		
				if ((VALORPIEZA_FASE[OBT_MOV_PIEZA(juego.Buffer_MOV[i])] > valorFC) &&
				    ((faseEval[colorMueve] - valorFC) > 0) && !eetSuperaPoda)
				{
#ifdef PRUEBAS_HCE
					contadorPodasEET++;
#endif
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
						agregarMovTablaHash(0, capa, V,
								   BANDERA_HASH_ABAJO,
								   juego.Buffer_MOV[i], Vparcial);
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
