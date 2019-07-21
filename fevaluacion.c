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

#ifndef FEVAL_C
#define FEVAL_C

int evaluacionTablero(int alfa, int beta)
{

	int puntaje = 0;
//	BOOLEANO esFinal = ES_ETAPA_FINAL;

//****************************************************************************************************************
//****************************************************************************************************************
//			COMPROBAR QUE ESTA POSICION NO HAYA SIDO YA EVALUADA 
//****************************************************************************************************************
//****************************************************************************************************************
//*
	if (verificarEvalTablaHash(&puntaje)) 
	{	
		puntaje = (juego.colorTurno)?-puntaje:puntaje;
		return puntaje;
	}
//*/
//****************************************************************************************************************
//****************************************************************************************************************
//			COMPROBAR CON EL ALGORITMO DE PUNTAJE PERSOSO 
//****************************************************************************************************************
//****************************************************************************************************************
//*
	int EvalFlojo =  (juego.colorTurno ? -juego.material_total : juego.material_total);
//*
	if (((EvalFlojo + LIMITE_MOV_NULL) < alfa) || (beta < (EvalFlojo - LIMITE_MOV_NULL)))
	{
		return EvalFlojo;
	} 
//*/
//****************************************************************************************************************


	nDama[BLANCO]			= cuentaBit(juego.tablero[BLANCO][DAMA]);
	nDama[NEGRO]			= cuentaBit(juego.tablero[NEGRO][DAMA]);

	nCaballos[BLANCO]		= cuentaBit(juego.tablero[BLANCO][CABALLO]);
	nCaballos[NEGRO]		= cuentaBit(juego.tablero[NEGRO][CABALLO]);

	nTorres[BLANCO]			= cuentaBit(juego.tablero[BLANCO][TORRE]);
	nTorres[NEGRO]			= cuentaBit(juego.tablero[NEGRO][TORRE]);

	nAlfil[BLANCO]			= cuentaBit(juego.tablero[BLANCO][ALFIL]);
	nAlfil[NEGRO]			= cuentaBit(juego.tablero[NEGRO][ALFIL]);

	fase[BLANCO] 			= MINIMO(31,((nDama[BLANCO]*9)+(nTorres[BLANCO]*5)+(nCaballos[BLANCO]*3)+(nAlfil[BLANCO]*3)));
	fase[NEGRO]  			= MINIMO(31,((nDama[NEGRO] *9)+(nTorres[NEGRO] *5)+(nCaballos[NEGRO] *3)+(nAlfil[NEGRO] *3)));
	FASE	 			= fase[BLANCO] + fase[NEGRO];
/*
	int margenEvalFlojo = (535-(FASE*6));
	if (!esFinal && (((EvalFlojo + margenEvalFlojo) < alfa) || (beta < (EvalFlojo - LIMITE_MOV_NULL))))
	{
		return EvalFlojo;
	} 
//*/
	escaqueRey[BLANCO]		= bitScanForwardBruijn(juego.tablero[BLANCO][REY]);
	escaqueRey[NEGRO]		= bitScanForwardBruijn(juego.tablero[NEGRO][REY]);

	entornoRey[BLANCO] 		= mascaraRey[escaqueRey[BLANCO]];
	entornoRey[NEGRO] 		= mascaraRey[escaqueRey[NEGRO]];

	tropismo[BLANCO]		= 0;
	tropismo[NEGRO]			= 0;

	mapaTodosPeones			= (juego.tablero[BLANCO][PEON] | juego.tablero[NEGRO][PEON]);

	int puntajeNegro		= 0;
	int puntajeBlanco		= 0;

	memset(peonDefectos,		0, sizeof(int)   *2*8);
	memset(peonesDebiles,		0, sizeof(uint64)*2);
	memset(peonesPasados,		0, sizeof(uint64)*2);
	memset(peonesCandidatos,	0, sizeof(uint64)*2);

	memset(mapaPosAtacadasXPza,	0, sizeof(uint64)*2*6);
	memset(mapaPosAtacadas,		0, sizeof(uint64)*2);

	juego.desOcupados		= ~juego.ocupados;

	nPeones[BLANCO]			= cuentaBit(juego.tablero[BLANCO][PEON]);
	nPeones[NEGRO]			= cuentaBit(juego.tablero[NEGRO][PEON]);

	nPiezasSP[NEGRO] 		=  nTorres[NEGRO]   + nCaballos[NEGRO]  + nAlfil[NEGRO]  + nDama[NEGRO];
	nPiezasSP[BLANCO] 		=  nTorres[BLANCO]  + nCaballos[BLANCO] + nAlfil[BLANCO] + nDama[BLANCO];

	nPiezas[NEGRO] 			=  nPeones[NEGRO]  + nPiezasSP[NEGRO];
	nPiezas[BLANCO] 		=  nPeones[BLANCO] + nPiezasSP[BLANCO];

	totalPiezasSP			= nPiezasSP[NEGRO] + nPiezasSP[BLANCO];
	totalPeones			= nPeones[NEGRO]   + nPeones[BLANCO];
	totalPiezas			= nPiezas[NEGRO]   + nPiezas[BLANCO];

	materialTotal			= juego.material_lado_blanco + juego.material_lado_negro;

	valorPiezas[NEGRO]		= (nCaballos[NEGRO] * VALOR_CABALLO) + (nAlfil[NEGRO] * VALOR_ALFIL) +
					  (nTorres[NEGRO]   * VALOR_TORRE)   + (nDama[NEGRO]  * VALOR_DAMA);

	valorPiezas[BLANCO]		= (nCaballos[BLANCO] * VALOR_CABALLO) + (nAlfil[BLANCO] * VALOR_ALFIL) +
					  (nTorres[BLANCO]   * VALOR_TORRE)   + (nDama[BLANCO]  * VALOR_DAMA);

	valorPeones[NEGRO]		= nPeones[NEGRO]  * VALOR_PEON;
	valorPeones[BLANCO]		= nPeones[BLANCO] * VALOR_PEON;

	piezasBlancas 			= juego.blancos ^ juego.tablero[BLANCO][REY];
	piezasNegras  			= juego.negros  ^ juego.tablero[NEGRO][REY];

	PIEZAS_MAYORES[BLANCO] 		= (nDama[BLANCO] * 2) + nTorres[BLANCO];
	PIEZAS_MAYORES[NEGRO]  		= (nDama[NEGRO]  * 2) + nTorres[NEGRO];

	PIEZAS_MENORES[BLANCO] 		= nCaballos[BLANCO] + nAlfil[BLANCO];				
	PIEZAS_MENORES[NEGRO]  		= nCaballos[NEGRO] + nAlfil[NEGRO];

	puntaje_m[BLANCO]		= 0;
	puntaje_m[NEGRO]		= 0;
	puntaje_f[BLANCO]		= 0;
	puntaje_f[NEGRO]		= 0;

	int puedeGanar			= 3;
	int mn				= 0;
	int my				= 0;
	int puntajeTemporal		= 0;

//****************************************************************************************************************
//****************************************************************************************************************
//			INI LADO NEGRO Y BLANCO 
//****************************************************************************************************************
//****************************************************************************************************************

	ini_material();

	//Determinar si un lado es peligroso
	//Blanco
	esPeligroso[BLANCO] = ((juego.tablero[BLANCO][DAMA] && (fase[BLANCO] > 13)) || ((nTorres[BLANCO]>1) && (fase[BLANCO] > 15)));
	esPeligroso[NEGRO]  = ((juego.tablero[NEGRO][DAMA]  && (fase[NEGRO]  > 13)) || ((nTorres[NEGRO] >1) && (fase[NEGRO]  > 15)));


	//Eval empate e insuficiencia de material
	if ((fase[BLANCO] < 13) && (fase[NEGRO] < 13))
	{
		do {
			if (!FASE && (mascaraSinPeonTorre & juego.tablero[BLANCO][PEON]) && 
				(mascaraSinPeonTorre & juego.tablero[NEGRO][PEON]))
			{
				break;
			}

			my = PIEZAS_MAYORES[BLANCO] - PIEZAS_MAYORES[NEGRO];
			if (abs(my)==1)
			{
				mn = PIEZAS_MENORES[BLANCO] - PIEZAS_MENORES[NEGRO];
				if (my == -mn)
				{
					if (!nPeones[NEGRO])  puedeGanar &= 1;
					if (!nPeones[BLANCO]) puedeGanar &= 2;
				}

				if (!puedeGanar) break;
			}

			if (!esPosibilidadVictoria(BLANCO))			
				puedeGanar &= 2;

			if (!esPosibilidadVictoria(NEGRO))			
				puedeGanar &= 1;
		} while(0);
	}

	if (!mapaTodosPeones)
	{
		if ((fase[BLANCO]>3) || (fase[NEGRO]>3))
		{
			if (juego.material_total > 0)
			{
				evaluarMate(BLANCO);

			} else if (juego.material_total < 0) {

				evaluarMate(NEGRO);
			}

			puntajeTemporal = puntaje_f[BLANCO] - puntaje_f[NEGRO];

			if ((puntajeTemporal > 0) && !(puedeGanar & 1))
				puntajeTemporal = puntajeTemporal/4;

			if ((puntajeTemporal < 0) && !(puedeGanar & 2))
				puntajeTemporal = puntajeTemporal/4;

			return (juego.colorTurno) ? -puntajeTemporal:puntajeTemporal;
		}
	} 

	//Peones
	evalPeones(BLANCO);
	evalPeones(NEGRO);

	if (peonesPasados[NEGRO] || peonesPasados[BLANCO])
	{
		if (peonesPasados[BLANCO])
			evalPeonesPasados(BLANCO);

		if (peonesPasados[NEGRO])
			evalPeonesPasados(NEGRO);

		if (((fase[BLANCO]==0) &&  peonesPasados[NEGRO]) || ((fase[NEGRO]==0) &&  peonesPasados[BLANCO]))
		{
			evalCarreraPeonesPasados();
		}
	}


	evalEntorno(NEGRO);
	evalEntorno(BLANCO);

	int pN = ((puntaje_m[NEGRO]  * FASE) + (puntaje_f[NEGRO]  * (62 - FASE))) / 62;			
	int pB = ((puntaje_m[BLANCO] * FASE) + (puntaje_f[BLANCO] * (62 - FASE))) / 62;			

	EvalFlojo = pB-pN;
	EvalFlojo = (juego.colorTurno ? -EvalFlojo : EvalFlojo);

	int corteMargen = ((esPeligroso[BLANCO] || esPeligroso[NEGRO]) ? 114+FASE:102)+FASE;

	BOOLEANO esEvaluarTodo = ((EvalFlojo + corteMargen) > alfa && (EvalFlojo - corteMargen) < beta);

	if (esEvaluarTodo)
	{
		//Eval Caballo
		evalCaballo(NEGRO);
		evalCaballo(BLANCO);

		//Eval Alfil
		evalAlfil(NEGRO);
		evalAlfil(BLANCO);

		//Eval Torre
		evalTorre(NEGRO);
		evalTorre(BLANCO);

		//Eval Dama
		evalDama(NEGRO);
		evalDama(BLANCO);
	}

	evalFilaRey(NEGRO);
	evalFilaRey(BLANCO);

	evalRey(NEGRO);
	evalRey(BLANCO);

#ifdef VERVALORES
	printf("******FINAL*******\n");
	printf("m[B]=%d\n",puntaje_m[BLANCO]);
	printf("f[B]=%d\n",puntaje_f[BLANCO]);
	printf("m[N]=%d\n",puntaje_m[NEGRO]);
	printf("f[N]=%d\n",puntaje_f[NEGRO]);
#endif

	puntajeNegro  =	((puntaje_m[NEGRO]  * FASE) + (puntaje_f[NEGRO]  * (62 - FASE))) / 62;			
	puntajeBlanco =	((puntaje_m[BLANCO] * FASE) + (puntaje_f[BLANCO] * (62 - FASE))) / 62;			

#ifdef VERVALORES
	printf("******FINAL 2*******\n");
	printf("puntajeBlanco=%d\n",puntajeBlanco);
	printf("puntajeNegro =%d\n",puntajeNegro);
#endif
  

// ****************************************************************************************************************
// **************************************** PIEZAS CLAVADAS *******************************************************
// ****************************************************************************************************************
//*
	if (esEvaluarTodo)
	{
		int contCB 		= 0;
		int contCN 		= 0;
		uint64 clavadas[2]	= {0,0};
		uint64 tempDestinos	= 0;
		int escaqueOrigen	= 0;


		// CLAVADAS NEGRAS*****
		//obtener clavadas con respecto al REY
		clavadas[NEGRO] = 
			obtPiezasClavadasPor(escaqueRey[NEGRO], BLANCO,
			(juego.tablero[NEGRO][DAMA]  | juego.tablero[NEGRO][TORRE] | 
			 juego.tablero[NEGRO][ALFIL] | juego.tablero[NEGRO][CABALLO]), 
			(juego.tablero[BLANCO][DAMA]  | juego.tablero[BLANCO][ALFIL]),
			(juego.tablero[BLANCO][DAMA]  | juego.tablero[BLANCO][TORRE]));


		//obtener clavadas con respecto a la  DAMA
		tempDestinos   = juego.tablero[NEGRO][DAMA];
		// analizar todas las damas negras presentes en el tablero
		while (tempDestinos)
		{		
			escaqueOrigen = bitScanForwardBruijn(tempDestinos);

			clavadas[NEGRO] |= 
				obtPiezasClavadasPor(escaqueOrigen, BLANCO,
				(juego.tablero[NEGRO][TORRE] |juego.tablero[NEGRO][ALFIL] | juego.tablero[NEGRO][CABALLO]),
				 juego.tablero[BLANCO][ALFIL], juego.tablero[BLANCO][TORRE]);


			tempDestinos ^= BITSET[escaqueOrigen];
		}

		//obtener clavadas con respecto a la TORRE
		tempDestinos   = juego.tablero[NEGRO][TORRE];
		// analizar todas las torres negras presentes en el tablero
		while (tempDestinos)
		{		
			escaqueOrigen = bitScanForwardBruijn(tempDestinos);

			clavadas[NEGRO] |= 
				obtPiezasClavadasPor(escaqueOrigen, BLANCO,
				(juego.tablero[NEGRO][ALFIL] | juego.tablero[NEGRO][CABALLO]),
				 juego.tablero[BLANCO][ALFIL], 0);

			tempDestinos ^= BITSET[escaqueOrigen];
		}


	// **********************************CLAVADAS BLANCAS**************************************************************

		//obtener clavadas con respecto al REY
		clavadas[BLANCO] = 
			obtPiezasClavadasPor(escaqueRey[BLANCO], NEGRO,
			(juego.tablero[BLANCO][DAMA]  | juego.tablero[BLANCO][TORRE] | 
			 juego.tablero[BLANCO][ALFIL] | juego.tablero[BLANCO][CABALLO]), 
			(juego.tablero[NEGRO][DAMA]  | juego.tablero[NEGRO][ALFIL]),
			(juego.tablero[NEGRO][DAMA]  | juego.tablero[NEGRO][TORRE]));


		//obtener clavadas con respecto a la  DAMA
		tempDestinos   = juego.tablero[BLANCO][DAMA];
		// analizar todas las damas negras presentes en el tablero
		while (tempDestinos)
		{		
			escaqueOrigen = bitScanForwardBruijn(tempDestinos);

			clavadas[BLANCO] |= 
				obtPiezasClavadasPor(escaqueOrigen, NEGRO,
				(juego.tablero[BLANCO][TORRE] |juego.tablero[BLANCO][ALFIL] | juego.tablero[BLANCO][CABALLO]),
				 juego.tablero[NEGRO][ALFIL], juego.tablero[NEGRO][TORRE]);

			tempDestinos ^= BITSET[escaqueOrigen];
		}

		//obtener clavadas con respecto a la TORRE
		tempDestinos   = juego.tablero[BLANCO][TORRE];
		// analizar todas las torres negras presentes en el tablero
		while (tempDestinos)
		{		
			escaqueOrigen = bitScanForwardBruijn(tempDestinos);

			clavadas[BLANCO] |= 
				obtPiezasClavadasPor(escaqueOrigen, NEGRO,
				(juego.tablero[BLANCO][ALFIL] | juego.tablero[BLANCO][CABALLO]),
				 juego.tablero[NEGRO][ALFIL], 0);

			tempDestinos ^= BITSET[escaqueOrigen];
		}

		// Analisis de las piezas clavadas
		//Blanco
		puntajeBlanco -= (cuentaBit(juego.tablero[BLANCO][CABALLO] & clavadas[BLANCO]) * DESCUENTO_CABALLO_CLAVADO);
		puntajeBlanco -= (cuentaBit(juego.tablero[BLANCO][ALFIL]   & clavadas[BLANCO]) * DESCUENTO_ALFIL_CLAVADO);
		puntajeBlanco -= (cuentaBit(juego.tablero[BLANCO][TORRE]   & clavadas[BLANCO]) * DESCUENTO_TORRE_CLAVADO);
		puntajeBlanco -= (cuentaBit(juego.tablero[BLANCO][DAMA]    & clavadas[BLANCO]) * DESCUENTO_DAMA_CLAVADO);

		//Negro
		puntajeNegro -= (cuentaBit(juego.tablero[NEGRO][CABALLO]   & clavadas[NEGRO]) * DESCUENTO_CABALLO_CLAVADO);
		puntajeNegro -= (cuentaBit(juego.tablero[NEGRO][ALFIL]     & clavadas[NEGRO]) * DESCUENTO_ALFIL_CLAVADO);
		puntajeNegro -= (cuentaBit(juego.tablero[NEGRO][TORRE]     & clavadas[NEGRO]) * DESCUENTO_TORRE_CLAVADO);
		puntajeNegro -= (cuentaBit(juego.tablero[NEGRO][DAMA]      & clavadas[NEGRO]) * DESCUENTO_DAMA_CLAVADO);


	// ****************************************** PIEZAS COLGADAS (piezas atacadas y no defendidas) *****************************

			//Piezas negras colgadas
			tempDestinos = mapaPosAtacadas[BLANCO] & juego.tablero[NEGRO][CABALLO];
			while (tempDestinos)
			{		
				escaqueOrigen = bitScanForwardBruijn(tempDestinos);

				if (!(mapaPosAtacadas[NEGRO] & BITSET[escaqueOrigen]))
				{
					contCN++;
				}
				tempDestinos ^= BITSET[escaqueOrigen];
			}

			tempDestinos = mapaPosAtacadas[BLANCO] & juego.tablero[NEGRO][ALFIL];
			while (tempDestinos)
			{		
				escaqueOrigen = bitScanForwardBruijn(tempDestinos);

				if (!(mapaPosAtacadas[NEGRO] & BITSET[escaqueOrigen]))
				{
					contCN++;
				}
				tempDestinos ^= BITSET[escaqueOrigen];
			}

			tempDestinos = mapaPosAtacadas[BLANCO] & juego.tablero[NEGRO][TORRE];
			while (tempDestinos)
			{		
				escaqueOrigen = bitScanForwardBruijn(tempDestinos);

				if (!(mapaPosAtacadas[NEGRO] & BITSET[escaqueOrigen]))
				{
					contCN++;
				}
				tempDestinos ^= BITSET[escaqueOrigen];
			}

			tempDestinos = mapaPosAtacadas[BLANCO] & juego.tablero[NEGRO][DAMA];
			while (tempDestinos)
			{		
				escaqueOrigen = bitScanForwardBruijn(tempDestinos);

				if (!(mapaPosAtacadas[NEGRO] & BITSET[escaqueOrigen]))
				{
					contCN++;
				}
				tempDestinos ^= BITSET[escaqueOrigen];
			}

			//Piezas blancas colgadas
			tempDestinos = mapaPosAtacadas[NEGRO] & juego.tablero[BLANCO][CABALLO];
			while (tempDestinos)
			{		
				escaqueOrigen = bitScanForwardBruijn(tempDestinos);

				if (!(mapaPosAtacadas[BLANCO] & BITSET[escaqueOrigen]))
				{
					contCB++;
				}
				tempDestinos ^= BITSET[escaqueOrigen];
			}

			tempDestinos = mapaPosAtacadas[NEGRO] & juego.tablero[BLANCO][ALFIL];
			while (tempDestinos)
			{		
				escaqueOrigen = bitScanForwardBruijn(tempDestinos);

				if (!(mapaPosAtacadas[BLANCO] & BITSET[escaqueOrigen]))
				{
					contCB++;
				}
				tempDestinos ^= BITSET[escaqueOrigen];
			}

			tempDestinos = mapaPosAtacadas[NEGRO] & juego.tablero[BLANCO][TORRE];
			while (tempDestinos)
			{		
				escaqueOrigen = bitScanForwardBruijn(tempDestinos);

				if (!(mapaPosAtacadas[BLANCO] & BITSET[escaqueOrigen]))
				{
					contCB++;
				}
				tempDestinos ^= BITSET[escaqueOrigen];
			}

			tempDestinos = mapaPosAtacadas[NEGRO] & juego.tablero[BLANCO][DAMA];
			while (tempDestinos)
			{		
				escaqueOrigen = bitScanForwardBruijn(tempDestinos);

				if (!(mapaPosAtacadas[BLANCO] & BITSET[escaqueOrigen]))
				{
					contCB++;
				}
				tempDestinos ^= BITSET[escaqueOrigen];
			}

			puntajeNegro	+= ((contCB - contCN) * BONO_PIEZAS_COLGADAS);
			puntajeBlanco	+= ((contCN - contCB) * BONO_PIEZAS_COLGADAS);
	}
//*/


//****************************************************************************************************************
//****************************************************************************************************************
//			OBTENER RESULTADO SEGUN EL BANDO A EVALUAR 
//****************************************************************************************************************
//****************************************************************************************************************

	puntajeTemporal = puntajeBlanco - puntajeNegro;
#ifdef VERVALORES
	printf("******FINAL puntajeTemporal = puntajeBlanco - puntajeNegro*******\n");
	printf("puntajeTemporal=%d\n",puntajeTemporal);
	printf("puedeGanar=%d\n",puedeGanar);

#endif

	puntajeTemporal = transformarSegunEmpate(puedeGanar,puntajeTemporal);

#ifdef VERVALORES
	printf("******FINAL transformarSegunEmpate*******\n");
	printf("puntajeTemporal=%d\n",puntajeTemporal);
#endif

	puntaje = (juego.colorTurno)?-puntajeTemporal:puntajeTemporal;

	agregarEvalTablaHash(puntajeTemporal);

	return 	puntaje; 		
	
}


