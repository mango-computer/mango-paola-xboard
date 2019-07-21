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

#ifndef MACROS_H
#define MACROS_H

#define 	VERSION_MANGO_AJEDREZ 		"Mango Paola Ajedrez 1.1, (c) 2013. Creado por: Jose Andres Morales Linares. comprasmangocomputer@gmail.com"

#define NOMBRE_MANGO_PAOLA			"MangoPaolaAjedrez"
#define VERSION_MANGO_PAOLA			"1.1"


//#define		DEBUG_MANGO_AJEDREZ
//#define		COMPILAR_CON_EGBB

//************ LIMITES MAXIMOS
#define		MAX_CAPAS			32
#define		MAX_CAPAS_QBUSQUEDAD		30
#define		MAX_CAPAS_BUSQUEDAD		24
#define		MAX_BUFF_MOV			8192	//4096

#define		VERDADERO			1
#define		FALSO				0
#define		BLANCO				0
#define		NEGRO				1
#define		SIN_COLOR			2

#define		MATADOR_MOV			0
#define		MATADOR_VALOR			1
#define		INFINITO			100000
#define		VALOR_JAQUE_MATE		99000
#define		VALOR_ALTO			90000
#define		VALOR_BAJO			-90000
#define		VALOR_AHOGADO			0
#define		INCREMENTO_PROMO_DELTA_CORTE	875

#define		LARGO_HASH_EVAL			1048575
//#define		LARGO_TABLA_HASH	4194303
/*
	Llave Hash de 26 bits = posiciones 67.108.863 * (24 Bytes del Registro Tabla Hash) = 1536 MB
	Llave Hash de 25 bits = posiciones 33.554.431 * (24 Bytes del Registro Tabla Hash) =  768 MB
	Llave Hash de 24 bits = posiciones 16.777.215 * (24 Bytes del Registro Tabla Hash) =  384 MB
	Llave Hash de 23 bits = posiciones  8.388.607 * (24 Bytes del Registro Tabla Hash) =  192 MB
	Llave Hash de 22 bits = posiciones  4.194.303 * (24 Bytes del Registro Tabla Hash) =   96 MB
	Llave Hash de 21 bits = posiciones  2.097.151 * (24 Bytes del Registro Tabla Hash) =   48 MB
	Llave Hash de 20 bits = posiciones  1.048.575 * (24 Bytes del Registro Tabla Hash) =   24 MB
	Llave Hash de 19 bits = posiciones    262.143 * (24 Bytes del Registro Tabla Hash) =    6 MB

*/
#define		LARGO_TABLA_HASH_LLAVE_18BIT	262143
#define		LARGO_TABLA_HASH_LLAVE_19BIT	524287
#define		LARGO_TABLA_HASH_LLAVE_20BIT	1048575
#define		LARGO_TABLA_HASH_LLAVE_21BIT	2097151
#define		LARGO_TABLA_HASH_LLAVE_22BIT	4194303
#define		LARGO_TABLA_HASH_LLAVE_23BIT	8388607
#define		LARGO_TABLA_HASH_LLAVE_24BIT	16777215
#define		LARGO_TABLA_HASH_LLAVE_25BIT	33554431
#define		LARGO_TABLA_HASH_LLAVE_26BIT	67108863

#define		BANDERA_HASH_ARRIBA		1
#define		BANDERA_HASH_ABAJO		2
#define		BANDERA_HASH_EXACTO		4
#define		BANDERA_HASH_EVITAR_NULL	8
#define		BANDERA_HASH_VACIO		16

#define		CONTADOR_DESCENDENTE_INTERVALO	10000
#define		VALOR_CAMBIOS_MALOS		-100
#define		TIEMPO_VOYMAL			80

#define		MAX_MOV_LIBRO			8
#define		LIMITE_MOV_NULL			324		// Valor caballo menos 1
#define		REDUCCION_MOV_NULL		2

#define		TIPO_BUSQUEDA_NORMAL		1
#define		TIPO_BUSQUEDA_PONDER		2

#define		OO				0
#define		OOO				1
#define 	PEON				0 	
#define 	ALFIL				1	
#define 	CABALLO				2 	
#define 	TORRE				3 	
#define 	DAMA				4	
#define 	REY				5	

#define		LARGO_BUFER_COMANDOS		256

#define		MEDIO_JUEGO			0
#define		FINAL_JUEGO			1

