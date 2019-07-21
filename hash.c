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

/*
	Llave Hash de 26 bits = posiciones 67.108.863 * (24 Bytes del Registro Tabla Hash) = 1536 MB
	Llave Hash de 25 bits = posiciones 33.554.431 * (24 Bytes del Registro Tabla Hash) =  768 MB
	Llave Hash de 24 bits = posiciones 16.777.215 * (24 Bytes del Registro Tabla Hash) =  384 MB
	Llave Hash de 23 bits = posiciones  8.388.607 * (24 Bytes del Registro Tabla Hash) =  192 MB
	Llave Hash de 22 bits = posiciones  4.194.303 * (24 Bytes del Registro Tabla Hash) =   96 MB
	Llave Hash de 21 bits = posiciones  2.097.151 * (24 Bytes del Registro Tabla Hash) =   48 MB
*/


#ifndef HASH_C
#define HASH_C

void iniciarHash()
{
	// inicializar todos los 64-bit numeros al azar

	int i;
//	time_t now;

//	srand((unsigned int)time(&now));

	for (i = 0; i < 64; i++)
	{
		arrayHash.ep[i] = enpassant_random[i];//rand64();
		arrayHash.llaves[i][PEON_BLANCO] = randoms[BLANCO][1][i];
		arrayHash.llaves[i][CABALLO_BLANCO] = randoms[BLANCO][2][i];
		arrayHash.llaves[i][ALFIL_BLANCO] = randoms[BLANCO][3][i];
		arrayHash.llaves[i][TORRE_BLANCO] = randoms[BLANCO][4][i];
		arrayHash.llaves[i][DAMA_BLANCO] = randoms[BLANCO][5][i];
		arrayHash.llaves[i][REY_BLANCO] = randoms[BLANCO][6][i];
		arrayHash.llaves[i][PEON_NEGRO] = randoms[NEGRO][1][i];
		arrayHash.llaves[i][CABALLO_NEGRO] = randoms[NEGRO][2][i];
		arrayHash.llaves[i][ALFIL_NEGRO] = randoms[NEGRO][3][i];
		arrayHash.llaves[i][TORRE_NEGRO] = randoms[NEGRO][4][i];
		arrayHash.llaves[i][DAMA_NEGRO] = randoms[NEGRO][5][i];
		arrayHash.llaves[i][REY_NEGRO] = randoms[NEGRO][6][i];
	}

	arrayHash.lado 	= 0x0002000204081abcull;//rand64();
	arrayHash.OOB 	= castle_random[BLANCO][0]; //rand64();
	arrayHash.OOOB 	= castle_random[BLANCO][1]; //rand64();
	arrayHash.OON 	= castle_random[NEGRO][0];  //rand64();
	arrayHash.OOON 	= castle_random[NEGRO][1];  //rand64();

}

uint64 rand64()
{
	return rand()^((uint64)rand()<<15)^((uint64)rand()<<30)^((uint64)rand()<<45)^((uint64)rand()<<60);
}

void agregarEvalTablaHash(int valor)
{
	HASH_EVAL *ptabla 	= hash_eval + (juego.llaveHash & LARGO_HASH_EVAL);

	ptabla->id 		= juego.llaveHash;
	ptabla->valor		= valor;
}

BOOLEANO verificarEvalTablaHash(int *valor)
{
	HASH_EVAL *ptabla 	= hash_eval + (juego.llaveHash & LARGO_HASH_EVAL);
	BOOLEANO esEncontrado 	= FALSO;

	if (ptabla->id == juego.llaveHash)
	{
		*valor 		= ptabla->valor;
		esEncontrado 	= VERDADERO;
	}

	return esEncontrado;	
}


//**************************************************************************

void agregarMovTablaHash(int profundidad, int capa, int valor, int banderas, MOVIMIENTO mov)
{
	REGISTRO_TABLA_HASH *ptabla 	= tabla_hash + (juego.llaveHash & LARGO_TABLA_HASH);

	if ((ptabla->profundidad > profundidad) && (ptabla->id == juego.llaveHash)) return;

	if ((valor > VALOR_ALTO) || (valor < VALOR_BAJO))
	{
		if (valor > 0)
		{
			valor += capa;
		} else {
			valor -= capa;
		}
	}

	ptabla->id 		= (uint64)juego.llaveHash;
	ptabla->profundidad	= profundidad;
	ptabla->puntaje		= valor;
	ptabla->mov		= mov;
	ptabla->banderas	= banderas;
}

int verificarTablaHash(int alfa, int beta, int capa, int profundidad, int *banderas, MOVIMIENTO *mov)
{
	REGISTRO_TABLA_HASH *ptabla = tabla_hash + (juego.llaveHash & LARGO_TABLA_HASH);

	int valor = 0;
	int flag  = 0; 

	*banderas = BANDERA_HASH_VACIO;

	if (ptabla->id == juego.llaveHash)
	{
		valor 		= ptabla->puntaje;
		flag	 	= ptabla->banderas;
		*mov		= ptabla->mov;

		if ((valor > VALOR_ALTO) || (valor < VALOR_BAJO))
		{
			if (valor > 0)
			{
				valor -= capa;
			} else {
				valor += capa;
			}
		}

		if ((ptabla->profundidad >= profundidad-REDUCCION_MOV_NULL) && (valor < beta) && (flag == BANDERA_HASH_ARRIBA))
		{
			*banderas = BANDERA_HASH_EVITAR_NULL;
		}

		if (ptabla->profundidad > profundidad)
		{
			if ((flag == BANDERA_HASH_ARRIBA) && (valor <=alfa))
			{
				*banderas = BANDERA_HASH_ARRIBA;
				return alfa;
			}

			if ((flag == BANDERA_HASH_ABAJO) && (valor >= beta))
			{
				*banderas = BANDERA_HASH_ABAJO;
				return beta;
			}

			if (flag == BANDERA_HASH_EXACTO)
			{
				*banderas = BANDERA_HASH_EXACTO;
				return valor;
			}
		}
	}

	return 0;	
}


#endif