//****************************************************************************************************************
//****************************************************************************************************************
//			****************** F U N C I O N E S **************************** 
//****************************************************************************************************************
//****************************************************************************************************************

void evalEntorno(COLOR colorEval)
{
	COLOR xcolorEval = !colorEval;
	int esqE4[2] = {28,36};
	int esqD4[2] = {27,35};
	int esqC2[2] = {10,50};
	int esqC3[2] = {18,42};
	int ajuste;

	if (!(BITSET[esqE4[colorEval]] & juego.tablero[colorEval][PEON]) && 
			BITSET[esqD4[colorEval]] & juego.tablero[colorEval][PEON]) 
	{
		if ((BITSET[esqC2[colorEval]] & juego.tablero[colorEval][PEON]) && 
			(BITSET[esqC3[colorEval]] & (juego.tablero[colorEval][CABALLO] | juego.tablero[colorEval][ALFIL])))
		{
			puntaje_m[colorEval] -= 12;
		}
	}

	if (colorEval)
	{
		if (!juego.ENROQUEN)
		{
			ajuste =  (juego.tablero[xcolorEval][DAMA]?3:1);

			if (juego.OON && juego.OOON)
			{
				puntaje_m[colorEval] -= (ajuste * DESCUENTO_X_NO_ENRROCAR);

			} else  if (!juego.OON && !juego.OOON) {

				puntaje_m[colorEval] -= (ajuste * DESCUENTO_PERDIDA_ENROQUE);

			} else  {

				puntaje_m[colorEval] -= ((ajuste * DESCUENTO_PERDIDA_ENROQUE)/2);
			}
		}

	} else {

		if (!juego.ENROQUEB)
		{
			ajuste =  (juego.tablero[xcolorEval][DAMA]?3:1);

			if (juego.OOB && juego.OOOB)
			{
				puntaje_m[colorEval] -= (ajuste * DESCUENTO_X_NO_ENRROCAR);

			} else  if (!juego.OOB && !juego.OOOB) {

				puntaje_m[colorEval] -= (ajuste * DESCUENTO_PERDIDA_ENROQUE);

			} else  {

				puntaje_m[colorEval] -= ((ajuste * DESCUENTO_PERDIDA_ENROQUE)/2);
			}
		}
	}
}