#define		SIN_ENROQUE			0
#define		ENROQUE_OO			1
#define		ENROQUE_OOO			2

#define		SIN_POS_VALIDA			100
#define		VACIO				0
#define		PEON_BLANCO			1
#define		REY_BLANCO			2
#define		CABALLO_BLANCO			3
#define		ALFIL_BLANCO			5
#define		TORRE_BLANCO			6
#define		DAMA_BLANCO			7

#define		PEON_NEGRO			9
#define		REY_NEGRO			10
#define		CABALLO_NEGRO			11
#define		ALFIL_NEGRO			13
#define		TORRE_NEGRO			14
#define		DAMA_NEGRO			15

#define		EET_VALOR_PEON			1	
#define		EET_VALOR_CABALLO		3	
#define		EET_VALOR_ALFIL			3	
#define		EET_VALOR_TORRE			5	
#define		EET_VALOR_DAMA			9	
#define		EET_VALOR_REY			98	

#define		VALOR_PEON			100	
#define		VALOR_CABALLO			325	
#define		VALOR_ALFIL			340	
#define		VALOR_TORRE			500	
#define		VALOR_DAMA			975	
#define		VALOR_REY			9750
#define		DOBLE_VALOR_CABALLO_ALFIL	650	
#define		VALOR_CABALLO_MENOS_5		320	
#define		VALOR_TRES_PEONES		300	
#define		VALOR_DOS_PEONES		200	
#define		VALOR_MEDIO_PEON		 50	
#define		VALOR_MINIMA_DIFERENCIA_PIEZAS	 16	
#define		MARGEN_MAL_PUNTAJE		300
#define		INCREMENTO_TIEMPO_MAL_PUNTAJE	5

#define		LIMITE_INFERIOR_FINAL_JUEGO	1475 //valor de la dama + torre 

#define		PUNTAJE_EMPATE			0

#define		PC_NIVEL_FACIL			0
#define		PC_NIVEL_MEDIO			1
#define		PC_NIVEL_FUERTE			2
#define		PC_NIVEL_PERSONALIZADO		3

#define		COD_MOV_TRANQUILO		0
#define		COD_MOV_PEON_SALIDA_DOBLE	1
#define		COD_MOV_ENROQUE_OO		2
#define		COD_MOV_ENROQUE_OOO		3
#define		COD_MOV_CAPTURA			4
#define		COD_MOV_CAPTURA_PEON_PASO	5
#define		COD_MOV_PROMOCION_CABALLO	8
#define		COD_MOV_PROMOCION_ALFIL		9
#define		COD_MOV_PROMOCION_TORRE		10
#define		COD_MOV_PROMOCION_DAMA		11
#define		COD_MOV_PROMOCAPT_CABALLO	12
#define		COD_MOV_PROMOCAPT_ALFIL		13
#define		COD_MOV_PROMOCAPT_TORRE		14
#define		COD_MOV_PROMOCAPT_DAMA		15
//************************************************************

//**************** PEON **************************************
//************** Bonos  peon *********************************
#define		BONO_PEON_PASADO			25
#define		BONO_ATAQUE_PEON_DEBIL			2
#define		BONO_ATAQUE_PEON_DEBIL_X_REY		12
#define		BONO_PEON_DEFENDIDO_X_PEON		2
#define		BONO_PEON_PROXIMO_REY_ENEMIGO		40
#define		BONO_PEON_PASADO_CC			50
#define		BONO_PEON_PASADO_CC_SIN_REY		550
#define		BONO_PEON_ESCUDO_FUERTE			9
#define		BONO_PEON_ESCUDO_DEBIL			4
#define		BONO_PEON_PASADO_LIBRE_FRENTE		5 
#define		BONO_PEON_PASADO_DEFENDIDO		50//15
#define		BONO_PEON_PASADO_NO_AISLADO		20

//************** Descuentos  peon *********************************
#define		DESCUENTO_PEON_BLOQUEADO		20
#define		DESCUENTO_PEON_DOBLE			10
#define		DESCUENTO_PEON_AISLADO			10
#define		DESCUENTO_PEON_ATRASADO			10 
#define		DESCUENTO_PEON_BLOQUEADO_X_PEON		10
#define		DESCUENTO_PEON_BAJO_ATAQUE 		 2
#define		DESCUENTO_PEON_BASE_BAJO_ATAQUE		18

//**************** CABALLO **************************************
//************** Bonos  caballo *********************************

