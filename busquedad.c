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

#ifndef BUSQUEDAD_C
#define BUSQUEDAD_C

void limpiarAntesDeBusqueda()
{
	contadorNodos		= 0;
	QcontadorNodos		= 0;
	nodoRaiz		= 0;
	memset(juego.historicoMovMatadores, -INFINITO, largoResetMatadores); 
	maxHistorial[0]		= 0;
	maxHistorial[1]		= 0;
	siguePV 		= VERDADERO;

}

void limpiarBufffer()
{

	memset(juego.Buffer_MOV_INDEXCAPAS, 	0, 		sizeof(uint32)		* MAX_CAPAS);
	memset(juego.Buffer_MOV, 		0, 		sizeof(uint32)		* MAX_BUFF_MOV);
	memset(juego.Buffer_MOV_PONDERACION,	-INFINITO, 	sizeof(int)		* MAX_BUFF_MOV);
	memset(juego.triangularLargo, 		0, 		sizeof(int)		* MAX_CAPAS);
	memset(juego.triangularArray, 		0, 		sizeof(MOVIMIENTO)	* MAX_CAPAS*MAX_CAPAS);
	memset(juego.heuristicaNegra,		0, 		sizeof(uint32)		* 64*64);
	memset(juego.heuristicaBlanca,		0, 		sizeof(uint32)		* 64*64);
	maxHistorial[0]		= 0;
	maxHistorial[1]		= 0;
	siguePV 		= VERDADERO;

}