void evalPeonesPasados(COLOR colorEval)
{
	COLOR xcolorEval 	= !colorEval;
	int sq			= 0;
	int sqT			= 0;
	int sqAdelante		= 0;
	int rank_sq		= 0;
	int bono		= 0;
	int mgsc		= 0;
	int egsc		= 0;
	uint64 mapa		= 0;	
	int filaPeon		= 0;
	unsigned char mapaPP	= 0;

	mapaPP = peonesPasadosMapaFila[colorEval];
	for (;mapaPP;mapaPP &= mapaPP - 1)		
	{

		filaPeon 	= lsb_8bit[mapaPP];
		sq 		= CABEZA_ESQ(colorEval, (FILEBIT[filaPeon+1] & juego.tablero[colorEval][PEON]));
		rank_sq 	= CONVER_RANKS[colorEval][RANKS[sq]];
		bono 		= BONO_PEON_DIST_CC[rank_sq];

		mgsc = 200;
		egsc = 220;

		if (MASCARA_PEON_CC[sq] & juego.tablero[colorEval][PEON])
		{
			mgsc += 20;
			egsc += 65;
		} 
		
		if (juego.tablero[colorEval][TORRE] | juego.tablero[xcolorEval][TORRE])
		{
			mapa = ((colorEval)? mas8dir[sq]:menos8dir[sq]) & juego.ocupados;

			if (mapa)
			{
				sqT = CABEZA_ESQ(colorEval, mapa);
				if (BITSET[sqT] & juego.tablero[colorEval][TORRE])
				{
					mgsc += 20;
					egsc += 60;

				} else if (BITSET[sqT] & juego.tablero[xcolorEval][TORRE]) {

					mgsc -= 20;
					egsc -= 60;
				}
			}
		}

		sqAdelante = sq + DIR[colorEval];
		mapa = ((colorEval)?menos8dir[sq]:mas8dir[sq]);

		if (mapa & juego.ocupados)
		{
			egsc -= 25;

			if (BITSET[sqAdelante] & ((colorEval)?juego.blancos:juego.negros))
			{
				mgsc -= 95;
				egsc -= 70;

			} else if (BITSET[sqAdelante] & ((colorEval)?juego.negros:juego.blancos)) {

				mgsc -= 45;
				egsc -= 20;

			}
		} else {
			
			egsc +=(CARRERA_PEONES[colorEval][juego.colorTurno][sq] & juego.tablero[xcolorEval][REY])?25:40;

		}
		
		puntaje_m[colorEval] += ((BONO_PEON_CC[rank_sq] * mgsc) / 100);
		puntaje_f[colorEval] += ((BONO_PEON_CC[rank_sq] * egsc) / 100);
		puntaje_f[colorEval] -= ((DISTANCIA[sqAdelante][escaqueRey[colorEval]] - 
					  DISTANCIA[sqAdelante][escaqueRey[xcolorEval]]) * bono);

	} // Fin for Peon 

	if (peonesPasadosMapaFila[colorEval])
	{
		if (is_outside[peonesPasadosMapaFila[colorEval]][peonesMapaFila[xcolorEval]])
		{
			puntaje_m[colorEval] += outside_pp[MEDIO_JUEGO];
			puntaje_f[colorEval] += outside_pp[FINAL_JUEGO];
		}
	}	

}