//************** Descuento  caballo *********************************
#define		DESCUENTO_CABALLO_ATRAPADO      	60 
#define		DESCUENTO_CABALLO_CLAVADO      	        30 
#define		DESCUENTO_PAREJA_CABALLOS      	        8 


//**************** ALFIL **************************************
//************** Bonos  alfil *********************************
#define		BONO_PAR_ALFIL				18
#define		BONO_FIANCHETTO				8

//************** Descuentos  alfil *********************************
//#define		DESCUENTO_ALFIL_PARALIZADO		100//200//20//200
#define		DESCUENTO_ALFIL_ATRAPADO      		174 
#define		DESCUENTO_ALFIL_CLAVADO      	        30 

//**************** TORRE **************************************
//************** Bonos  torre *********************************
#define		BONO_TORRE_DETRAS_PEON_PASADO		50//30
#define		BONO_TORRE_COLUMNA_MEDIA		6
#define		BONO_TORRE_COLUMNA_SEMIABIERTA		10
#define		BONO_TORRE_COLUMNA_EN_CORREDOR		15
#define		BONO_TORRE_LIBERADA			16
#define		BONO_TORRE_7_RANK			30
//************** Descuento  torre *********************************
#define		DESCUENTO_TORRE_ATRAPADO_PSEUDO_ENROQUE	60
#define		DESCUENTO_TORRE_ATRAPADA		60
 
#define		DESCUENTO_TORRE_CLAVADO      	        50 
#define		DESCUENTO_PAREJA_TORRES      	        16 


//**************** DAMA **************************************
//************** Bonos  Dama *********************************
#define		BONO_DAMA_7_RANK			10
#define		BONO_DAMA_PROXIMA_REY_ENEMIGO		12
//************** Descuento  dama *********************************
#define		DESCUENTO_SALIDA_DAMA_APERTURA		10
#define		DESCUENTO_DAMA_NO_PRESENTE		25
#define		DESCUENTO_DAMA_CLAVADO      	        90 

//**************** REY **************************************
//************** Bono  rey *********************************

//************** Descuento  rey *********************************
#define		DESCUENTO_G_ABIERTA			30
#define		DESCUENTO_REY_FILE_ABIERTA_AMIGOS	18
#define		DESCUENTO_REY_FILE_ABIERTA_ENEMIGOS	12
#define		DESCUENTO_REY_EN_PELIGRO_POR_FILE_H	300 	
#define		DESCUENTO_REY_MOVIDO			20
#define		DESCUENTO_REY_SIN_PEONES_PROXIMOS	40
#define		BONO_MOVILIDAD_REY_FINAL		6
#define		REY_TROPISMO				10


//**************** GENERAL **************************************
//************** Bono   *********************************
#define		BONO_MOVILIDAD_PEON		 	 2
#define		BONO_MOVILIDAD_AMENAZADA		 4
#define		BONO_MOVILIDAD				 6
#define		BONO_MOVILIDAD_SEGURA_INTERIOR		 4 
#define		BONO_MOVILIDAD_SEGURA_EXTERIOR		 2
#define		BONO_ENRROQUE				40
#define		BONO_DEFIENDE_PEON			10
#define		BONO_PIEZAS_COLGADAS			20
#define		COMERCIO_PEONES				 8
#define		COMERCIO_PIEZAS				 4
#define		PEON_PUEDE_CORONAR			525

//************** Descuento  *********************************
#define		DESCUENTO_X_NO_ENRROCAR			20
#define		DESCUENTO_PERDIDA_ENROQUE		20

#define		DESCUENTO_RUPTURA			16
#define		DESCUENTO_DESARROLLO_LENTO	 	5 //25
#define		DESCUENTO_PEON_DEBIL_M_PA 		4 
#define		DESCUENTO_PEON_DEBIL_M	 		8 
#define		DESCUENTO_PEON_DEBIL_F	 		18 



/******************	MACROS MANIPULACION FLAG DEL MOVIMIENTO	******************/