MOVIMIENTO pensarRapido()
{
	ABORTABLE 		= FALSO;
	int valoracion		= 0;
	int valoracionAnterior	= 0;
	int i_pv		= 0;
	uint64 tiempo		= 0;
	MOVIMIENTO movResul	= 0;
	MOVIMIENTO movGafo	= 0;
	int i;
	BOOLEANO esJaque;
	int contMov		= 0;
	int i_unicoMov		= 0;	
	int profundidadPrint	= 0;		 

	int alfa		= -INFINITO;
	int beta		=  INFINITO;

if (tipoDeBusqueda == TIPO_BUSQUEDA_NORMAL)
{

//********************************************************************************************
	//	Libro de Aperturas
	juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);

	if (usarLibroAperturas && esActivoLibro && (nFueraLibro < 3) && (juego.totalMov<60))
	{
		char fen[100];
		memset(fen,'\0', 100);
		converTabler2FEN(fen);
		char FROM[10],TO[10], MOVALG_LIBRO[20];
		char FROM1[20],FROM2[20],TO1[20],TO2[20];
		int  nMovLibro;

		FIND_OPENING(fen,FROM,TO,FROM1,FROM2,TO1,TO2,&nMovLibro);

		if (!nMovLibro)
			nFueraLibro++;
		else {
			nFueraLibro = 0;
			sprintf(MOVALG_LIBRO, "%s%s",FROM,TO);

			MOVIMIENTO m = parse_mov(MOVALG_LIBRO);
			if (m != -1)
			{
				hacerMovimiento(m);

				if (juego.colorTurno) // Le toca al negro, se chequea que el rey blanco no haya quedado en jaque
				{
					esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
				} else {
					esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
				}

				desHacerMovimiento(m);
			
				if (!esJaque)
				{
					printf ("0 0 0 0 Book move\n");
					return m;
				}
			}
		}
	}

//********************************************************************************************
	//Rutina para comprobar si estamos en presencia de un unico movimiento
	//en caso de ser asi, no vale la pena buscar nada, solo hay que devolverlo y ya
//	juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
	for (i = juego.Buffer_MOV_INDEXCAPAS[0]; i < juego.Buffer_MOV_INDEXCAPAS[1]; i++)
	{
		hacerMovimiento(juego.Buffer_MOV[i]);

		// Le toca al negro, el lado blanco ha jugado, se chequea que el rey blanco no haya quedado en jaque
		if (juego.colorTurno) 
		{
			esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
		} else {
			esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
		}

		if (!esJaque)
		{
			contMov++;
			i_unicoMov = i;
		}

		desHacerMovimiento(juego.Buffer_MOV[i]);
	}

	if (contMov==1)
	{
		return juego.Buffer_MOV[i_unicoMov];
	} 

	totalMovRaiz 		= contMov;
	fraccionTotalMovRaiz 	= contMov/6; 

//********************************************************************************************
} else { // FIN busqueda normal

	juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);

}	
	profundidadTotal 	= juego.profundidadBusquedad;
	contadorDescendente 	= CONTADOR_DESCENDENTE_INTERVALO;
	tiempoVencido 		= FALSO;
	ladoJugando 	    	= juego.colorTurno;

	limpiarBufffer();
	prepararTiempo();
	iniciarTiempo();

	if (esConsola)
	{
		printf("capa      (ms)      nodos    qnodos    score     pv\n");
	}

	for (profundidadActual = 1; profundidadActual <= profundidadTotal; profundidadActual++)
	{

		limpiarAntesDeBusqueda();
		valoracionAnterior 	= valoracion; 

		if (movResul != 0)
		{
			ABORTABLE = VERDADERO;
		}

#ifdef COMPILAR_CON_EGBB
		profundidadEgbb = (profundidadActual*2) / 3;
#endif

		//BUSQUEDA
		valoracion = alfabetaNegado(0,profundidadActual,alfa, beta, VERDADERO);

//*****************************************************************************************************************
#define MARGEN  33
#define MARGEN2 66

		if (!tiempoVencido)
		{
			if (valoracion >= beta)
			{
				limpiarAntesDeBusqueda();
				valoracion = alfabetaNegado(0,profundidadActual,valoracion-1, beta+MARGEN2,  VERDADERO);
				if (valoracion >= beta)
				{		
					limpiarAntesDeBusqueda();
					valoracion = alfabetaNegado(0,profundidadActual,valoracion-1, INFINITO,  VERDADERO);
				}
	
			} else if (valoracion <= alfa) {

				limpiarAntesDeBusqueda();
				valoracion = alfabetaNegado(0,profundidadActual,alfa-MARGEN2, valoracion+1, VERDADERO);

				if (valoracion <= alfa)
				{
					limpiarAntesDeBusqueda();
					valoracion = alfabetaNegado(0,profundidadActual,-INFINITO, valoracion+1, VERDADERO);
				}
			} 

			alfa = valoracion - MARGEN;
			beta = valoracion + MARGEN;
		}
//*****************************************************************************************************************

		profundidadPrint = profundidadActual;

		tiempo = obt_msTiempo();

		if (esPost && (tipoDeBusqueda == TIPO_BUSQUEDA_NORMAL))
		{
			printf("%d %d %llu %d ",profundidadActual, valoracion, tiempo/1000,(contadorNodos+QcontadorNodos));
			for (i_pv=0;i_pv<juego.triangularLargo[0];++i_pv)
			{
				imprimirMovimiento(juego.triangularArray[0][i_pv]);
				printf(" ");
			}
			printf("\n");
		} else if (esPost && (tipoDeBusqueda == TIPO_BUSQUEDA_PONDER)) {

			printf("%d %d %llu %d ",profundidadActual, valoracion, tiempo/1000,(contadorNodos+QcontadorNodos));
			printf("(");imprimirMovimiento(mponder);printf(") ");
			for (i_pv=0;i_pv<juego.triangularLargo[0];++i_pv)
			{
				imprimirMovimiento(juego.triangularArray[0][i_pv]);
				printf(" ");
			}
			printf("\n");
		}

		if (esConsola)
		{
			printf("%3d %10lld  %9d  %9d  %6d     ",profundidadActual, tiempo,contadorNodos,QcontadorNodos, valoracion);
			for (i_pv=0;i_pv<juego.triangularLargo[0];++i_pv)
			{
				imprimirMovimiento(juego.triangularArray[0][i_pv]);
				printf("  ");
			}
			printf("\n");
		}

		//Aun queda tiempo en la busqueda, podemos en teoria seguir al proximo nivel de profundidad
		if (!tiempoVencido) 
		{
			if (juego.triangularArray[0][0])
			{
				movResul = juego.triangularArray[0][0];
			}

		} else if (tiempoVencido && movResul != juego.triangularArray[0][0]){ 

			//El tiempo se vencio durante la busquedad, hubo un corte en la profundidad
			//Verificar que tan confiable es el movimiento encontrado

			if (nodoRaiz > fraccionTotalMovRaiz)
			{
				if (juego.triangularArray[0][0])
				{
					movResul = juego.triangularArray[0][0];
				}
			}

			detenerTiempo();
			profundidadActual = profundidadTotal;

		} else if (tiempoVencido && movResul == juego.triangularArray[0][0]) {

			detenerTiempo();
			profundidadActual = profundidadTotal;
		}

		if (!ES_ETAPA_FINAL && (tiempo > (juego.maxTiempo*0.8)) && !tiempoVencido)
		{
			detenerTiempo();
			profundidadActual = profundidadTotal;
		}

		if ((valoracion > (VALOR_JAQUE_MATE-profundidadActual)) || (valoracion < -(VALOR_JAQUE_MATE-profundidadActual))) 
			profundidadActual = profundidadTotal;
	}

	//No encontro ningun movimiento bueno para ganar, (Termina en empate o derrota, devolver cualquier movimiento estupido)
	if (!movResul) 
	{
		juego.Buffer_MOV_INDEXCAPAS[1] = generarTodosMov(0);
		ponderarMovimientos(0, &movGafo, 1);
		 
		for (i = juego.Buffer_MOV_INDEXCAPAS[0]; i < juego.Buffer_MOV_INDEXCAPAS[1]; i++)
		{
			seleccionarMovimiento(0, i);
			hacerMovimiento(juego.Buffer_MOV[i]);

			// Le toca al negro, el lado blanco ha jugado, se chequea que el rey blanco no haya quedado en jaque
			if (juego.colorTurno) 
			{
				esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
			} else {
				esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
			}

			desHacerMovimiento(juego.Buffer_MOV[i]);

			if (!esJaque)
			{
				movResul = juego.Buffer_MOV[i];
				break;
			}
		}
	} 

	return (movResul);
}