void evalCarreraPeonesPasados()
{
#define NO_MAS_COLORES 2

	COLOR colorEval;
	COLOR xcolorEval;
	uint64 tempOrigenes	= 0; 
	int escaqueOrigen	= 0;
	int signo[2]		= {1,-1};
	unsigned char mapaPP	= 0;
	int esqPeonCabeza	= 0;
	int filaPeon		= 0;
	int f[2]		= {0,0};
	int dama_distancia	= 0;
	uint64 corredor[2]	= {0,0};
	int damer[2]		= {8,8};

 for (colorEval=BLANCO;colorEval<NO_MAS_COLORES;colorEval++)
 {
	xcolorEval = !colorEval;

  if (juego.tablero[colorEval][PEON] && !juego.tablero[xcolorEval][PEON] && !FASE)
  {
	for (tempOrigenes = juego.tablero[colorEval][PEON];tempOrigenes;tempOrigenes ^= BITSET[escaqueOrigen])
	{
		escaqueOrigen = bitScanForwardBruijn(tempOrigenes);

		if ((signo[colorEval] * RANKS[escaqueRey[colorEval]]) <= (RANKS[escaqueOrigen] * signo[colorEval])) continue;

		
		if (FILES[escaqueOrigen] == 1)
		{
			if ((FILES[escaqueRey[colorEval]] == 2) && 
				(DISTANCIA[escaqueRey[colorEval]][POSCORONACION[colorEval][escaqueOrigen]] < 			
				 DISTANCIA[escaqueRey[xcolorEval]][POSCORONACION[colorEval][escaqueOrigen]]))
			{
				puntaje_f[colorEval] += PEON_PUEDE_CORONAR;
				return;
			}
			continue;
		} else if (FILES[escaqueOrigen] == 8) {

			if ((FILES[escaqueRey[colorEval]] == 7) && 
				(DISTANCIA[escaqueRey[colorEval]][POSCORONACION[colorEval][escaqueOrigen]] < 			
				 DISTANCIA[escaqueRey[xcolorEval]][POSCORONACION[colorEval][escaqueOrigen]]))
			{
				puntaje_f[colorEval] += PEON_PUEDE_CORONAR;
				return;
			}
			continue;
		}

		if (DISTANCIA[escaqueRey[colorEval]][escaqueOrigen] < DISTANCIA[escaqueRey[xcolorEval]][escaqueOrigen])
		{
			if ((signo[colorEval] * RANKS[escaqueRey[colorEval]]) > 
				(RANKS[escaqueOrigen] * (signo[colorEval])+1-2*colorEval)) 
			{
				puntaje_f[colorEval] += PEON_PUEDE_CORONAR;
				return;
			}

			if (colorEval) //Negro
			{
				if (RANKS[escaqueRey[colorEval]] == 3)
				{
					puntaje_f[colorEval] += PEON_PUEDE_CORONAR;
					return;
				}
			} else {

				if (RANKS[escaqueRey[colorEval]] == 6)
				{
					puntaje_f[colorEval] += PEON_PUEDE_CORONAR;
					return;
				}
			}
		}

		if ((RANKS[escaqueRey[colorEval]] == (RANKS[escaqueOrigen] + 1 - 2 * colorEval)) && 
			evalOposicionReyes(colorEval))
		{
			puntaje_f[colorEval] += PEON_PUEDE_CORONAR;
			return;
		} 
	} // fin for de peones
  } 

	if (!fase[xcolorEval] && peonesPasados[colorEval])
	{
		mapaPP = peonesPasadosMapaFila[colorEval];
		for (;mapaPP;mapaPP &= mapaPP - 1)		
		{
			filaPeon = lsb_8bit[mapaPP];
			esqPeonCabeza = CABEZA_ESQ(colorEval, FILEBIT[filaPeon+1] & juego.tablero[colorEval][PEON]);

			f[xcolorEval] = 
			  (CARRERA_PEONES[colorEval][juego.colorTurno][esqPeonCabeza] & juego.tablero[xcolorEval][REY]) !=
			  (CARRERA_PEONES[colorEval][!juego.colorTurno][esqPeonCabeza] & juego.tablero[xcolorEval][REY]);

			if (!(CARRERA_PEONES[colorEval][juego.colorTurno][esqPeonCabeza] & juego.tablero[xcolorEval][REY]))
			{
				dama_distancia = DISTANCIA[esqPeonCabeza][POSCORONACION[colorEval][esqPeonCabeza]];
				if (juego.tablero[colorEval][REY] & (colorEval?menos8dir[esqPeonCabeza]:mas8dir[esqPeonCabeza]))
				{
					if ((filaPeon==0) || (filaPeon==7)) dama_distancia=99;

					dama_distancia++;
				}

				if (colorEval)
				{
					if (RANKS[esqPeonCabeza] == 7) dama_distancia--;
				} else {
					if (RANKS[esqPeonCabeza] == 2) dama_distancia--;
				}

				if (dama_distancia < damer[colorEval])
				{
					corredor[colorEval] = BITSET[esqPeonCabeza];
					damer[colorEval]    = dama_distancia;	

				} else if (dama_distancia == damer[colorEval]) {

					corredor[colorEval] |= BITSET[esqPeonCabeza];
				}
			}
		}
	}
 } // fin for de colores

	if ((damer[BLANCO]==8) && (damer[NEGRO]==8)) 	return;
	if (f[BLANCO] & f[NEGRO]) 			return;

	if ((damer[BLANCO] < 8) && (damer[NEGRO]==8))
	{
		puntaje_f[BLANCO] += (PEON_PUEDE_CORONAR + ((5-damer[BLANCO])*10));
		return;

	} else if ((damer[NEGRO] < 8) && (damer[BLANCO]==8)) {

		puntaje_f[NEGRO] += (PEON_PUEDE_CORONAR + ((5-damer[NEGRO])*10));
		return;
	}

	if ((damer[BLANCO] < damer[NEGRO]) && f[BLANCO] && !f[NEGRO])
	{
		puntaje_f[BLANCO] += (PEON_PUEDE_CORONAR + ((5-damer[BLANCO])*10));
		return;

	} else if ((damer[NEGRO] < damer[BLANCO]) && f[NEGRO] && f[BLANCO]){

		puntaje_f[NEGRO] += (PEON_PUEDE_CORONAR + ((5-damer[NEGRO])*10));
		return;
	}

}