#define 	EST_MOV_ORIGEN(bandera,a)    	bandera &= 0xFFFFFFC0; bandera |= ( a & 0x0000003F) 
#define 	EST_MOV_DESTINO(bandera,a)   	bandera &= 0xFFFFF03F; bandera |= ((a & 0x0000003F) << 6) 
#define 	EST_MOV_PIEZA(bandera,a)     	bandera &= 0xFFFF0FFF; bandera |= ((a & 0x0000000F) << 12) 
#define 	EST_MOV_CAPTURA(bandera,a)   	bandera &= 0xFFF0FFFF; bandera |= ((a & 0x0000000F) << 16) 
#define 	EST_MOV_PROMOCION(bandera,a) 	bandera &= 0xFF0FFFFF; bandera |= ((a & 0x0000000F) << 20) 
#define 	EST_MOV_CODIGO(bandera,a)    	bandera &= 0xF0FFFFFF; bandera |= ((a & 0x0000000F) << 24) 

#define 	OBT_MOV_ORIGEN(bandera)         ((bandera)       & 0x0000003F)
#define 	OBT_MOV_DESTINO(bandera)        ((bandera >> 6)  & 0x0000003F)
#define 	OBT_MOV_PIEZA(bandera)          ((bandera >> 12) & 0x0000000F)
#define 	OBT_MOV_CAPTURA(bandera)        ((bandera >> 16) & 0x0000000F)
#define 	OBT_MOV_PROMOCION(bandera)      ((bandera >> 20) & 0x0000000F)
#define 	OBT_MOV_CODIGO(bandera)         ((bandera >> 24) & 0x0000000F)

#define		ES_MOV_BLANCO(bandera)		((~bandera & 0x00008000) == 0x00008000)
#define		ES_MOV_NEGRO(bandera)		(( bandera & 0x00008000) == 0x00008000)
#define		ES_MOV_CAPTURA(bandera)		(( bandera & 0x000F0000) != 0x00000000)
#define		ES_MOV_CAPTURA_REY(bandera)	(( bandera & 0x00070000) == 0x00020000)
#define		ES_MOV_TORRE(bandera)		(( bandera & 0x00007000) == 0x00006000)
#define		ES_MOV_CAPTURA_TORRE(bandera)	(( bandera & 0x00070000) == 0x00060000)
#define		ES_MOV_REY(bandera)		(( bandera & 0x00007000) == 0x00002000)
#define		ES_MOV_PEON(bandera)		(( bandera & 0x00007000) == 0x00001000)
#define		ES_MOV_PEON_DOBLE(bandera)	((( bandera & 0x00007000) == 0x00001000)   &&  \
						((((bandera & 0x00000038) == 0x00000008)  &&   \
						((( bandera & 0x00000e00) == 0x00000600))) ||  \
						((( bandera & 0x00000038) == 0x00000030)   &&  \
						((( bandera & 0x00000e00) == 0x00000800)))))

#define		ES_MOV_CAPT_PEON_PASO(bandera)  (OBT_MOV_CODIGO(bandera) ==  COD_MOV_CAPTURA_PEON_PASO)				
#define		ES_MOV_PROMOCION(bandera)	(( bandera & 0x00700000) >  0x00200000)
#define		ES_MOV_ENROQUE(bandera)		(( bandera & 0xfe000000)  == 0x2000000)
#define		ES_MOV_OO(bandera)		(( bandera & 0xff000000)  == 0x2000000)
#define		ES_MOV_OOO(bandera)		(( bandera & 0xff000000)  == 0x3000000)


/******************	FIN MACROS MANIPULACION FLAG DEL MOVIMIENTO	******************/

#define 	Fila(x)       ((x)>>3)
#define 	Columna(x)    ((x)&7)
#define 	Flip(x)       ((x)^1)

#define		U64(numero) ((uint64)numero)
#define		U8(numero) ((uint8)numero)

#define		NTOTAL_PIEZAS		(cuentaBit(juego.tablero[BLANCO][PEON])    + cuentaBit(juego.tablero[NEGRO][PEON])    + \
			    	  	 cuentaBit(juego.tablero[BLANCO][DAMA])    + cuentaBit(juego.tablero[NEGRO][DAMA])    + \
				  	 cuentaBit(juego.tablero[BLANCO][CABALLO]) + cuentaBit(juego.tablero[NEGRO][CABALLO]) + \
				  	 cuentaBit(juego.tablero[BLANCO][TORRE])   + cuentaBit(juego.tablero[NEGRO][TORRE])   + \
				  	 cuentaBit(juego.tablero[BLANCO][ALFIL])   + cuentaBit(juego.tablero[NEGRO][ALFIL]) + 2)

#define		OBT_COLOR_PIEZA(pieza) ((pieza < 9) ? BLANCO:NEGRO)