int alfabetaNegado(int capa, int profundidad, int alfa, int beta, BOOLEANO hacerNULL)
{
	BOOLEANO nodoPV		= ((beta - alfa) > 1);
	BOOLEANO es;
	BOOLEANO estaEnJaque;
	BOOLEANO daJaque;
	BOOLEANO esFinal;
	BOOLEANO sePuede	= FALSO;
	int reduccionLMR	= 0;
	COLOR c;
	int i		 	= 0;
	int j		 	= 0;
	int V		 	= alfa;
	int contMov	 	= 0;
	int contPVMov	 	= 0;
	BOOLEANO esJaque 	= FALSO;
	MOVIMIENTO h_mov 	= 0; 
	int h_V 	 	= 0;
	int h_banderas 	 	= BANDERA_HASH_VACIO;
	int i_alfa		= alfa;
	int ext			= 0;
	int tmp1		= 0;
	int tmp2		= 0;
	int vv			= 0;
	int vr;
	BOOLEANO esAmenazaMate  = FALSO;
	BOOLEANO esPodable	= FALSO;
	BOOLEANO validarMasExt;
	BOOLEANO esMovPeon;
	BOOLEANO esCaptura;
	int posDestinoMovPeon; 

	juego.triangularLargo[capa] = capa;

//*
	// Poda Distancia de Mate
	if (capa) 
	{
		register int mv = VALOR_JAQUE_MATE - capa;

		if (mv < beta)
		{
			beta = mv;
			if (alfa >= mv) return mv;
		}

		mv = -VALOR_JAQUE_MATE + capa;
		if (mv > alfa)
		{
			alfa = mv;
			if (beta <= mv) return mv;
		}
	}
//*/

/*************************************************************************
 *
 *  Verificar Empate (Insuficiencia de Material o Repeticion de Tablero)
 *
 *************************************************************************/
	if (capa) 
	{
		//Chequear Repeticion del tablero
		ES_REPETICION_TABLERO(es);
		if (es)
		{
			return PUNTAJE_EMPATE;
		}
	}
/*************************************************************************
 *
 *  FIN Verificar Empate (Insuficiencia de Material o Repeticion de Tablero)
 *
 *************************************************************************/

	//Poner limite a las capas de busquedad
	if (capa >= MAX_CAPAS_BUSQUEDAD) return busquedadTranquilidad(capa, alfa, beta);	


/*************************************************************************
 *
 *  Hemos Llegado a las Hojas del Arbol
 *
 *************************************************************************/

	estaEnJaque = ES_ESTADO_JUEGO_JAQUE;
	esFinal	    = ES_ETAPA_FINAL;

	if (profundidad < 1)
	{
		return busquedadTranquilidad(capa, alfa, beta);
	}

/*************************************************************************
 *
 *  TABLA DE TRANSPOSICION
 *
 *************************************************************************/
	if (esUsoTablaHash)
	{
		if (capa)
		{
			h_V = verificarTablaHash(alfa, beta,capa, profundidad, &h_banderas, &h_mov);
			if (h_banderas != BANDERA_HASH_VACIO) 
			{
				switch(h_banderas)
				{
					case BANDERA_HASH_EXACTO:	if (!nodoPV) 		return h_V;		
					case BANDERA_HASH_ARRIBA:  	if (h_V <= alfa)	return alfa;
					case BANDERA_HASH_ABAJO: 	if (h_V >= beta)	return beta;
					case BANDERA_HASH_EVITAR_NULL:  hacerNULL = FALSO; break;
				}
			}
		}
	}

/*************************************************************************
 *  BASE DE DATOS DE FINALES NALIMOV
 *************************************************************************/
//*
#ifdef COMPILAR_CON_EGBB
	if (usar_egbb && egbb_is_loaded && capa && (capa <= profundidadEgbb))
	{

		if (ES_MOV_CAPTURA(movAnterior) || ES_MOV_PROMOCION(movAnterior) || (capa < 3))
		{ 
			int totalPiezas = cuentaBit(juego.tablero[BLANCO][PEON])    + cuentaBit(juego.tablero[NEGRO][PEON])    + 
			    	  	  cuentaBit(juego.tablero[BLANCO][DAMA])    + cuentaBit(juego.tablero[NEGRO][DAMA])    + 
				  	  cuentaBit(juego.tablero[BLANCO][CABALLO]) + cuentaBit(juego.tablero[NEGRO][CABALLO]) + 
				  	  cuentaBit(juego.tablero[BLANCO][TORRE])   + cuentaBit(juego.tablero[NEGRO][TORRE])   + 
				  	  cuentaBit(juego.tablero[BLANCO][ALFIL])   + cuentaBit(juego.tablero[NEGRO][ALFIL])   + 2;

			if (totalPiezas <= nLimitePiezasBitBases)
			{
				int V_egbb=0;
				if (probe_bitbases(&V_egbb)) 
				{
					alfa = V_egbb;
					if (alfa==0) 
					{
						if (juego.material_total > 0)
						{
							alfa += ((juego.colorTurno) ? -1:1);					

						} else if (juego.material_total < 0) {

							alfa += ((juego.colorTurno) ? 1:-1);					
						}

					} else 	if (alfa < 0) {

						alfa += (capa-2);

					} else if (alfa > 0) {

						alfa -= (capa+2);
					}

					agregarMovTablaHash(256, capa, alfa, BANDERA_HASH_EXACTO, 0);
					return alfa;
				}
			}
		}
	}
#endif
//*/
//*******************************************************************************************************************************
	if (capa && !estaEnJaque && !h_mov && nodoPV && profundidad > 3 && hacerNULL)
	{
		V   = alfabetaNegado(capa, profundidad-2,alfa,beta, VERDADERO);
		h_V = verificarTablaHash(alfa, beta,capa, profundidad, &h_banderas, &h_mov);
		if (h_banderas != BANDERA_HASH_VACIO) 
		{
			switch(h_banderas)
			{
				case BANDERA_HASH_EVITAR_NULL:  hacerNULL = FALSO; break;
			}
		}
	}

/*************************************************************************
 *
 *  PODA POR MOVIMIENTO NULO
 *
 *************************************************************************/
	if (capa && !estaEnJaque && !nodoPV && beta < VALOR_JAQUE_MATE-64)
	{

		vv = evaluacionTablero(-VALOR_JAQUE_MATE, VALOR_JAQUE_MATE);
		if (profundidad < 4 && hacerNULL)
		{
			vr = vv - 150;
			if (vr >= beta)
			{
				return vr;
			}

			vr = vv + 125;
			if (vr < beta)
			{
				register int nvalor;
				if (profundidad==1)
				{
					nvalor = busquedadTranquilidad(capa, alfa, beta);
					return MAXIMO(nvalor, vr);
				}
			}
		}

		if (hacerNULL &&  
		    ((juego.colorTurno  && (juego.material_lado_negro  > LIMITE_MOV_NULL)) || 
		    (!juego.colorTurno && (juego.material_lado_blanco > LIMITE_MOV_NULL))))
		{

			if (juego.colorTurno) // Le toca al negro, se chequea que el rey negro este en jaque
			{
				esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
			} else {
				esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
			}

			if (!esJaque)
			{
				contadorNodos++;

				COLOR 	 RcolorTurno	= juego.colorTurno;
				BOOLEANO ROOB		= juego.OOB;
				BOOLEANO ROOOB		= juego.OOOB;
				BOOLEANO ROON		= juego.OON;
				BOOLEANO ROOON		= juego.OOON;
				int	 RENROQUEB	= juego.ENROQUEB;
				int	 RENROQUEN	= juego.ENROQUEN;
				uint8	 RposPeonPaso	= juego.posPeonPaso;
				uint8	 RreglaCincuentaMov= juego.reglaCincuentaMov;
				uint32	 RtotalMov	= juego.totalMov;
				uint64	 RllaveHash	= juego.llaveHash;

				juego.colorTurno 	= !juego.colorTurno;
				juego.llaveHash        ^= arrayHash.lado;

				juego.reglaCincuentaMov = 0;

				V = -alfabetaNegado(capa,profundidad-REDUCCION_MOV_NULL-1,-beta,-beta+1, FALSO);	

				juego.colorTurno 	= RcolorTurno;
				juego.OOB 		= ROOB;
				juego.OOOB 		= ROOOB;
				juego.OON 		= ROON;
				juego.OOON 		= ROOON;
				juego.ENROQUEB 		= RENROQUEB;
				juego.ENROQUEN 		= RENROQUEN;
				juego.posPeonPaso 	= RposPeonPaso;
				juego.reglaCincuentaMov = RreglaCincuentaMov;
				juego.totalMov		= RtotalMov;
				juego.llaveHash 	= RllaveHash;

				if (tiempoVencido) return 0;		
				if (V >= beta)
				{
					if (esUsoTablaHash)
					{
						agregarMovTablaHash(profundidad, capa, V, BANDERA_HASH_ABAJO, juego.Buffer_MOV[i]);
					}

					if (V >= (VALOR_JAQUE_MATE-64)) V = beta;

					return V;//beta;
				} else if (V <= -VALOR_JAQUE_MATE+capa+2){
					esAmenazaMate = VERDADERO;
				}
			}
		}

		/*cormprobamos si es posible hacer futility prunning*/
		if (profundidad < 4 && alfa > (-VALOR_JAQUE_MATE+64))
		{
			if (vv + margen_poda[profundidad] < beta)
			{
				esPodable = VERDADERO;
			}
		}
	}
/*************************************************************************
 *
 *  FIN PODA POR MOVIMIENTO NULO
 *
 *************************************************************************/


/*************************************************************************
 *
 *  BUSQUEDA ALFA-BETA: Variación de búsqueda principal
 *
 *************************************************************************/
	contMov			= 0;
	contPVMov		= 0;

	juego.Buffer_MOV_INDEXCAPAS[capa+1] = generarTodosMov(juego.Buffer_MOV_INDEXCAPAS[capa]);
	ponderarMovimientos(capa, &h_mov, profundidad);
	 
	for (i = juego.Buffer_MOV_INDEXCAPAS[capa]; i < juego.Buffer_MOV_INDEXCAPAS[capa+1]; i++)
	{
		seleccionarMovimiento(capa, i);
		hacerMovimiento(juego.Buffer_MOV[i]);

		// Le toca al negro, el lado blanco ha jugado, se chequea que el rey blanco no haya quedado en jaque
		if (juego.colorTurno) 
		{
			esJaque = esAtacadoPor(juego.tablero[BLANCO][REY], NEGRO);
		} else {
			esJaque = esAtacadoPor(juego.tablero[NEGRO][REY], BLANCO);
		}

		if (!esJaque)
		{
			contadorNodos++;

			if (--contadorDescendente < 1) 
			{
				verReloj_y_Teclado();
			}

			//Verificar Extenciones
			ext 		  = 0;
			c 		  = !juego.colorTurno;
			validarMasExt     = FALSO;
			esCaptura         = ES_MOV_CAPTURA(juego.Buffer_MOV[i]);
			daJaque 	  = ES_ESTADO_JUEGO_JAQUE;
			posDestinoMovPeon = OBT_MOV_DESTINO(juego.Buffer_MOV[i]); 
			esMovPeon 	  = ES_MOV_PEON(juego.Buffer_MOV[i]);

			if (daJaque)
			{
				if (esCaptura)
				{
					if (EET(juego.Buffer_MOV[i]) < 1)
					{
						ext = 1;
					}
				} else {
					if (EETPOS((uint8)posDestinoMovPeon, juego.colorTurno) < 0) 
					{
						ext = 1;
					}
				}

			} else if (esAmenazaMate) {

				ext = 1;

			} else if (juego.indiceHJuego > 2 && esCaptura && ES_MOV_CAPTURA(movAnterior) && 
				 VALORPIEZA_FASE[OBT_MOV_CAPTURA(movAnterior)] > 1 &&
				(VALORPIEZA_FASE[OBT_MOV_CAPTURA(movAnterior)]==
						VALORPIEZA_FASE[OBT_MOV_CAPTURA(juego.Buffer_MOV[i])])){ 

				ext = 1;

			} else if (esMovPeon && RANKS[posDestinoMovPeon]==septimoRank[c]) {

				ext = 1;

			} else if (esFinal && esMovPeon && RANKS[posDestinoMovPeon]==sextoRank[c]) {

				ext = 1;

			} else if (esFinal && esCaptura) {

				register int pc = OBT_MOV_CAPTURA(juego.Buffer_MOV[i]);
				if (pc != PEON_BLANCO && pc != PEON_NEGRO)
				{
					if (!juego.tablero[BLANCO][DAMA]  && !juego.tablero[BLANCO][TORRE] &&
					    !juego.tablero[BLANCO][ALFIL] && !juego.tablero[BLANCO][CABALLO] &&
					    !juego.tablero[NEGRO][DAMA]   && !juego.tablero[NEGRO][TORRE] &&
					    !juego.tablero[NEGRO][ALFIL]  && !juego.tablero[NEGRO][CABALLO])
					{
						ext = 1;
					}
				}

			}

//			if (ext && nodoPV) ext++;

			if (contMov) 
			{
				sePuede = (!ext && !esCaptura && !daJaque && !ES_MOV_PROMOCION(juego.Buffer_MOV[i]) && 
					    h_mov != juego.Buffer_MOV[i] && !ES_MATADOR_MOV(capa,juego.Buffer_MOV[i]));

				if (esPodable && sePuede && contMov > 1)
				{
					desHacerMovimiento(juego.Buffer_MOV[i]);
					continue;
				}

				reduccionLMR = 0;
				if (sePuede && !siguePV && (contMov > 3 && profundidad > 2) && alfa > (-VALOR_JAQUE_MATE/2))
				{
					if (!esMovPeon || 
						(PASADO[!juego.colorTurno][posDestinoMovPeon] & 
						juego.tablero[juego.colorTurno][PEON]))
					{

						tmp1     = ((juego.colorTurno) ?
							juego.heuristicaBlanca[OBT_MOV_ORIGEN(juego.Buffer_MOV[i])]
									      [posDestinoMovPeon]:
							juego.heuristicaNegra[OBT_MOV_ORIGEN(juego.Buffer_MOV[i])]
									     [posDestinoMovPeon]);
						tmp2     = maxHistorial[!juego.colorTurno]/100;

						if (tmp1 < (tmp2*66))
						{
							if (!esFinal && contMov > 8  && profundidad > 3 && tmp1 < (tmp2*33)) {

								reduccionLMR = 2;

							} else {

								reduccionLMR = 1;
							}
						}
					}
				}
				V = -alfabetaNegado(capa + 1, profundidad-1+ext-reduccionLMR,-alfa-1,-alfa, VERDADERO);

				if ((alfa < V) && (V < beta))
				{
					V = -alfabetaNegado(capa + 1, profundidad-1+ext,-beta,-alfa, VERDADERO);
				}

//*/
/*************************************************************************
 *  			FIN PODA POR MOVIMIENTO TARDIO
 *************************************************************************/

			} else {
				V = -alfabetaNegado(capa + 1, profundidad-1+ext,-beta,-alfa, VERDADERO);
			}

			desHacerMovimiento(juego.Buffer_MOV[i]);
		
			if (tiempoVencido) return 0;		

			//Cuando hay corte de tiempo verificar que no se vaya a dar una respuesta superficial
			//Se lleva un conteo de cuantos nodos raiz se han procesado
			if (capa==0) nodoRaiz++;

			contMov++;

			if (V > alfa)
			{
				//Historial
				if (juego.colorTurno)
				{ 
					juego.heuristicaNegra[OBT_MOV_ORIGEN(juego.Buffer_MOV[i])]
							     [posDestinoMovPeon] += profundidad*profundidad;

					if (maxHistorial[NEGRO] < juego.heuristicaNegra[OBT_MOV_ORIGEN(juego.Buffer_MOV[i])]
							     			       [posDestinoMovPeon])
					{
						maxHistorial[NEGRO] = juego.heuristicaNegra[OBT_MOV_ORIGEN(juego.Buffer_MOV[i])]
							     			       [posDestinoMovPeon];
					}

				} else { 
					juego.heuristicaBlanca[OBT_MOV_ORIGEN(juego.Buffer_MOV[i])]
							     [posDestinoMovPeon] += profundidad*profundidad;

					if (maxHistorial[BLANCO] < juego.heuristicaBlanca[OBT_MOV_ORIGEN(juego.Buffer_MOV[i])]
							     			       [posDestinoMovPeon])
					{
						maxHistorial[BLANCO] = juego.heuristicaBlanca[OBT_MOV_ORIGEN(juego.Buffer_MOV[i])]
							     			       [posDestinoMovPeon];
					}
				}

				if (V >= beta)
				{
					//Movimientos Matadores
					actualizarMovMatadores(juego.Buffer_MOV[i], V, capa);
					//Actualizar Tabla Hash de Movimientos
					if (esUsoTablaHash)
					{
						agregarMovTablaHash(profundidad, capa, V, BANDERA_HASH_ABAJO, juego.Buffer_MOV[i]);
					}
					return V;//beta;
				}

				alfa = V;
				contPVMov++;

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

/*************************************************************************
 *
 *  FIN: BUSQUEDA ALFA-BETA: Variación de búsqueda principal
 *
 *************************************************************************/

	if (contPVMov)
	{
		if (juego.colorTurno) 
		{
			juego.heuristicaNegra[OBT_MOV_ORIGEN(juego.triangularArray[capa][capa])]
					     [OBT_MOV_DESTINO(juego.triangularArray[capa][capa])]+= profundidad*profundidad;

			if (maxHistorial[NEGRO] < juego.heuristicaNegra[OBT_MOV_ORIGEN(juego.triangularArray[capa][capa])]
					     			       [OBT_MOV_DESTINO(juego.triangularArray[capa][capa])])
			{
				maxHistorial[NEGRO] = juego.heuristicaNegra[OBT_MOV_ORIGEN(juego.triangularArray[capa][capa])]
					     			       [OBT_MOV_DESTINO(juego.triangularArray[capa][capa])];
			}


		} else {
			juego.heuristicaBlanca[OBT_MOV_ORIGEN(juego.triangularArray[capa][capa])]
					     [OBT_MOV_DESTINO(juego.triangularArray[capa][capa])]+= profundidad*profundidad;

			if (maxHistorial[BLANCO] < juego.heuristicaBlanca[OBT_MOV_ORIGEN(juego.triangularArray[capa][capa])]
					     			       [OBT_MOV_DESTINO(juego.triangularArray[capa][capa])])
			{
				maxHistorial[BLANCO] = juego.heuristicaBlanca[OBT_MOV_ORIGEN(juego.triangularArray[capa][capa])]
					     			       [OBT_MOV_DESTINO(juego.triangularArray[capa][capa])];
			}
		}
	}


/*************************************************************************
 *
 *  En caso de no haber movimientos, validar mate o ahogado
 *
 *************************************************************************/

	if (!contMov)
	{
		if (esAtacadoPor(juego.tablero[juego.colorTurno][REY], !juego.colorTurno)) // Esta jaque mate
		{
			return -VALOR_JAQUE_MATE+capa-2;

		} else { // El jugador esta ahogado

			return VALOR_AHOGADO;	
		}
	}

/*************************************************************************
 *
 *  Validacion Regla 50 movimientos
 *
 *************************************************************************/

	if (juego.reglaCincuentaMov > 99)
	{ 
		return PUNTAJE_EMPATE;
	}

/*************************************************************************
 *
 *  Actualizar Tabla Hash de Movimientos
 *
 *************************************************************************/
	if (esUsoTablaHash)
	{
		if (alfa > i_alfa)
		{
			agregarMovTablaHash(profundidad, capa, alfa, BANDERA_HASH_EXACTO, juego.triangularArray[capa][capa]);
		} else {
			agregarMovTablaHash(profundidad, capa, alfa, BANDERA_HASH_ARRIBA, juego.triangularArray[capa][capa]);
		}
	}

	return alfa;
}

#endif