void evalPeones(COLOR colorEval)
{
	COLOR xcolorEval 	= !colorEval;
	BOOLEANO esAislado	= FALSO;
	BOOLEANO esPeonAbierto 	= FALSO;	
	BOOLEANO esAtrasado	= FALSO;
	BOOLEANO esDebil	= FALSO;
	BOOLEANO salidaXbreak	= 0;
	uint64 tempOrigenes	= 0; 
	int escaqueOrigen	= 0;
	int pos			= 0;
	int esq			= 0;
	int defensa		= 0;
	int ataque		= 0;
	uint64 temporal		= 0;
	uint64 peonMov[2]	= {0,0};


	peonesMapaFila[colorEval] 	 = 0;
	peonesPasadosMapaFila[colorEval] = 0;

	tempOrigenes = juego.tablero[colorEval][PEON];
	while (tempOrigenes)
	{
		escaqueOrigen = bitScanForwardBruijn(tempOrigenes);
		peonesMapaFila[colorEval] |= 1 << (FILES[escaqueOrigen]-1); 

		for (esq=escaqueOrigen;esq != POSRANK7[colorEval][escaqueOrigen];esq+=DIR[colorEval])
		{
			peonMov[colorEval] |= BITSET[esq];
			if (BITSET[esq+DIR[colorEval]] & mapaTodosPeones) break;
			defensa = cuentaBit(
				  mascaraCapturarPeon[esq+DIR[colorEval]][xcolorEval] & juego.tablero[colorEval][PEON]);

			ataque = cuentaBit(
				  mascaraCapturarPeon[esq+DIR[colorEval]][colorEval] & juego.tablero[xcolorEval][PEON]);

			if ((ataque-defensa) > 0) break;
		} 

		tempOrigenes ^= BITSET[escaqueOrigen];
	}

	tempOrigenes = juego.tablero[colorEval][PEON];
	while (tempOrigenes)
	{
		escaqueOrigen = bitScanForwardBruijn(tempOrigenes);

		//Marcar las plazas atacadas por los peones
		mapaPosAtacadas[colorEval] 		|= mascaraCapturarPeon[escaqueOrigen][colorEval];
		mapaPosAtacadasXPza[colorEval][PEON] 	|= mascaraCapturarPeon[escaqueOrigen][colorEval];

		//Evaluar la posicion del Peon en tablero
		puntaje_m[colorEval] += PEON_PUNTAJE_POS[colorEval][escaqueOrigen];
		puntaje_f[colorEval] += PEON_PUNTAJE_POS[colorEval][escaqueOrigen];

		//Peones Aislados
		esAislado  = FALSO;
		esAtrasado = FALSO;
		pos 	   = (colorEval?escaqueOrigen-8:escaqueOrigen+8);

		//Evaluar peones aislados
		if (!(AISLADO[escaqueOrigen] & juego.tablero[colorEval][PEON]))
		{
			puntaje_m[colorEval] -=  peon_aislado[MEDIO_JUEGO];
			puntaje_f[colorEval] -=  peon_aislado[FINAL_JUEGO];
		
			peonesDebiles[colorEval] |= BITSET[escaqueOrigen];
			esAislado 		  = VERDADERO;

		} else {  

			//Peones Debiles
			ataque  = 0;
			defensa = 0;
			temporal = peonMov[colorEval] & (colorEval?menos8dir[escaqueOrigen]:mas8dir[escaqueOrigen]);

			while (temporal)
			{
				esq = bitScanForwardBruijn(temporal);

				ataque  = cuentaBit(mascaraCapturarPeon[esq][colorEval] & juego.tablero[xcolorEval][PEON]);
				defensa = cuentaBit(mascaraCapturarPeon[esq][xcolorEval] & juego.tablero[colorEval][PEON]);;
				if (defensa && defensa >= ataque) 
				{
					salidaXbreak = VERDADERO;
					break;
				}

				temporal ^= BITSET[esq];
			}

			if (!salidaXbreak)
			{
				if (!(mascaraCapturarPeon[escaqueOrigen][xcolorEval] & peonMov[colorEval]))
				{
					esDebil = VERDADERO;
					peonesDebiles[colorEval] |= BITSET[escaqueOrigen];
					puntaje_m[colorEval] -= DESCUENTO_PEON_DEBIL_M;
					puntaje_f[colorEval] -= DESCUENTO_PEON_DEBIL_F;
				}
			} 

			//Peones Dobles
			if ((juego.tablero[colorEval][PEON] ^ BITSET[escaqueOrigen]) & FILEMASK[escaqueOrigen])
			{
				//DESCUENTO_PEONES_DOBLADOS[escaqueOrigen];
				puntaje_m[colorEval] -= peon_doblado[MEDIO_JUEGO]; 
				puntaje_f[colorEval] -= peon_doblado[FINAL_JUEGO]; 
			}

			//Peon Duo
			if (MASCARA_PEON_CC[escaqueOrigen] & juego.tablero[colorEval][PEON])
			{
				puntaje_m[colorEval] += peon_duo[MEDIO_JUEGO]; 
				puntaje_f[colorEval] += peon_duo[FINAL_JUEGO];
			} 
		}

		//Comprobar si es peon abierto
		if (FILEMASK[escaqueOrigen] & juego.tablero[xcolorEval][PEON])
			esPeonAbierto = FALSO;	
		else
			esPeonAbierto = VERDADERO;	

		if (esPeonAbierto)
		{
			if (esDebil)
			{
				puntaje_m[colorEval] -= DESCUENTO_PEON_DEBIL_M_PA;
			}

			if (esAislado)
			{
				puntaje_m[colorEval] -=  peon_aislado[MEDIO_JUEGO]/2;  
				puntaje_f[colorEval] -=  peon_aislado[FINAL_JUEGO]/2;  
			}

			//Evaluar peones pasados	
			if (!(PASADO[colorEval][escaqueOrigen] & juego.tablero[xcolorEval][PEON]))
			{

				peonesPasados[colorEval]         |= BITSET[escaqueOrigen];
				peonesPasadosMapaFila[colorEval] |= 1 << (FILES[escaqueOrigen]-1); 


			} else { //Peon Candidato a Pasado

				if ((AISLADO[escaqueOrigen] & juego.tablero[colorEval][PEON]) && 
				    !(mascaraCapturarPeon[escaqueOrigen][colorEval] & juego.tablero[xcolorEval][PEON]))
				{
					for (esq=escaqueOrigen;esq != POSRANK7[colorEval][escaqueOrigen];esq+=DIR[colorEval])
					{
						if (BITSET[esq+DIR[colorEval]] & mapaTodosPeones) break;

						defensa = 
						cuentaBit(mascaraCapturarPeon[esq][xcolorEval] & peonMov[colorEval]);

						ataque = 
						cuentaBit(mascaraCapturarPeon[esq][colorEval] & juego.tablero[xcolorEval][PEON]);
					
						if (ataque) break;
					}

					if (ataque <= defensa)
					{
						if (!(PASADO[colorEval][esq+DIR[colorEval]] & juego.tablero[xcolorEval][PEON]))
						{
							puntaje_m[colorEval] += 
							  peon_pasado_candidato[MEDIO_JUEGO][colorEval][RANKS[escaqueOrigen]];
							puntaje_f[colorEval] += 
							  peon_pasado_candidato[FINAL_JUEGO][colorEval][RANKS[escaqueOrigen]];

							peonesCandidatos[colorEval] |= BITSET[escaqueOrigen];
						}
					}
				}
			}
		}

		if (RANKS[escaqueOrigen]==(colorEval?3:6) && 
			BITSET[escaqueOrigen+DIR[colorEval]] & juego.tablero[xcolorEval][PEON] && 
			((FILES[escaqueOrigen] < 8 && BITSET[escaqueOrigen-7+16*colorEval] & juego.tablero[colorEval][PEON] && 
		          !(PEON_ESCONDIDO_D[colorEval][FILES[escaqueOrigen]] & juego.tablero[xcolorEval][PEON])) ||
	              (FILES[escaqueOrigen] > 1 && BITSET[escaqueOrigen-9+16*colorEval] & juego.tablero[colorEval][PEON] && 
		          !(PEON_ESCONDIDO_I[colorEval][FILES[escaqueOrigen]] & juego.tablero[xcolorEval][PEON]))))	
		{
			puntaje_m[colorEval] += peon_pasado_escondido[MEDIO_JUEGO];
			puntaje_f[colorEval] += peon_pasado_escondido[FINAL_JUEGO];
		}

		tempOrigenes ^= BITSET[escaqueOrigen];
	} // Fin Peon 
}

void evalCaballo(COLOR colorEval)
{
	COLOR xcolorEval 	= !colorEval;
	uint64 tempOrigenes	= 0; 
	uint64 tempDestinos	= 0;
	int escaqueOrigen	= 0;
	int i			= 0;

	if (colorEval)
		juego.destinos	= (juego.desOcupados | juego.blancos);
	else
		juego.destinos	= (juego.desOcupados | juego.negros);

	tempOrigenes = juego.tablero[colorEval][CABALLO];
	while (tempOrigenes)
	{
		escaqueOrigen = bitScanForwardBruijn(tempOrigenes);

		//Evaluar la posicion del Caballo en el tablero
		puntaje_m[colorEval] += (CABALLO_PUNTAJE_POS[colorEval][escaqueOrigen]);
		puntaje_f[colorEval] += (CABALLO_PUNTAJE_POS_FINAL[escaqueOrigen]);

		//Ajuste del caballo segun el numero de peones
		puntaje_m[colorEval] += adjuste_caballo[nPeones[colorEval]];
		puntaje_f[colorEval] += adjuste_caballo[nPeones[colorEval]];
//*/
		//tropismo
		if (esPeligroso[colorEval])
		{
			tropismo[colorEval] += CABALLO_TROPISMO[DISTANCIA[escaqueOrigen][escaqueRey[xcolorEval]]];
		}

		tempDestinos   		 		 = genCaballoAtaqueTablero(escaqueOrigen, juego);
		mapaPosAtacadas[colorEval] 		|= tempDestinos;
		mapaPosAtacadasXPza[colorEval][CABALLO]	|= tempDestinos;

		//Puesto de Avanzada
		i = CABALLO_PUESTO_AVANZADA[colorEval][escaqueOrigen];
		if (!((PASADO[colorEval][escaqueOrigen] & AISLADO[escaqueOrigen]) & juego.tablero[xcolorEval][PEON]) && i)
		{
			puntaje_m[colorEval] += i;
			puntaje_f[colorEval] += i;

			if (mascaraCapturarPeon[escaqueOrigen][xcolorEval] & juego.tablero[colorEval][PEON])
			{
				puntaje_m[colorEval] += i/2;
				puntaje_f[colorEval] += i/2;
				if (!juego.tablero[xcolorEval][CABALLO])
				{
					if ((BITSET[escaqueOrigen] & ESCAQUES_NEGROS) && 
						!(juego.tablero[xcolorEval][ALFIL] & ESCAQUES_NEGROS))
					{
						puntaje_m[colorEval] += i;
						puntaje_f[colorEval] += i;
						
					} else if ((BITSET[escaqueOrigen] & ESCAQUES_BLANCOS) && 
						!(juego.tablero[xcolorEval][ALFIL] & ESCAQUES_BLANCOS)) {
						puntaje_m[colorEval] += i;
						puntaje_f[colorEval] += i;
					}
				}
			}
		}

		puntaje_m[colorEval] += movilidadCaballo[cuentaBit(tempDestinos)]; 
		puntaje_f[colorEval] += movilidadCaballo[cuentaBit(tempDestinos)]; 

		tempOrigenes ^= BITSET[escaqueOrigen];
	}

}