//#define		ES_REDUCIBLE(a)		(!ES_MOV_PROMOCION(a) && !ES_MOV_CAPTURA(a) && !ES_ESTADO_JUEGO_JAQUE)
#define		ES_REDUCIBLE(a)		(!ES_MOV_PROMOCION(a) && !ES_MOV_CAPTURA(a))

#define		ES_MATADOR_MOV(capa, a)	((juego.historicoMovMatadores[MATADOR_MOV][0][capa]==a) || 		\
					 (juego.historicoMovMatadores[MATADOR_MOV][1][capa]==a)) 

#define		ES_INSUFICIENTE_MATERIAL(es) 									\
	uint64 piezasBlancas = juego.blancos ^ juego.tablero[BLANCO][REY]; 					\
	uint64 piezasNegras  = juego.negros  ^ juego.tablero[NEGRO][REY];					\
														\
	if (!juego.tablero[BLANCO][PEON] && !juego.tablero[NEGRO][PEON]) /* No hay peones en el tablero	*/	\
	{													\
		/* insuficiente material*/									\
		if ((juego.material_lado_blanco < VALOR_TORRE) && (juego.material_lado_negro < VALOR_TORRE))	\
		{												\
			es = VERDADERO;										\
														\
		} else if (!piezasBlancas && !piezasNegras) /* Rey vs Rey */					\
		{												\
			es = VERDADERO;										\
														\
		}else if (((piezasBlancas == juego.tablero[BLANCO][CABALLO]) && !piezasNegras) || /* Rey y Caballo's vs Rey */ \
		    ((piezasNegras == juego.tablero[NEGRO][CABALLO])  && !piezasBlancas))			\
		{												\
			es = VERDADERO;										\
														\
		} else if (juego.material_lado_blanco < VALOR_TORRE) 						\
		{												\
		/*Rey Blanco con material insuficiente Vs rey con dos alfiles del mismo color*/			\
			if ((cuentaBit(juego.tablero[NEGRO][ALFIL] & ESCAQUES_BLANCOS) == 2) ||			\
			    (cuentaBit(juego.tablero[NEGRO][ALFIL] & ESCAQUES_NEGROS)  == 2))			\
			{											\
				es =  VERDADERO;								\
			}											\
		} else if (juego.material_lado_negro < VALOR_TORRE)						\
		{												\
			/*Rey Negro con material insuficiente Vs rey con dos alfiles del mismo color*/		\
			if ((cuentaBit(juego.tablero[BLANCO][ALFIL] & ESCAQUES_BLANCOS) == 2) ||		\
			    (cuentaBit(juego.tablero[BLANCO][ALFIL] & ESCAQUES_NEGROS)  == 2))			\
			{											\
				es = VERDADERO;									\
			}											\
		} 												\
	} else {												\
														\
		es = FALSO;											\
	}	

#define		ES_REPETICION_TABLERO(es)									\
	int i_;													\
	int repeticion_   = 1;      		                                       				\
	int indexBusqRep_ = juego.indiceHJuego - juego.reglaCincuentaMov;  	 				\
	for (i_ = juego.indiceHJuego - 2; i_ >= indexBusqRep_; i_ -= 2)        					\
	{													\
		if (juego.historicoJuego[i_].llaveHash == juego.llaveHash)					\
		{ 												\
			repeticion_++;										\
		}												\
	}													\
														\
	es = (repeticion_ > 1);											\
//Es mejor un empate por repeticion, que generar una jugada que pueda hacernos perder

#define		ES_REPETICION_TABLEROMAIN(es)									\
	int i_;													\
	int repeticion_   = 1;      		                                       				\
	int indexBusqRep_ = juego.indiceHJuego - juego.reglaCincuentaMov;  	 				\
	for (i_ = juego.indiceHJuego - 2; i_ >= indexBusqRep_; i_ -= 2)        					\
	{													\
		if (juego.historicoJuego[i_].llaveHash == juego.llaveHash)					\
		{ 												\
			repeticion_++;										\
		}												\
	}													\
														\
	es = (repeticion_ > 2);											\
		

#define		OBT_MATERIAL(c) ((c) ? -juego.material_total : juego.material_total)

//******************************* MACROS MOVIMIENTOS MANIPULACION DIRECTA *************************************************************

#define FILASATAQUEX(a, oo, dd)         (RANK_ATTACKS[(a)][((oo & RANKMASK[(a)]) >> RANKSHIFT[(a)])] & dd)
#define COLUMNASATAQUEX(a,oo,dd)        (FILE_ATTACKS[(a)][((oo & FILEMASK[(a)]) * FILEMAGIC[(a)]) >> 57] & dd)