void evalAlfil(COLOR colorEval)
{
	COLOR xcolorEval 	= !colorEval;
	uint64 tempOrigenes	= 0; 
	uint64 tempDestinos	= 0;
	int escaqueOrigen	= 0;
	int i			= 0;
	int esqA7[2]		= {48,8};
	int esqB6[2]		= {41,17};
	int esqG6[2]		= {46,22};
	int indicador;

	if (colorEval)
		juego.destinos	= (juego.desOcupados | juego.blancos);
	else
		juego.destinos	= (juego.desOcupados | juego.negros);

	tempOrigenes = juego.tablero[colorEval][ALFIL];
	while (tempOrigenes)
	{
		escaqueOrigen = bitScanForwardBruijn(tempOrigenes);

		//Evaluar la posicion del Alfil en el tablero
		puntaje_m[colorEval] += (ALFIL_PUNTAJE_POS[colorEval][escaqueOrigen]);
		puntaje_f[colorEval] += (ALFIL_PUNTAJE_POS_FINAL[escaqueOrigen]);

		tempDestinos  		 		 = genAlfilAtaqueTablero(escaqueOrigen, juego);
		mapaPosAtacadas[colorEval] 		|= tempDestinos;
		mapaPosAtacadasXPza[colorEval][ALFIL]	|= tempDestinos;

		//Con peones Ganadores
		if ((mapaTodosPeones & mascaraFGH) && (mapaTodosPeones & mascaraABC))
		{
			puntaje_m[colorEval] += alfil_peon_ganador[MEDIO_JUEGO];
			puntaje_f[colorEval] += alfil_peon_ganador[FINAL_JUEGO];
		}

		//Puesto de Avanzada
		i = ALFIL_PUESTO_AVANZADA[colorEval][escaqueOrigen];
		if (i)
		{
		   if (i>0)
		   {
			if (!((PASADO[colorEval][escaqueOrigen] & AISLADO[escaqueOrigen]) & juego.tablero[xcolorEval][PEON]))
			{
				puntaje_m[colorEval] += i;
				puntaje_f[colorEval] += i;

				if (mascaraCapturarPeon[escaqueOrigen][xcolorEval] & juego.tablero[colorEval][PEON])
				{
					puntaje_m[colorEval] += i/2;
					puntaje_f[colorEval] += i/2;

					if (!juego.tablero[xcolorEval][CABALLO])
					{
						if ((BITSET[escaqueOrigen] & ESCAQUES_NEGROS) && 
							!(juego.tablero[xcolorEval][ALFIL] & ESCAQUES_NEGROS))
						{
							puntaje_m[colorEval] += i;
							puntaje_f[colorEval] += i;
						
						} else if ((BITSET[escaqueOrigen] & ESCAQUES_BLANCOS) && 
							!(juego.tablero[xcolorEval][ALFIL] & ESCAQUES_BLANCOS)) {
							puntaje_m[colorEval] += i;
							puntaje_f[colorEval] += i;
						}
					}

				}
			}

		    } else { //Alfiles Atrapados

			if (escaqueOrigen == esqA7[colorEval])
			{
				if (BITSET[esqB6[colorEval]] & juego.tablero[xcolorEval][PEON])
				{
					puntaje_m[colorEval] -= DESCUENTO_ALFIL_ATRAPADO;
					puntaje_f[colorEval] -= DESCUENTO_ALFIL_ATRAPADO;
				}

			} else if (BITSET[esqG6[colorEval]] & juego.tablero[xcolorEval][PEON]) {

					puntaje_m[colorEval] -= DESCUENTO_ALFIL_ATRAPADO;
					puntaje_f[colorEval] -= DESCUENTO_ALFIL_ATRAPADO;
			}
                    }
		}

		//tropismo
		if (esPeligroso[colorEval])
		{
			indicador = ((MATRIZ_MOV_ALFIL[escaqueOrigen] & entornoRey[xcolorEval]) &&
			genAlfilAtaqueTableroX(escaqueOrigen, 
			(juego.ocupados^juego.tablero[colorEval][DAMA]),
			entornoRey[xcolorEval])) ?
			1 : DISTANCIA[escaqueOrigen][escaqueRey[xcolorEval]];

			tropismo[colorEval] += ALFIL_TROPISMO[indicador];
		}

		puntaje_m[colorEval] += movilidadAlfil[cuentaBit(tempDestinos)]; 
		puntaje_f[colorEval] += movilidadAlfil[cuentaBit(tempDestinos)]; 

		tempOrigenes ^= BITSET[escaqueOrigen];
	}
}

void evalTorre(COLOR colorEval)
{
	COLOR xcolorEval 	= !colorEval;
	uint64 tempOrigenes	= 0; 
	uint64 tempDestinos	= 0;
	int escaqueOrigen	= 0;
	int indicador;

	if (colorEval)
		juego.destinos	= (juego.desOcupados | juego.blancos);
	else
		juego.destinos	= (juego.desOcupados | juego.negros);

	tempOrigenes = juego.tablero[colorEval][TORRE];
	while (tempOrigenes)
	{
		escaqueOrigen = bitScanForwardBruijn(tempOrigenes);

		tempDestinos  		 		 = genTorreAtaqueTablero(escaqueOrigen, juego);
		mapaPosAtacadas[colorEval] 		|= tempDestinos;
		mapaPosAtacadasXPza[colorEval][TORRE]	|= tempDestinos;

		//Evaluar la posicion de la Torre en el tablero
		puntaje_m[colorEval] += (TORRE_PUNTAJE_POS[escaqueOrigen]);
		puntaje_f[colorEval] += (TORRE_PUNTAJE_POS[escaqueOrigen]);

		//Ajuste de la torre segun el numero de peones
		puntaje_m[colorEval] += adjuste_torre[nPeones[colorEval]];
		puntaje_f[colorEval] += adjuste_torre[nPeones[colorEval]];


		//tropismo
		if (esPeligroso[colorEval])
		{
			indicador = ((MATRIZ_MOV_TORRE[escaqueOrigen] & entornoRey[xcolorEval]) &&
			genTorreAtaqueTableroX(escaqueOrigen, 
			(juego.ocupados^(juego.tablero[colorEval][DAMA]|juego.tablero[colorEval][TORRE])),
			entornoRey[xcolorEval])) ?
			1 : DISTANCIA[escaqueOrigen][escaqueRey[xcolorEval]];
			tropismo[colorEval] += TORRE_TROPISMO[indicador];
		}


		//Evaluar torre en columna semi-abierta
		if (!(juego.tablero[colorEval][PEON] & FILEMASK[escaqueOrigen]))
		{
			if (!(juego.tablero[xcolorEval][PEON] & FILEMASK[escaqueOrigen]))
			{
				puntaje_m[colorEval] += torre_fila_abierta[MEDIO_JUEGO];
				puntaje_f[colorEval] += torre_fila_abierta[FINAL_JUEGO];

			} else {

				puntaje_m[colorEval] += torre_media_fila_abierta[MEDIO_JUEGO];
				puntaje_f[colorEval] += torre_media_fila_abierta[FINAL_JUEGO];
			}
		}

		//Evaluar Torre 7 Rank 
		if (colorEval)
		{
			if ((RANKS[escaqueOrigen] == 8) && (RANKS[escaqueRey[colorEval]]==8))
			{
				if ((FILES[escaqueRey[colorEval]] > 5) && (FILES[escaqueOrigen] > FILES[escaqueRey[colorEval]]))
				{
					puntaje_m[colorEval] -= DESCUENTO_TORRE_ATRAPADA;
					puntaje_f[colorEval] -= DESCUENTO_TORRE_ATRAPADA;

				} else if ((FILES[escaqueRey[colorEval]] < 4) && 
						(FILES[escaqueOrigen] < FILES[escaqueRey[colorEval]])) {

					puntaje_m[colorEval] -= DESCUENTO_TORRE_ATRAPADA;
					puntaje_f[colorEval] -= DESCUENTO_TORRE_ATRAPADA;
				}
				
			} else if (RANKS[escaqueOrigen] == 2){

				puntaje_m[colorEval] += torre_7rank[MEDIO_JUEGO];
				puntaje_f[colorEval] += torre_7rank[FINAL_JUEGO];
			}

		} else {

			if ((RANKS[escaqueOrigen] == 1) && (RANKS[escaqueRey[colorEval]] == 1))
			{
				if ((FILES[escaqueRey[colorEval]] > 5) && (FILES[escaqueOrigen] > FILES[escaqueRey[colorEval]]))
				{
					puntaje_m[colorEval] -= DESCUENTO_TORRE_ATRAPADA;
					puntaje_f[colorEval] -= DESCUENTO_TORRE_ATRAPADA;

				} else if ((FILES[escaqueRey[colorEval]] < 4) && 
						(FILES[escaqueOrigen] < FILES[escaqueRey[colorEval]])) {

					puntaje_m[colorEval] -= DESCUENTO_TORRE_ATRAPADA;
					puntaje_f[colorEval] -= DESCUENTO_TORRE_ATRAPADA;
				}
				
			} else if (RANKS[escaqueOrigen] == 7){

				puntaje_m[colorEval] += torre_7rank[MEDIO_JUEGO];
				puntaje_f[colorEval] += torre_7rank[FINAL_JUEGO];
			}
		}

		// Analisis Movilidad
		puntaje_m[colorEval] += movilidadTorre[cuentaBit(tempDestinos)]; 
		puntaje_f[colorEval] += movilidadTorreFinal[cuentaBit(tempDestinos)]; 

		tempOrigenes ^= BITSET[escaqueOrigen];
	}
}

void evalDama(COLOR colorEval)
{
	COLOR xcolorEval 	= !colorEval;
	uint64 tempOrigenes	= 0; 
	uint64 tempDestinos	= 0;
	int escaqueOrigen	= 0;
	int valor		= 0;

	if (colorEval)
		juego.destinos	= (juego.desOcupados | juego.blancos);
	else
		juego.destinos	= (juego.desOcupados | juego.negros);


	tempOrigenes = juego.tablero[colorEval][DAMA];
	while (tempOrigenes)
	{
		escaqueOrigen = bitScanForwardBruijn(tempOrigenes);

		//Evaluar la posicion de la dama en el tablero
		puntaje_m[colorEval] += (DAMA_PUNTAJE_POS[colorEval][escaqueOrigen]);
		puntaje_f[colorEval] += (DAMA_PUNTAJE_POS_FINAL[escaqueOrigen]);

		tempDestinos  		 		 = genDamaAtaqueTablero(escaqueOrigen, juego);
		mapaPosAtacadas[colorEval] 		|= tempDestinos;
		mapaPosAtacadasXPza[colorEval][DAMA]	|= tempDestinos;

		//tropismo
		if (esPeligroso[colorEval])
		{
			tropismo[colorEval] += DAMA_TROPISMO[DISTANCIA[escaqueOrigen][escaqueRey[xcolorEval]]];
			valor = 8 - (RankDistancia(escaqueOrigen, escaqueRey[xcolorEval]) + 
				     FileDistancia(escaqueOrigen, escaqueRey[xcolorEval]));

			puntaje_m[colorEval] += valor;
			puntaje_f[colorEval] += valor;
		}

		puntaje_m[colorEval] += movilidadDama[cuentaBit(tempDestinos)];
		puntaje_f[colorEval] += movilidadDamaFinal[cuentaBit(tempDestinos)];

		tempOrigenes ^= BITSET[escaqueOrigen];
	}
}

void ini_material()
{
	int mayores, menores, balance;
	static int b[17] = { 0, 40, 40, 35, 30, 24, 16, 12, 10, 8, 7, 6, 5, 4, 3, 2, 1 };


	if (juego.material_total < 0)
	{
		puntaje_m[NEGRO]   = (-juego.material_total);
		puntaje_f[NEGRO]   = (-juego.material_total);

	} else if (juego.material_total > 0) { 

		puntaje_m[BLANCO]  = juego.material_total;
		puntaje_f[BLANCO]  = juego.material_total;

	}

#ifdef VERVALORES
	printf("******Material Base*******\n");
	printf("m[B]=%d\n",puntaje_m[BLANCO]);
	printf("f[B]=%d\n",puntaje_f[BLANCO]);
	printf("m[N]=%d\n",puntaje_m[NEGRO]);
	printf("f[N]=%d\n",puntaje_f[NEGRO]);
#endif


	if (juego.colorTurno)
	{
		puntaje_m[NEGRO]  += BONO_TURNO[MEDIO_JUEGO];
		puntaje_f[NEGRO]  += BONO_TURNO[FINAL_JUEGO];

	} else { 
		puntaje_m[BLANCO] += BONO_TURNO[MEDIO_JUEGO];
		puntaje_f[BLANCO] += BONO_TURNO[FINAL_JUEGO];
	}

#ifdef VERVALORES
	printf("******Material Base + Bono Turno*******\n");
	printf("m[B]=%d\n",puntaje_m[BLANCO]);
	printf("f[B]=%d\n",puntaje_f[BLANCO]);
	printf("m[N]=%d\n",puntaje_m[NEGRO]);
	printf("f[N]=%d\n",puntaje_f[NEGRO]);
#endif


	//Ajuste de Balance de Piezas Mayores (Dama, Torre) y Menores (Alfil, Caballo)

	mayores = 4 + PIEZAS_MAYORES[BLANCO] - PIEZAS_MAYORES[NEGRO]; 
	menores = 4 + PIEZAS_MENORES[BLANCO] - PIEZAS_MENORES[NEGRO];

	mayores = MAXIMO(MINIMO(mayores,8),0);
	menores = MAXIMO(MINIMO(menores,8),0);
	balance = imbalance[mayores][menores];

	if (balance > 0)
	{
		puntaje_m[BLANCO] += balance;
		puntaje_f[BLANCO] += balance;

	} else if (balance < 0) {

		puntaje_m[NEGRO] += -balance;
		puntaje_f[NEGRO] += -balance;
	}

#ifdef VERVALORES
	printf("******Material Base + Bono Turno + imbalace*******\n");
	printf("m[B]=%d\n",puntaje_m[BLANCO]);
	printf("f[B]=%d\n",puntaje_f[BLANCO]);
	printf("m[N]=%d\n",puntaje_m[NEGRO]);
	printf("f[N]=%d\n",puntaje_f[NEGRO]);
	printf("balance[%d][%d]=%d\n",mayores,menores,balance);
#endif


	if (nAlfil[BLANCO] > 1)
	{
		puntaje_m[BLANCO] += 38;
		puntaje_f[BLANCO] += 56;
	}	

	if (nAlfil[NEGRO] > 1)
	{
		puntaje_m[NEGRO] += 38;
		puntaje_f[NEGRO] += 56;
	}

#ifdef VERVALORES
	printf("******Material Base + Bono Turno + imbalace + ajuste alfil*******\n");
	printf("m[B]=%d\n",puntaje_m[BLANCO]);
	printf("f[B]=%d\n",puntaje_f[BLANCO]);
	printf("m[N]=%d\n",puntaje_m[NEGRO]);
	printf("f[N]=%d\n",puntaje_f[NEGRO]);
#endif
	
/*
	if (!balance && ((!PIEZAS_MAYORES[BLANCO] && PIEZAS_MENORES[BLANCO]==1) ||
			 (!PIEZAS_MAYORES[NEGRO]  && PIEZAS_MENORES[NEGRO] ==1)))
*/
//	if (!balance && !PIEZAS_MAYORES[BLANCO] && PIEZAS_MENORES[BLANCO]==1)
	if (!balance && ((!PIEZAS_MAYORES[BLANCO] && PIEZAS_MENORES[BLANCO]==1) ||
			 (!PIEZAS_MAYORES[NEGRO]  && PIEZAS_MENORES[NEGRO] ==1)))
	{
		if ((mapaTodosPeones & mascaraFGH) && (mapaTodosPeones & mascaraABC))
		{
			balance = b[totalPeones];
			if (nAlfil[BLANCO])
			{
				puntaje_m[BLANCO] += balance;
				puntaje_f[BLANCO] += balance;
			}

			if (nAlfil[NEGRO])
			{
				puntaje_m[NEGRO] += balance;
				puntaje_f[NEGRO] += balance;
			}

		}
	}
#ifdef VERVALORES
	printf("******Material Base + Bono Turno + imbalace + ajuste alfil + alfil finales*******\n");
	printf("m[B]=%d\n",puntaje_m[BLANCO]);
	printf("f[B]=%d\n",puntaje_f[BLANCO]);
	printf("m[N]=%d\n",puntaje_m[NEGRO]);
	printf("f[N]=%d\n",puntaje_f[NEGRO]);
#endif

}

void evalRey(COLOR colorEval)
{
	COLOR xcolorEval = !colorEval;
	int defectos	 = 0;
	BOOLEANO E_OO; 			
	BOOLEANO E_OOO;
	BOOLEANO HUBO_ENROQUE;

	if (mapaTodosPeones)
	{
		if ((mapaTodosPeones & mascaraEFGH) && (mapaTodosPeones & mascaraABCD))
		{
			puntaje_f[colorEval] += REY_PUNTAJE_RESGUARDO[colorEval][escaqueRey[colorEval]];

		} else if (mapaTodosPeones & mascaraEFGH) {

			puntaje_f[colorEval] += REY_PUNTAJE_RESGUARDO_R[colorEval][escaqueRey[colorEval]];

		} else {

			puntaje_f[colorEval] += REY_PUNTAJE_RESGUARDO_D[colorEval][escaqueRey[colorEval]];
		}
	}

	if (esPeligroso[xcolorEval])
	{
		defectos = 0;
		HUBO_ENROQUE = (colorEval)?juego.ENROQUEN:juego.ENROQUEB;
		if (HUBO_ENROQUE) //Enroque
		{
			if (FILES[escaqueRey[colorEval]] >= 5)
			{
				if (FILES[escaqueRey[colorEval]] > 5)
				{
					defectos = peonDefectos[colorEval][7];
				} else {
					defectos = peonDefectos[colorEval][5];
				}

			} else {

				if (FILES[escaqueRey[colorEval]] < 4) {
					defectos = peonDefectos[colorEval][2];
				} else {
					defectos = peonDefectos[colorEval][4];
				}
			}

		} else { //sin haber hecho enroque
			
			E_OO  = colorEval?juego.OON :juego.OOB;			
			E_OOO = colorEval?juego.OOON:juego.OOOB;

			if (E_OO && E_OOO)
			{
				defectos = MINIMO(MINIMO(peonDefectos[colorEval][7], peonDefectos[colorEval][5]),
						peonDefectos[colorEval][2]);
			} else if (!E_OOO && E_OO) {

				defectos = MINIMO(peonDefectos[colorEval][7], peonDefectos[colorEval][5]);
	
			} else {

				defectos = MINIMO(peonDefectos[colorEval][2], peonDefectos[colorEval][5]);
			}

			if (defectos < 3) defectos = 3;
		}

		if (tropismo[xcolorEval] < 0) tropismo[xcolorEval]  = 0;
		if (tropismo[xcolorEval] > 15) tropismo[xcolorEval] = 15;
		if (defectos > 15) defectos = 15;
		if (defectos < 0)  defectos = 0;

		puntaje_m[colorEval] -= MATRIZ_SEGURIDAD_REY[defectos][tropismo[xcolorEval]];
	}
}