#define DIAGONALA8H1ATAQUEX(a,oo,dd)(DIAGA8H1_ATTACKS[(a)][((oo & DIAGA8H1MASK[(a)]) * DIAGA8H1MAGIC[(a)]) >> 57] & dd)
#define DIAGONALA1H8ATAQUEX(a,oo,dd)(DIAGA1H8_ATTACKS[(a)][((oo & DIAGA1H8MASK[(a)]) * DIAGA1H8MAGIC[(a)]) >> 57] & dd)

#define genTorreAtaqueTableroX(a,oo,dd) (FILASATAQUEX(a, oo, dd) | COLUMNASATAQUEX(a,oo, dd))
#define genAlfilAtaqueTableroX(a,oo,dd) (DIAGONALA8H1ATAQUEX(a,oo,dd) | DIAGONALA1H8ATAQUEX(a,oo,dd))


//******************************* MACROS MOVIMIENTOS MAS RAPIDOS *************************************************************

#define FILASATAQUE(a, t)          (RANK_ATTACKS[(a)][((t.ocupados & RANKMASK[(a)]) >> RANKSHIFT[(a)])] & t.destinos)
#define COLUMNASATAQUE(a,t)        (FILE_ATTACKS[(a)][((t.ocupados & FILEMASK[(a)]) * FILEMAGIC[(a)]) >> 57] & t.destinos)
#define DIAGONALA8H1ATAQUE(a,t)    (DIAGA8H1_ATTACKS[(a)][((t.ocupados & DIAGA8H1MASK[(a)]) * DIAGA8H1MAGIC[(a)]) >> 57] & t.destinos)
#define DIAGONALA1H8ATAQUE(a,t)    (DIAGA1H8_ATTACKS[(a)][((t.ocupados & DIAGA1H8MASK[(a)]) * DIAGA1H8MAGIC[(a)]) >> 57] & t.destinos)
#define genTorreAtaqueTablero(a,t) (FILASATAQUE(a, t) | COLUMNASATAQUE(a,t))
#define genAlfilAtaqueTablero(a,t) (DIAGONALA8H1ATAQUE(a,t) | DIAGONALA1H8ATAQUE(a,t))
#define genDamaAtaqueTablero(a,t)  (genAlfilAtaqueTablero(a,t) | genTorreAtaqueTablero(a,t))

#define	genReyAtaqueTablero(escaque, t) 	(mascaraRey[escaque]     & t.destinos)
#define genCaballoAtaqueTablero(escaque, t) 	(mascaraCaballo[escaque] & t.destinos)

//******************************* FIN MACROS MOVIMIENTOS MAS RAPIDOS *************************************************************

//********************************* MACROS USADOS EN EET **************************************************************************
#define FILASMOVATAQUE(a, t)          (RANK_ATTACKS[(a)][((t.ocupados & RANKMASK[(a)]) >> RANKSHIFT[(a)])])
#define COLUMNASMOVATAQUE(a,t)        (FILE_ATTACKS[(a)][((t.ocupados & FILEMASK[(a)]) * FILEMAGIC[(a)]) >> 57])
#define DIAGONALA8H1MOVATAQUE(a,t)    (DIAGA8H1_ATTACKS[(a)][((t.ocupados & DIAGA8H1MASK[(a)]) * DIAGA8H1MAGIC[(a)]) >> 57])
#define DIAGONALA1H8MOVATAQUE(a,t)    (DIAGA1H8_ATTACKS[(a)][((t.ocupados & DIAGA1H8MASK[(a)]) * DIAGA1H8MAGIC[(a)]) >> 57])
#define genTorreMOVAtaqueTablero(a,t) (FILASMOVATAQUE(a, t) | COLUMNASMOVATAQUE(a,t))
#define genAlfilMOVAtaqueTablero(a,t) (DIAGONALA8H1MOVATAQUE(a,t) | DIAGONALA1H8MOVATAQUE(a,t))

//*********************************************************************************************************************************

#define		OBT_STR_COLOR_TURNO ((juego.colorTurno?"Negro":"Blanco"))
#define		IMPRIMIR_COLOR_TURNO printf("color: %s\n", OBT_STR_COLOR_TURNO)
#define		DEPURACION_VER_MOV IMPRIMIR_COLOR_TURNO;verMov2(juego.Buffer_MOV[i]);mostrarTablero();