BOOLEANO evalOposicionReyes(COLOR colorEval)
{
	COLOR xcolorEval   = !colorEval;
	int distancia_file  = abs(FILES[escaqueRey[colorEval]] - FILES[escaqueRey[xcolorEval]]);
	int distancia_rank = abs(RANKS[escaqueRey[colorEval]] - RANKS[escaqueRey[xcolorEval]]);

	if (distancia_rank < 2) return VERDADERO;

	if (juego.colorTurno==colorEval)
	{
		if (distancia_rank & 1) distancia_rank--;
		if (distancia_file & 1) distancia_file--;
	}

	if (!(distancia_file & 1) && !(distancia_rank & 1)) return VERDADERO;

	return FALSO;
}

void evalFilaRey(COLOR colorEval)
{
	COLOR xcolorEval= !colorEval;

	int fila[5] 	= {2, 4, 5, 7, 0}; //B,D,E,G
	int fpivo	= 0;
	int i		= 0;
	int final 	= 0;
	int esqA2[2]	= {8, 48};
	int esqA3[2]	= {16, 40};

	for(;fila[i];i++)
	{
		final = fila[i]+2;
		fpivo = fila[i]-1;
		for(;fpivo<final;fpivo++)
		{
			if (!(FILEBIT[fpivo] & mapaTodosPeones))
			{
				peonDefectos[colorEval][fila[i]] += defectos_fila_abierta[fpivo];

			} else {
				
				if (!(FILEBIT[fpivo] & juego.tablero[xcolorEval][PEON]))
				{
					peonDefectos[colorEval][fila[i]] += defectos_media_fila_abierta[fpivo]/2;
				} else {
					peonDefectos[colorEval][fila[i]] += defectos_peon[colorEval][
						RANKS[CABEZA_ESQ(xcolorEval, FILEBIT[fpivo] & juego.tablero[xcolorEval][PEON])]];
				}

				if (!(FILEBIT[fpivo] & juego.tablero[colorEval][PEON]))
				{
					peonDefectos[colorEval][fila[i]] += defectos_media_fila_abierta[fpivo];
				} else {
			
					if (!(juego.tablero[colorEval][PEON] & BITSET[esqA2[colorEval]+fpivo-1]))
					{
						peonDefectos[colorEval][fila[i]]++;
						if (!(juego.tablero[colorEval][PEON] & BITSET[esqA3[colorEval]+fpivo-1]))
						{
							peonDefectos[colorEval][fila[i]]++;
						}
					}
				}
			}
		}
	} 
}

BOOLEANO esPosibilidadVictoria(COLOR colorEval)
{
	COLOR xcolorEval= !colorEval;
	int erd, ard, promod, esqPromo, esq;

	if (!juego.tablero[colorEval][PEON])
	{
		if (fase[colorEval] < 4) 
			return FALSO;

		if (((fase[colorEval] - fase[xcolorEval]) < 4) && (juego.tablero[xcolorEval][REY] & mascaraSinPeonTorre))
			return FALSO;
	}

	if (juego.tablero[colorEval][PEON] && !(juego.tablero[colorEval][PEON] & mascaraSinPeonTorre))
	{
		do {
			if ((fase[colorEval] > 3) || ((fase[colorEval] == 3) && juego.tablero[colorEval][CABALLO]))
				continue;

			if (!fase[colorEval]  && 
				(juego.tablero[colorEval][PEON] & FILEBIT[1]) && (juego.tablero[colorEval][PEON] & FILEBIT[8]))
				continue;

			if (juego.tablero[colorEval][ALFIL])
			{
				if (!fase[xcolorEval])
				{
					if (juego.tablero[colorEval][ALFIL] & ESCAQUES_NEGROS)
					{
						if (FILEBIT[esquinaNegra[colorEval]] & juego.tablero[colorEval][PEON])
							continue;

					} else if (FILEBIT[esquinaBlanca[colorEval]] & juego.tablero[colorEval][PEON]) {
						continue;
					}

				} else {
					continue;
				}
			}

			if (!(juego.tablero[colorEval][PEON] & FILEBIT[1]) || !(juego.tablero[colorEval][PEON] & FILEBIT[8]))
			{
				if (juego.tablero[colorEval][PEON] & FILEBIT[1])
				{
					esqPromo = POSCORONACION[colorEval][0];
				} else {
					esqPromo = POSCORONACION[colorEval][7];
				}

				erd = DISTANCIA[escaqueRey[xcolorEval]][esqPromo] - (juego.colorTurno != colorEval);

				if (erd < 2)
				{
					return FALSO;

				} else {
					ard = DISTANCIA[escaqueRey[colorEval]][esqPromo] - (juego.colorTurno == colorEval);

					promod = DISTANCIA[
					CABEZA_ESQ(colorEval, juego.tablero[colorEval][PEON] & FILEBIT[FILES[esqPromo]])][esqPromo]
					- (juego.colorTurno == colorEval);

					if (erd <= ard && (erd-1) <= promod) return FALSO;
				}
			}

		} while(0);
	}

	if (juego.tablero[colorEval][PEON]) return VERDADERO;

	if (!juego.tablero[colorEval][PEON] && fase[colorEval]==6 && fase[xcolorEval]==3 && 
	    (juego.tablero[colorEval][CABALLO] || !juego.tablero[xcolorEval][CABALLO]))
		return FALSO;


	if (!juego.tablero[colorEval][PEON] && fase[colorEval]==6 && !juego.tablero[colorEval][ALFIL] 
			&& !fase[xcolorEval] && !juego.tablero[xcolorEval][PEON])
		return FALSO;

	if (nPeones[colorEval]==1 && !nPeones[xcolorEval] &&
	   ((fase[colorEval]==5 && fase[xcolorEval]==5) || (fase[colorEval]==9 && fase[xcolorEval]==9)))
	{
		esq = bitScanForwardBruijn(juego.tablero[colorEval][PEON]);

		if ((FileDistancia(escaqueRey[xcolorEval],esq) < 2) && 
			de_Enfrente(colorEval,RANKS[escaqueRey[xcolorEval]], RANKS[esq]))
		{
			return FALSO;		

		} else if ((FileDistancia(escaqueRey[colorEval],esq ) > 1) || 
			de_Atras(colorEval,RANKS[escaqueRey[colorEval]], RANKS[esq])) {

			return FALSO;		
		}
	}

	return VERDADERO;
}

void evaluarMate(COLOR colorEval)
{
	COLOR xcolorEval = !colorEval;
	int puntaje	 = 0;

	if (!fase[xcolorEval] && PIEZAS_MENORES[colorEval]==2 && nAlfil[colorEval]==1)
	{
		if (juego.tablero[colorEval][ALFIL] & ESCAQUES_NEGROS)
		{
			puntaje = alfil_mate_escaques_negros[escaqueRey[xcolorEval]];

		} else {

			puntaje = alfil_mate_escaques_blancos[escaqueRey[xcolorEval]];
		}

	} else {
	
		puntaje  = mate[escaqueRey[xcolorEval]];
		puntaje -= (DISTANCIA[escaqueRey[xcolorEval]][escaqueRey[colorEval]]-3) * REY_TROPISMO;
	}

	puntaje_m[colorEval] += puntaje;
	puntaje_f[colorEval] += puntaje;
}

int transformarSegunEmpate(int puedeGanar, int puntaje)
{
	if ((fase[BLANCO] < 9) && (fase[NEGRO] < 9))
	{
		if ((nAlfil[BLANCO]==1) && (nAlfil[NEGRO]==1))
		{
			if (((juego.tablero[BLANCO][ALFIL] & ESCAQUES_NEGROS)  && (juego.tablero[NEGRO][ALFIL] & ESCAQUES_BLANCOS)) ||
			    ((juego.tablero[BLANCO][ALFIL] & ESCAQUES_BLANCOS) && (juego.tablero[NEGRO][ALFIL] & ESCAQUES_NEGROS ))) 
			{
				if (fase[BLANCO]==3 && fase[NEGRO]==3 &&
				   ((nPeones[BLANCO] < 4 && nPeones[NEGRO] < 4) ||
				    abs(fase[BLANCO]-fase[NEGRO])<2))
				{
					puntaje = puntaje / 2;
	
				} else if (fase[BLANCO] == fase[NEGRO]) {

					puntaje = 3 * puntaje/4;

				}
			}
		} 
	}

	if (puedeGanar != 3)
	{
		if (puedeGanar != 1 && puntaje > 0)
		{
			puntaje = puntaje / 4;

		} else if (puedeGanar != 2 && puntaje < 0) {

			puntaje = puntaje / 4;
		} 
	}

	if (juego.reglaCincuentaMov>80)
	{
		puntaje = puntaje * ((101-juego.reglaCincuentaMov)/20);
	}

	return puntaje;

}

#endif