//************************************************* MACROS DE TECLAS **************************************************************

#define		TECLA_ESC		27

//****************************************************** EET **********************************************************************
#define		OESTE		       -1
#define		NOR_OESTE		7
#define		NORTE			8
#define		NOR_ESTE		9
#define		ESTE			1
#define		SUR_ESTE	       -7
#define		SUR		       -8
#define		SUR_OESTE	       -9

//******************************************************** CODIGOS DE ESTADOS *******************************************************

#define		CODESTADO_PIEZA_TRANQUILA	3
#define		CODESTADO_PIEZA_AMIGA_ATACADA	4
#define		CODESTADO_PIEZA_ENEMIGA_ATACADA	5

//******************************************************* DESAMBIGUACION**************************************************************

#define		NO_AMBIGUA			0
#define		DESAMBIGUACION_POR_FILA		1
#define		DESAMBIGUACION_POR_RANK		2
#define		ESABG(mov) (esMovimientoAmbiguo(mov))

//********************************************************* ESTADOS DEL JUEGO *******************************************************

#define 	ES_ESTADO_JUEGO_JAQUE 		(esAtacadoPor(juego.tablero[juego.colorTurno][REY],!juego.colorTurno))
#define		FINAL_JUEGO_JAQUE_MATE 		4
#define		FINAL_JUEGO_AHOGADO 		3
#define		FINAL_JUEGO_FRONTERA 		2
#define		NO_ES_FINAL_JUEGO_HAY_JAQUE	1
#define		NO_ES_FINAL_JUEGO 		0
#define		ES_ETAPA_FINAL			 ((juego.material_lado_blanco < LIMITE_INFERIOR_FINAL_JUEGO) || \
 			   			  (juego.material_lado_negro  < LIMITE_INFERIOR_FINAL_JUEGO))

#define 	movAnterior 			juego.historicoJuego[juego.indiceHJuego-2].mov

#define		PUNTAJE_MEDIO_JUEGO		4105 //8P+2T+2C+2A+D
#define		PUNTAJE_FINAL_JUEGO		3065 //4T+A+C+4P

#define		ALFA_BETA_PROXIMO_MATE		((alfa < VALOR_BAJO || alfa > VALOR_ALTO) || (beta < VALOR_BAJO || beta > VALOR_ALTO))

#define		ES_PIEZA(a)			(((a > 0) && (a < 4)) || ((a > 4) && (a < 8)) || ((a > 8) && (a < 12)) || ((a > 12) && (a < 16)))


#define		STATUS_FIN_JUEGO_SIN_ESTATUS		0
#define		STATUS_FIN_JUEGO_JAQUE			1
#define		STATUS_FIN_JUEGO_MATE_GANAN_B		2
#define		STATUS_FIN_JUEGO_MATE_GANAN_N		3
#define		STATUS_FIN_JUEGO_TABLAS			4
#define		STATUS_FIN_JUEGO_TABLAS_INSUFI_MATERIAL	6
#define		STATUS_FIN_JUEGO_TABLAS_REPETICION	7
#define		STATUS_FIN_JUEGO_TABLAS_REGLA_50MOV	8
#define		STATUS_FIN_JUEGO_INDETERMINADO		9
#define		STATUS_FIN_JUEGO_TERMINADO		(statusFinJuego > 1)
#define		STATUS_FIN_JUEGO_NO_TERMINADO		(statusFinJuego < 2)

#define		TABLERO_VISTA_INICIAL		"nbnbnbnbbnbnbnbnnbnbnbnbbnbnbnbnnbnbnbnbbnbnbnbnnbnbnbnbbnbnbnbn"

//********************************************************* UTILES *******************************************************

#define		MAXIMO(a,b) 			(a>b?a:b)
#define		MINIMO(a,b) 			(a<b?a:b)

#define		CABEZA_ESQ(c, mapa) ((c)?bitScanForwardBruijn(mapa):bitScanLast(mapa))

#define		PARAR_Y_VER(c) if (c){printf("Corte \n");mostrarTablero();exit(0);}

#define		de_Enfrente(c,r,p) ((c)? r<p:r>p)
#define		de_Atras(c,r,p)    ((c)? r>p:r<p)

#define FileDistancia(a,b) abs(FILES[a] - FILES[b])
#define RankDistancia(a,b) abs(RANKS[a] - RANKS[b])


#endif
